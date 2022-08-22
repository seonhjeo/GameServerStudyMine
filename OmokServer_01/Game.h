#pragma once

#include "ErrorCode.h"

#define MAX_N 19

class Game
{
private:
	bool m_gameStarted;
	int m_nowPlayer;
	int m_board[MAX_N][MAX_N];
	// �� ���� = -1 / 0�� 1�� m_userID �迭�� �ε���

	// ��ǥ�� �������� �Ѿ���� Ȯ���ϴ� �Լ�
	bool IsOverBound(int x, int y);

	// ���� �������� �� �¸��� �Ǿ����� Ȯ���ϴ� �Լ�
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