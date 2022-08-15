#include "Game.h"

Game::Game()
{
    Init();
}

Game::~Game()
{
}

void Game::Init()
{
    m_nowPlayer = 0;
    for (int i = 0; i < MAX_N; i++)
        for (int j = 0; j < MAX_N; j++)
            m_board[i][j] = -1;
}

ERROR_CODE Game::PutStone(int x, int y, int user)
{
    // TODO
    // user�� m_nowPlayer�̶� ���� ������ ERR_GAME_NOT_PLAYERS_TURN
    // ���� �ڸ��� �ٿ�带 �Ѱ����� ERR_GAME_BOUND_OVER
    // ���� �ڸ��� �̹� ������ ������ ERR_ALREADY_STONE_EXIST
    // �� ����

    if (IsWin())
    {
        // TODO : ���ڰ� �������� ��Ŷ ������
    }
}

bool Game::IsOverBound(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAX_N || y >= MAX_N)
        return true;
    return false;
}

bool Game::IsWin()
{
    // | ����
    for (int i = 0; i < MAX_N - 4; i++)
    {
        for (int j = 0; j < MAX_N; j++)
        {
            if (m_board[i][j] == m_nowPlayer && m_board[i + 1][j] == m_nowPlayer && m_board[i + 2][j] == m_nowPlayer
                && m_board[i + 3][j] == m_nowPlayer && m_board[i + 4][j] == m_nowPlayer)
            {
                return true;
            }
        }
    }
    // �� ����
    for (int j = 0; j < MAX_N - 4; j++)
    {
        for (int i = 0; i < MAX_N; i++)
        {
            if (m_board[i][j] == m_nowPlayer && m_board[i][j + 1] == m_nowPlayer && m_board[i][j + 2] == m_nowPlayer
                && m_board[i][j + 3] == m_nowPlayer && m_board[i][j + 4] == m_nowPlayer)
            {
                return true;
            }
        }
    }
    // \ ����
    for (int i = 0; i < MAX_N - 4; i++)
    {
        for (int j = 0; j < MAX_N - 4; j++)
        {
            if (m_board[i][j] == m_nowPlayer && m_board[i + 1][j + 1] == m_nowPlayer && m_board[i + 2][j + 2] == m_nowPlayer
                && m_board[i + 3][j + 3] == m_nowPlayer && m_board[i + 4][j + 4] == m_nowPlayer)
            {
                return true;
            }
        }
    }
    // / ����
    for (int i = 0; i < MAX_N - 4; i++)
    {
        for (int j = 0; j < MAX_N - 4; j++)
        {
            if (m_board[MAX_N - i - 1][j] == m_nowPlayer && m_board[MAX_N - i - 2][j + 1] == m_nowPlayer && m_board[MAX_N - i - 3][j + 2] == m_nowPlayer
                && m_board[MAX_N - i - 4][j + 3] == m_nowPlayer && m_board[MAX_N - i - 5][j + 4] == m_nowPlayer)
            {
                return true;
            }
        }
    }
    return false;
}