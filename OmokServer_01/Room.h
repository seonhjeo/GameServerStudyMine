#pragma once

#include <vector>
#include <string>
#include <memory>

#include "User.h"


namespace NServerNetLib { class ITcpNetwork; }
namespace NServerNetLib { class ILog; }


	
using TcpNet = NServerNetLib::ITcpNetwork;
using ILog = NServerNetLib::ILog;

class Game;

class Room
{
public:
	Room();
	virtual ~Room();
	// 초기화
	void Init(const short index, const short maxUserCount);
	// 네트워크 연결
	void SetNetwork(TcpNet* pNetwork, ILog* pLogger);
	// 모두 방에서 나갔을 때 방을 비우기
	void Clear();
	// 방에서 유저 한 명을 비우는 함수
	bool UserLeaveRoom(const short userIndex);
	// 유저를 이 방에 추가하는 함수	 
	bool UserEnterRoom(User *user);

	void RoomAnnounce();
		
	short GetIndex() { return m_Index;  }

	bool IsUsed() { return m_IsUsed; }
		
	short MaxUserCount() { return m_MaxUserCount; }

	short GetUserCount() { return (short)m_UserList.size(); }

	bool IsFull();

	void BroadCastOtherJoin(User* other);
	void BroadCastOtherLeave(User* other);
	void BroadCastOtherChat(User* other, std::string msg);
	void GetListOtherUser(User* other);

		

private:
	ILog* m_pRefLogger;
	TcpNet* m_pRefNetwork;

	short m_Index = -1;
	short m_MaxUserCount;
		
	bool m_IsUsed = false;
	std::vector<User*> m_UserList;

	// 룸에서 실행할 게임 오브젝트
	// Game* m_pGame = nullptr;
};
