#pragma once

#include "PacketID.h"
#include "ErrorCode.h"

#pragma pack(push, 1)
// 종류에 따른 패킷의 바디를 만드는 곳
struct PktHeader
{
	short TotalSize;
	short Id;
	unsigned char Reserve;
};

struct PktBase
{
	short ErrorCode = (short)ERROR_CODE::NONE;
	void SetError(ERROR_CODE error) { ErrorCode = (short)error; }
};

//- 로그인 요청
const int MAX_USER_ID_SIZE = 16;
const int MAX_USER_PASSWORD_SIZE = 16;
struct PktLogInReq
{
	char szID[MAX_USER_ID_SIZE] = { 0, };
	char szPW[MAX_USER_PASSWORD_SIZE] = { 0, };
};

struct PktLogInRes : PktBase
{
};


//- 룸에 들어가기 요청
const int MAX_ROOM_TITLE_SIZE = 16;
struct PktRoomEnterReq
{
	//bool IsCreate;
	//short RoomIndex;
	int RoomIndex;
};

struct PktRoomEnterRes : PktBase
{
	long long UniqueId;
};

		
//- 룸에 있는 유저에게 새로 들어온 유저 정보 통보
struct PktRoomEnterUserInfoNtf
{
	long long uniqueId;
	char idlen;
	char UserID[MAX_USER_ID_SIZE];
};


//- 룸 나가기 요청
struct PktRoomLeaveReq {};

struct PktRoomLeaveRes : PktBase
{
};

//- 룸에서 나가는 유저 통보(로비에 있는 유저에게)
struct PktRoomLeaveUserInfoNtf
{
	// TODO:
	//char UserId[MAX_USER_ID_SIZE] = { 0, };
	long long uniqueId;
	char idlen;
	char UserID[MAX_USER_ID_SIZE];
};
		

//- 룸 채팅
const int MAX_ROOM_CHAT_MSG_SIZE = 256;
struct PktRoomChatReq
{
	// TODO:
	short Msglen;
	//wchar_t Msg[MAX_ROOM_CHAT_MSG_SIZE] = { 0, };
	char Msg[MAX_ROOM_CHAT_MSG_SIZE] = { 0, };
};

struct PktRoomChatRes : PktBase
{
};

struct PktRoomChatNtf
{
	//TODO:
	//char UserID[MAX_USER_ID_SIZE] = { 0, };
	long long uniqueId;
	short msgLen;
	wchar_t Msg[MAX_ROOM_CHAT_MSG_SIZE] = { 0, };
};

// 게임 관련 패킷
struct PktGameStartReq
{
};

struct PktGameStartRes : PktBase
{
	long long UniqueId;
};

struct PktGameStartNtf
{
};

struct PktGameFinishReq
{
};

struct PktGameFinishRes : PktBase
{
	long long UniqueId;
};

struct PktGameFinishNtf
{
};

struct PktGameStoneReq
{
	int x;
	int y;
};

struct PktGameStoneRes : PktBase
{
};

struct PktGameStoneNtf
{
	int x;
	int y;
	bool color;
};
#pragma pack(pop)

