﻿#include <algorithm>
#include <cstring>
#include <wchar.h>

#include "NetLib/ILog.h"
#include "NetLib/TcpNetwork.h"
#include "Packet.h"
#include "ErrorCode.h"
#include "User.h"
#include "Room.h"



Room::Room() {}

Room::~Room()
{
}
	
void Room::Init(const short index, const short maxUserCount)
{
	m_Index = index;
	m_MaxUserCount = maxUserCount;
}

void Room::SetNetwork(TcpNet* pNetwork, ILog* pLogger)
{
	m_pRefLogger = pLogger;
	m_pRefNetwork = pNetwork;
}

void Room::Clear()
{
	m_IsUsed = false;
	m_UserList.clear();
}

bool Room::IsFull()
{
	if (m_UserList.size() == m_MaxUserCount)
		return (true);
	else
		return (false);
}

bool Room::UserLeaveRoom(const short userIndex)
{
	std::vector<User*>::iterator it = m_UserList.begin();
	for (; it != m_UserList.end(); it++)
	{
		if ((*it)->GetIndex() == userIndex)
		{
			(*it)->SetRoomIndex(-1);
			m_UserList.erase(it);
			if (m_UserList.size() == 0)
				Clear();
			return (true);
		}
	}
	return (false);
}

bool Room::UserEnterRoom(User *user)
{
	if (IsFull() == false)
	{
		m_UserList.push_back(user);
		if (m_IsUsed == false)
			m_IsUsed = true;
		return (true);
	}
	return (false);
}

void Room::BroadCastOtherJoin(User* other)
{
	PktRoomEnterUserInfoNtf pktRes;

	pktRes.uniqueId = other->GetIndex();
	pktRes.idlen = (char)other->GetID().size();
	memcpy(pktRes.UserID, other->GetID().c_str(), pktRes.idlen);

	for (User* element : m_UserList)
	{
		if (element != other)
			m_pRefNetwork->SendData(element->GetSessionIndex(), (short)PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(PktRoomEnterUserInfoNtf), (char*)&pktRes);
	}
}

void Room::GetListOtherUser(User* other)
{
	PktRoomEnterUserInfoNtf pktRes;

	for (User* element : m_UserList)
	{
		pktRes.uniqueId = element->GetIndex();
		pktRes.idlen = (char)element->GetID().size();
		memcpy(pktRes.UserID, element->GetID().c_str(), pktRes.idlen);
		m_pRefNetwork->SendData(other->GetSessionIndex(), (short)PACKET_ID::ROOM_ENTER_NEW_USER_NTF, sizeof(PktRoomEnterUserInfoNtf), (char*)&pktRes);
	}
}

void Room::BroadCastOtherLeave(User* other)
{
	PktRoomLeaveUserInfoNtf pktRes;

	pktRes.uniqueId = other->GetIndex();
	pktRes.idlen = other->GetID().size();
	memcpy(pktRes.UserID, other->GetID().c_str(), pktRes.idlen);
	for (User* element : m_UserList)
		m_pRefNetwork->SendData(element->GetSessionIndex(), (short)PACKET_ID::ROOM_LEAVE_USER_NTF, sizeof(PktRoomLeaveUserInfoNtf), (char*)&pktRes);
}

void Room::BroadCastOtherChat(User* other, std::string msg)
{
	PktRoomChatNtf pktRes;

	pktRes.uniqueId = other->GetIndex();
	pktRes.msgLen = msg.size();
	memcpy(pktRes.Msg, msg.c_str(), msg.size());

	for (User* element : m_UserList)
		m_pRefNetwork->SendData(element->GetSessionIndex(), (short)PACKET_ID::ROOM_CHAT_NTF, sizeof(PktRoomChatNtf), (char*)&pktRes);
}