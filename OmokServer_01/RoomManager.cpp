#include <algorithm>

#include "NetLib/ILog.h"
#include "NetLib/TcpNetwork.h"
#include "Packet.h"
#include "ErrorCode.h"
#include "User.h"
#include "Room.h"
#include "RoomManager.h"

void RoomManager::Init(const int maxRoomCountByLobby, const int maxRoomUserCount)
{		
	for (int i = 0; i < maxRoomCountByLobby; ++i)
	{
		m_RoomList.emplace_back(new Room());
		m_RoomList[i]->Init((short)i, (short)maxRoomUserCount);
	}
}

void RoomManager::Release()
{
	for (int i = 0; i < (int)m_RoomList.size(); ++i)
	{
		delete m_RoomList[i];
	}

	m_RoomList.clear();
}

void RoomManager::SetNetwork(TcpNet* pNetwork, ILog* pLogger)
{
	m_pRefLogger = pLogger;
	m_pRefNetwork = pNetwork;

	for (auto pRoom : m_RoomList)
	{
		pRoom->SetNetwork(pNetwork, pLogger);
	}
}

short RoomManager::GetUserCount()
{ 
	return static_cast<short>(m_UserIndexDic.size()); 
}

Room* RoomManager::GetRoom(const short roomIndex)
{
	if (roomIndex < 0 || roomIndex >= m_RoomList.size()) {
		return nullptr;
	}

	return m_RoomList[roomIndex];
}

Room* RoomManager::GetRandomRoom()
{
	std::vector<Room*>::iterator it = m_RoomList.begin();
	for (; it != m_RoomList.end(); it++)
	{
		if ((*it)->IsFull() == false)
			return (*it);
	}
	return nullptr;
}



