#pragma once

#include "ErrorCode.h"

#define MAX_N 19

class Game
{
private:
	bool m_gameStarted;
	int m_nowPlayer;
	int m_board[MAX_N][MAX_N];
	// 돌 없음 = -1 / 0과 1은 m_userID 배열의 인덱스

	// 좌표가 오목판을 넘어가는지 확인하는 함수
	bool IsOverBound(int x, int y);

	// 돌이 놓아졌을 때 승리가 되었는지 확인하는 함수
	const bool IsWin();

public:
	Game();
	~Game();

	void Init();

	const bool GetGameStarted(){
		return m_gameStarted;
	}

	const int GetNowPlayer() {
		return m_nowPlayer;
	}

	ERROR_CODE PutStone(int x, int y, int user);
	
	ERROR_CODE StartGame();
	ERROR_CODE FinishGame();
};