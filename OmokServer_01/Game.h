#pragma once

#include "ErrorCode.h"

#define MAX_N 19

class Game
{
private:
	int m_nowPlayer;
	int m_board[MAX_N][MAX_N];
	// �� ���� = -1 / 0�� 1�� m_userID �迭�� �ε���

	Game();
	~Game();

	void Init();

	// ��ǥ�� �������� �Ѿ���� Ȯ���ϴ� �Լ�
	bool IsOverBound(int x, int y);

	// ���� �������� �� �¸��� �Ǿ����� Ȯ���ϴ� �Լ�
	bool IsWin();

public:
	ERROR_CODE PutStone(int x, int y, int user);
};