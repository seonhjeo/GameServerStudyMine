#include <cstring>

#include "NetLib/ILog.h"
#include "NetLib/TcpNetwork.h"
#include "NetLib/Define.h"
#include "User.h"
#include "UserManager.h"
#include "Room.h"
#include "RoomManager.h"
#include "PacketProcess.h"


using LOG_TYPE = NServerNetLib::LOG_TYPE;
using ServerConfig = NServerNetLib::ServerConfig;

	
PacketProcess::PacketProcess() {}
PacketProcess::~PacketProcess() {}

void PacketProcess::Init(TcpNet* pNetwork, UserManager* pUserMgr, RoomManager* pLobbyMgr, ServerConfig* pConfig, ILog* pLogger)
{
	m_pRefLogger = pLogger;
	m_pRefNetwork = pNetwork;
	m_pRefUserMgr = pUserMgr;
	m_pRefRoomMgr = pLobbyMgr;
	m_pConfig = pConfig;
}

#include <iostream>

// 대부분의 네트워킹 작업이 이뤄지는 공간
// 받은 패킷에 따라 적절한 함수를 호출해준다.
void PacketProcess::Process(PacketInfo packetInfo)
{
	using netLibPacketId = NServerNetLib::PACKET_ID;
	using commonPacketId = PACKET_ID;

	auto packetId = packetInfo.PacketId;
	
	// CONNECT와 CLOSE는 클라이언트가 실제로 패킷을 만들지 않는 프로세스
	// 네트워크 라이브러이에서 더미 패킷을 만들어다 전달함
	switch (packetId)
	{
	case (int)netLibPacketId::NTF_SYS_CONNECT_SESSION:
		NtfSysConnctSession(packetInfo);
		break;
	case (int)netLibPacketId::NTF_SYS_CLOSE_SESSION:
		NtfSysCloseSession(packetInfo);
		break;
	case (int)commonPacketId::LOGIN_IN_REQ:
		Login(packetInfo);
		break;
	case (int)commonPacketId::ROOM_ENTER_REQ:
		EnterRoom(packetInfo);
		break;
	case (int)commonPacketId::ROOM_LEAVE_REQ:
		LeaveRoom(packetInfo);
		break;
	case (int)commonPacketId::ROOM_CHAT_REQ:
		ChatRoom(packetInfo);
		break;
	case (int)commonPacketId::GAME_STONE_REQ:
		PutStone(packetInfo);
	}
}


ERROR_CODE PacketProcess::NtfSysConnctSession(PacketInfo packetInfo)
{
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);
	return ERROR_CODE::NONE;
}

ERROR_CODE PacketProcess::NtfSysCloseSession(PacketInfo packetInfo)
{
	auto pUser = std::get<1>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));

	if (pUser)
		m_pRefUserMgr->RemoveUser(packetInfo.SessionIndex);		
			
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysCloseSesson. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);
	return ERROR_CODE::NONE;
}

ERROR_CODE PacketProcess::Login(PacketInfo packetInfo)
{
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);

	PktLogInRes resPkt;
	auto reqPkt = (PktLogInReq*)packetInfo.pRefData;

	// 에러체크 : 받은 데이터의 크기 확인
	if (sizeof(*reqPkt) != sizeof(PktLogInReq))
		return (SendError(packetInfo, resPkt, ERROR_CODE::UNASSIGNED_ERROR, PACKET_ID::LOGIN_IN_RES));

	auto addRet = m_pRefUserMgr->AddUser(packetInfo.SessionIndex, reqPkt->szID);
	if (addRet != ERROR_CODE::NONE)
		return (SendError(packetInfo, resPkt, addRet, PACKET_ID::LOGIN_IN_RES));
	
	return (SendError(packetInfo, resPkt, ERROR_CODE::NONE, PACKET_ID::LOGIN_IN_RES));
}

