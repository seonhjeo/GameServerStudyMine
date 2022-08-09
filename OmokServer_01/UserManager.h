#pragma once
#include <unordered_map>
#include <deque>
#include <string>
#include <vector>

#include "ErrorCode.h"
	
class User;

class UserManager
{
public:
	UserManager();
	virtual ~UserManager();

	void Init(const int maxUserCount);

	ERROR_CODE AddUser(const int sessionIndex, const char* pszID);
	ERROR_CODE RemoveUser(const int sessionIndex);

	std::tuple<ERROR_CODE,User*> GetUser(const int sessionIndex);

				
private:
	User* AllocUserObjPoolIndex();
	void ReleaseUserObjPoolIndex(const int index);

	User* FindUser(const int sessionIndex);
	User* FindUser(const char* pszID);
				
private:
	// 유저의 총량이 정해져 있기 때문에 Pool을 이용해 생성/삭제 간소화
	std::vector<User> m_UserObjPool;
	std::deque<int> m_UserObjPoolIndex;

	std::unordered_map<int, User*> m_UserSessionDic;
	std::unordered_map<const char*, User*> m_UserIDDic; 

};
