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
    m_gameStarted = false;
    m_nowPlayer = 0;
    for (int i = 0; i < MAX_N; i++)
        for (int j = 0; j < MAX_N; j++)
            m_board[i][j] = -1;
}

ERROR_CODE Game::PutStone(int x, int y, int user)
{
    // TODO
    // ������ ���ۻ��°� �ƴϸ� ERR_GAME_NOT_STARTED
    if (m_gameStarted == false)
        return ERROR_CODE::GAME_MGR_GAME_NOT_STARTED;
    // user�� m_nowPlayer�̶� ���� ������ ERR_GAME_NOT_PLAYERS_TURN
    if (user != m_nowPlayer)
        return ERROR_CODE::GAME_MGR_NOT_PLAYERS_TURN;
    // ���� �ڸ��� �ٿ�带 �Ѱ����� ERR_GAME_BOUND_OVER
    if (IsOverBound(x, y))
        return ERROR_CODE::GAME_MGR_BOARD_BOUND_OVER;
    // ���� �ڸ��� �̹� ������ ������ ERR_ALREADY_STONE_EXIST
    if (m_board[y][x] != -1)
        return ERROR_CODE::GAME_MGR_ALREADY_STONE_EXIST;

    // �� ����
    m_board[y][x] = user;

    if (IsWin())
        return ERROR_CODE::GAME_MGR_PLAYER_WIN;
 
    return ERROR_CODE::NONE;
}

bool Game::IsOverBound(int x, int y)
{
    if (x < 0 || y < 0 || x >= MAX_N || y >= MAX_N)
        return true;
    return false;
}

const bool Game::IsWin()
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

ERROR_CODE Game::StartGame()
{
    m_gameStarted = true;
    return ERROR_CODE::NONE;
}
ERROR_CODE Game::FinishGame()
{
    m_gameStarted = false;
    Init();
    return ERROR_CODE::NONE;
}