ERROR_CODE PacketProcess::EnterRoom(PacketInfo packetInfo)
{
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);

	PktRoomEnterRes resPkt;
	auto reqPkt = (PktRoomEnterReq*)packetInfo.pRefData;
	if (sizeof(*reqPkt) != sizeof(PktRoomEnterReq))
		return (SendError(packetInfo, resPkt, ERROR_CODE::UNASSIGNED_ERROR, PACKET_ID::ROOM_ENTER_RES));

	// 에러체크 : 유저가 이미 방에 들어가있는지 확인
	auto pUser = std::get<1>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));
	if (pUser->GetRoomIndex() > -1)
		return (SendError(packetInfo, resPkt, ERROR_CODE::USER_MGR_ALREADY_IN_ROOM, PACKET_ID::ROOM_ENTER_RES));
	
	// 에러체크: 방 번호가 서버가 생성한 방번호 보다 큰지 확인
	if (reqPkt->RoomIndex >= m_pConfig->MaxRoomCount)
		return (SendError(packetInfo, resPkt, ERROR_CODE::ROOM_MGR_NUMBER_OVER_MAX, PACKET_ID::ROOM_ENTER_RES));

	// 실행 : 방 입장
	Room* pRoom = nullptr;
	if (reqPkt->RoomIndex == -1)
	{
		pRoom = m_pRefRoomMgr->GetRandomRoom();
		if (pRoom == nullptr)
			return (SendError(packetInfo, resPkt, ERROR_CODE::ROOM_MGR_ENTER_ROOM_FAIL, PACKET_ID::ROOM_ENTER_RES));
	}
	else
	{
	    //방번호의 방 객체를 얻는다.
		pRoom = m_pRefRoomMgr->GetRoom(reqPkt->RoomIndex);
		if (pRoom == nullptr)
			return (SendError(packetInfo, resPkt, ERROR_CODE::ROOM_MGR_ENTER_ROOM_FAIL, PACKET_ID::ROOM_ENTER_RES));

	    //방에 들어간 인원이 이미 다 찬 상태이면 실패이다.
		if (pRoom->IsFull() == true)
			return (SendError(packetInfo, resPkt, ERROR_CODE::ROOM_MGR_ROOM_ALREADY_FULL, PACKET_ID::ROOM_ENTER_RES));
	}
	
	// 위에서 성공한 경우
	// 현재 방 인원수가 2명 이상이면 이미 다른 사람이 있는 방에 입장한 것이므로 기존 사람들에게
	// PK_NEW_USER_ENTER_ROOM_NTF 패킷을 보낸다.
	//원하는 방번호를 가진 방에 입장시킨다.
	pRoom->UserEnterRoom(pUser);
	pUser->EnterRoom(pRoom->GetIndex());

	resPkt.SetError(ERROR_CODE::NONE);
	resPkt.UniqueId = pUser->GetIndex();
	m_pRefNetwork->SendData(packetInfo.SessionIndex, (short)PACKET_ID::ROOM_ENTER_RES, sizeof(PktRoomEnterRes), (char*)&resPkt);

	pRoom->BroadCastOtherJoin(pUser);
	pRoom->GetListOtherUser(pUser);

	return ERROR_CODE::NONE;
}

//TODO: 방 나가기
ERROR_CODE PacketProcess::LeaveRoom(PacketInfo packetInfo)
{
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);

	PktRoomLeaveRes resPkt;

	// 유저가 방에 들어온 상태가 맞는지 확인한다
	auto pUser = std::get<User *>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));
	if (pUser->GetRoomIndex() == -1)
		return (SendError(packetInfo, resPkt, ERROR_CODE::USER_MGR_ALREADY_IN_ROOM, PACKET_ID::ROOM_LEAVE_RES));

	// 유저가 들어간 방의 객체를 가져온다
	auto pRoom = m_pRefRoomMgr->GetRoom(pUser->GetRoomIndex());
	if (pRoom == nullptr)
		return (SendError(packetInfo, resPkt, ERROR_CODE::ROOM_MGR_NO_SUCH_ROOM, PACKET_ID::ROOM_LEAVE_RES));

	// 방에서 유저를 빼고, 유저 상태도 변경한다.
	pRoom->UserLeaveRoom(pUser->GetRoomIndex());
	pUser->SetRoomIndex(-1);

	// 아직 방에 다른 유저가 있다면 PK_USER_LEAVE_ROOM_NTF을 보낸다
	if (pRoom->GetUserCount() != 0)
		pRoom->BroadCastOtherLeave(pUser);

	return (SendError(packetInfo, resPkt, ERROR_CODE::NONE, PACKET_ID::ROOM_LEAVE_RES));
}

