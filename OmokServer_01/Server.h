#pragma once
#include <memory>

#include "Packet.h"
#include "ErrorCode.h"


namespace NServerNetLib
{
	struct ServerConfig;
	class ILog;
	class ITcpNetwork;
}


class UserManager;
class RoomManager;
class PacketProcess;

class Server
{
public:
	Server();
	~Server();

	ERROR_CODE Init();

	void Run();

	void Stop();


private:
	ERROR_CODE LoadConfig();

	void Release();


private:
	bool m_IsRun = false;

	/* 서버에서 사용할 모든 구조체들 */
	// 서버 설정(수용량, 로그인 변수 등)정보
	std::unique_ptr<NServerNetLib::ServerConfig> m_pServerConfig;
	// 로그 정보
	std::unique_ptr<NServerNetLib::ILog> m_pLogger;

	// 네트워크 기능 클래스
	std::unique_ptr<NServerNetLib::ITcpNetwork> m_pNetwork;
	// 패킷 프로세스 클래스
	std::unique_ptr<PacketProcess> m_pPacketProc;
	// 유저 매니저(로그인된 클라이언트의 정보를 담고 있음)
	std::unique_ptr<UserManager> m_pUserMgr;
	// 룸 매니저(로비-룸 방식(MO방식)을 차용했기 때문에 
	std::unique_ptr<RoomManager> m_pRoomMgr;
		
};

