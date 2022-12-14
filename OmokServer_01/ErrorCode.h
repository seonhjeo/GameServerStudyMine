#pragma once


// 201 부터 시작한다.
enum class ERROR_CODE : short
{
	NONE = 0,

	UNASSIGNED_ERROR = 201,

	MAIN_INIT_NETWORK_INIT_FAIL = 206,

	USER_MGR_ID_DUPLICATION = 211,
	USER_MGR_MAX_USER_COUNT = 212,
	USER_MGR_INVALID_SESSION_INDEX = 213,
	USER_MGR_NOT_CONFIRM_USER = 214,
	USER_MGR_REMOVE_INVALID_SESSION = 221,

	USER_MGR_ALREADY_IN_ROOM = 231,
	USER_MGR_NOT_IN_ROOM = 232,

	ROOM_MGR_NO_SUCH_ROOM = 241,
	ROOM_MGR_NUMBER_OVER_MAX = 242,
	ROOM_MGR_ROOM_ALREADY_FULL = 243,
	ROOM_MGR_ENTER_ROOM_FAIL = 244,

	GAME_MGR_GAME_NOT_STARTED = 251,
	GAME_MGR_NOT_PLAYERS_TURN = 252,
	GAME_MGR_BOARD_BOUND_OVER = 253,
	GAME_MGR_ALREADY_STONE_EXIST = 254,
	GAME_MGR_PLAYER_WIN = 255
};
