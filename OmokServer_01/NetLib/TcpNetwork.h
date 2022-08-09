#pragma once

#ifdef _WIN32
#define FD_SETSIZE 5096 // http://blog.naver.com/znfgkro1/220175848048
#else
// 리눅스에서는 1024까지만  https://qiita.com/fujinochan/items/2337ce48a998cf67966b
#endif

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>


#include <vector>
#include <deque>
#include <unordered_map>
#include "ITcpNetwork.h"


namespace NServerNetLib
{
	class TcpNetwork : public ITcpNetwork
	{
	public:
		TcpNetwork();
		virtual ~TcpNetwork();
		// 초기화 함수
		NET_ERROR_CODE Init(const ServerConfig* pConfig, ILog* pLogger) override;
		
		// 특정 클라이언트에 패킷을 보낼 때 사용되는 함수
		// seesionIndex : 특정 클라이언트 번호, packetId : 패킷 ID, size : 바디 사이즈, pMsg : 바디 데이터
		NET_ERROR_CODE SendData(const int sessionIndex, const short packetId, const short size, const char* pMsg) override;
		
		// 네트워크에서 발생한 이벤트를 가져오기 위한 함수
		// 이벤트는 크게 세 가지(새 접속 발생, 접속이 끊어짐, 접속한 클라이언트에서 패킷 전송)
		void Run() override;
		
		// 전송받은 패킷을 처리하는 함수
		RecvPacketInfo GetPacketInfo() override;

		// 서버를 종료할 때 호출하는 함수
		void Release() override;

		int ClientSessionPoolSize() override { return (int)m_ClientSessionPool.size(); }

		void ForcingClose(const int sessionIndex) override;


	protected:
		NET_ERROR_CODE InitServerSocket();
		NET_ERROR_CODE BindListen(short port, int backlogCount);
		
		int AllocClientSessionIndex();
		void ReleaseSessionIndex(const int index);

		int CreateSessionPool(const int maxClientCount);
		NET_ERROR_CODE NewSession();
		void SetSockOption(const SOCKET fd);
		void ConnectedSession(const int sessionIndex, const SOCKET fd, const char* pIP);
		
		void CloseSession(const SOCKET_CLOSE_CASE closeCase, const SOCKET sockFD, const int sessionIndex);
		
		NET_ERROR_CODE RecvSocket(const int sessionIndex);
		NET_ERROR_CODE RecvBufferProcess(const int sessionIndex);
		void AddPacketQueue(const int sessionIndex, const short pktId, const short bodySize, char* pDataPos);
		
		void RunProcessWrite(const int sessionIndex, const SOCKET fd, fd_set& write_set);
		NetError FlushSendBuff(const int sessionIndex);
		NetError SendSocket(const SOCKET fd, const char* pMsg, const int size);

		bool RunCheckSelectResult(const int result);
		void RunCheckSelectClients(fd_set& read_set, fd_set& write_set);
		bool RunProcessReceive(const int sessionIndex, const SOCKET fd, fd_set& read_set);

		NET_ERROR_CODE SetNonBlockSocket(const SOCKET sock);
		
	protected:
		ServerConfig m_Config;
				
		SOCKET m_ServerSockfd;
        SOCKET m_MaxSockFD = 0;

		fd_set m_Readfds;
		size_t m_ConnectedSessionCount = 0;
		
		int64_t m_ConnectSeq = 0;
		
		std::vector<ClientSession> m_ClientSessionPool;
		std::deque<int> m_ClientSessionPoolIndex;
		
		std::deque<RecvPacketInfo> m_PacketQueue;

		ILog* m_pRefLogger;
	};
}