//TODO: 방 채팅
ERROR_CODE PacketProcess::ChatRoom(PacketInfo packetInfo)
{
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);

	PktRoomChatRes resPkt;
	auto reqPkt = (PktRoomChatReq*)packetInfo.pRefData;

	// 유저가 방에 들어온 상태가 맞는지 확인한다
	auto pUser = std::get<User*>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));
	if (pUser->GetRoomIndex() == -1)
		return (SendError(packetInfo, resPkt, ERROR_CODE::USER_MGR_NOT_IN_ROOM, PACKET_ID::ROOM_CHAT_RES));

	// 유저가 들어간 방의 객체를 가져온다
	auto pRoom = m_pRefRoomMgr->GetRoom(pUser->GetRoomIndex());
	if (pRoom == nullptr)
		return (SendError(packetInfo, resPkt, ERROR_CODE::ROOM_MGR_NO_SUCH_ROOM, PACKET_ID::ROOM_CHAT_RES));

	// 채팅 메시지에 문제가 없다면 요청이 성공함을 알린다
	std::string Msg;
	Msg.assign(reqPkt->Msg, 0, reqPkt->Msglen);
	pRoom->BroadCastOtherChat(pUser, Msg);

	return (SendError(packetInfo, resPkt, ERROR_CODE::NONE, PACKET_ID::ROOM_CHAT_RES));
}

// TODO : 게임 시작 및 종료
ERROR_CODE PacketProcess::StartGame(PacketInfo packetInfo)
{
	// 해당 방에 2명이 아니면 ERR_ROOM_MGR_USER_NOT_FULL
	return ERROR_CODE::NONE;
}

ERROR_CODE PacketProcess::FinishGame(PacketInfo packetInfo)
{
	return ERROR_CODE::NONE;
}

// TODO : 돌 놓기
ERROR_CODE PacketProcess::PutStone(PacketInfo packetInfo)
{
	m_pRefLogger->Write(LOG_TYPE::L_INFO, "%s | NtfSysConnctSession. sessionIndex(%d)", __FUNCTION__, packetInfo.SessionIndex);

	PktGameStoneRes resPkt;
	auto reqPkt = (PktGameStoneReq*)packetInfo.pRefData;

	auto pUser = std::get<User*>(m_pRefUserMgr->GetUser(packetInfo.SessionIndex));
	auto pRoom = m_pRefRoomMgr->GetRoom(pUser->GetRoomIndex());
	auto pGame = pRoom->GetGame();

	ERROR_CODE resCode = pGame->PutStone(reqPkt->x, reqPkt->y, pUser->GetRoomIndex());
	if (resCode == ERROR_CODE::GAME_MGR_PLAYER_WIN)
	{
		std::string Msg = "Player " + pGame->GetNowPlayer();
		Msg += " Win!";
		pRoom->BroadCastOtherChat(pUser, Msg);
		return ERROR_CODE::NONE;
	}
	else
		return SendError(packetInfo, resPkt, resCode, PACKET_ID::GAME_STONE_RES);
}

ERROR_CODE PacketProcess::SendError(PacketInfo info, PktBase packet, ERROR_CODE err_code, PACKET_ID packetId)
{
	packet.SetError(err_code);
	m_pRefNetwork->SendData(info.SessionIndex, (short)packetId, sizeof(packet), (char*)&packet);
	return err_code;
}