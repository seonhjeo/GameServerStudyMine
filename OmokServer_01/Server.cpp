#include <thread>
#include <chrono>

#include "NetLib/ServerNetErrorCode.h"
#include "NetLib/Define.h"
#include "NetLib/TcpNetwork.h"
#include "ConsoleLogger.h"
#include "RoomManager.h"
#include "PacketProcess.h"
#include "UserManager.h"
#include "Server.h"

using NET_ERROR_CODE = NServerNetLib::NET_ERROR_CODE;
using LOG_TYPE = NServerNetLib::LOG_TYPE;

Server::Server()
{
}

Server::~Server()
{
	Release();
}

ERROR_CODE Server::Init()
{
	// 로그 정보 클래스 생성
	m_pLogger = std::make_unique<ConsoleLog>();

	// 설정 정보 지정
	LoadConfig();

	m_pNetwork = std::make_unique<NServerNetLib::TcpNetwork>();
	auto result = m_pNetwork->Init(m_pServerConfig.get(), m_pLogger.get());

	if (result != NET_ERROR_CODE::NONE)
	{
		m_pLogger->Write(LOG_TYPE::L_ERROR, "%s | Init Fail. NetErrorCode(%s)", __FUNCTION__, (short)result);
		return ERROR_CODE::MAIN_INIT_NETWORK_INIT_FAIL;
	}


	m_pUserMgr = std::make_unique<UserManager>();
	m_pUserMgr->Init(m_pServerConfig->MaxClientCount);

	m_pRoomMgr = std::make_unique<RoomManager>();
	m_pRoomMgr->Init(m_pServerConfig->MaxRoomCount, m_pServerConfig->MaxRoomUserCount);
	m_pRoomMgr->SetNetwork(m_pNetwork.get(), m_pLogger.get());

	m_pPacketProc = std::make_unique<PacketProcess>();
	m_pPacketProc->Init(m_pNetwork.get(), m_pUserMgr.get(), m_pRoomMgr.get(), m_pServerConfig.get(), m_pLogger.get());

	m_IsRun = true;

	m_pLogger->Write(LOG_TYPE::L_INFO, "%s | Init Success. Server Run", __FUNCTION__);
	return ERROR_CODE::NONE;
}

void Server::Release()
{
	if (m_pNetwork) {
		m_pNetwork->Release();
	}
}

void Server::Stop()
{
	m_IsRun = false;
}

void Server::Run()
{
	while (m_IsRun)
	{
		// 서버 내에서 네트워크도 같이 run하는 중
		m_pNetwork->Run();

		while (true)
		{	
			// 패킷 확인
			auto packetInfo = m_pNetwork->GetPacketInfo();

			// 받아온 패킷이 없으면 종료
			if (packetInfo.PacketId == 0)
			{
				break;
			}
			// 받아온 패킷이 있으면 패킷 처리
			else
			{
				m_pPacketProc->Process(packetInfo);
			}
		}
	}
}

// TODO : ini 파일에서 값을 끌어들이도록 하기
ERROR_CODE Server::LoadConfig()
{
	m_pServerConfig = std::make_unique<NServerNetLib::ServerConfig>();

	m_pServerConfig->Port = 11021;
	m_pServerConfig->BackLogCount = 128;
	m_pServerConfig->MaxClientCount = 1000;

	// 커널 쪽 소켓의 버퍼
	m_pServerConfig->MaxClientSockOptRecvBufferSize = 10240;
	m_pServerConfig->MaxClientSockOptSendBufferSize = 10240;
	// 클라이언트쪽 소켓의 버퍼
	m_pServerConfig->MaxClientRecvBufferSize = 8192;
	m_pServerConfig->MaxClientSendBufferSize = 8192;

	m_pServerConfig->IsLoginCheck = false;

	m_pServerConfig->ExtraClientCount = 64;
	m_pServerConfig->MaxRoomCount = 20;
	m_pServerConfig->MaxRoomUserCount = 2;
	    
	m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | Port(%d), Backlog(%d)", __FUNCTION__, m_pServerConfig->Port, m_pServerConfig->BackLogCount);
	m_pLogger->Write(NServerNetLib::LOG_TYPE::L_INFO, "%s | IsLoginCheck(%d)", __FUNCTION__, m_pServerConfig->IsLoginCheck);
	return ERROR_CODE::NONE;
}
		
