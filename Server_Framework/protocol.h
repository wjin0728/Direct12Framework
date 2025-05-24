#pragma once

constexpr int PORT_NUM = 4000;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 300;

constexpr int MAX_USER = 3;
constexpr int MAX_ITEM = 5;



constexpr int MAX_HP_FIGHTER = 1500;
constexpr int MAX_HP_ARCHER_MAGE = 1200;

constexpr int WATER_HEAL_AMT = 300;
constexpr int WATER_SHIELD_AMT = 2;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_MOUSE_VEC3 = 3;
constexpr char CS_SKILL_TARGET = 4;
constexpr char CS_SKILL_NONTARGET = 5;
constexpr char CS_ULTIMATE_SKILL = 6;
constexpr char CS_000 = 7;
constexpr char CS_CHANGE_SCENE = 8;
constexpr char CS_CHANGE_STATE = 9;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_LOGIN_FAIL = 3;
constexpr char SC_ADD_OBJECT = 4;
constexpr char SC_REMOVE_OBJECT = 5;
constexpr char SC_ALL_PLAYERS_POS = 6;
constexpr char SC_CHAT = 7;
constexpr char SC_SCENE_CHANGE = 8;
constexpr char SC_ADD_PLAYER = 9;
constexpr char SC_REMOVE_PLAYER = 10;
constexpr char SC_DROP_ITEM = 11;
constexpr char SC_REMOVE_ITEM = 12;
constexpr char SC_USE_SKILL = 13;
constexpr char SC_MONSTER_POS = 14;
constexpr char SC_ITEM_POS = 15;
constexpr char SC_CHANGE_SCENE = 16;


#pragma pack (push, 1)
struct PACKET {
	unsigned short	size;
	char			type;
};


// ----------------------------------------------------------------------------------


struct CS_LOGIN_PACKET : PACKET {
	char			name[NAME_SIZE];
};

struct CS_CHAT_PACKET : PACKET {
	char			mess[CHAT_SIZE];
};

struct CS_MOVE_PACKET : PACKET {
	int				id;
	uint8_t			dir;
	float			look_x;
	float			look_y;
	float			look_z;
};

struct CS_MOUSE_VEC3_PACKET : PACKET {
	int				id;
	float			dir_x;
	float			dir_y;
	float			dir_z;
};

struct CS_000_PACKET : PACKET {
	int				id;
};

struct CS_SKILL_TARGET_PACKET : PACKET {
	int				id;
	uint8_t			skill_enum;
	int				target_id;
};

struct CS_SKILL_NONTARGET_PACKET : PACKET {
	int				id;
	uint8_t			skill_enum;
};

struct CS_ULTIMATE_SKILL_PACKET : PACKET {
	int				id;
	uint8_t			skill_enum;
	int				target_id;
};


struct CS_CHANGE_SCENE_PACKET : PACKET {
	uint8_t			change_scene;
};

struct CS_CHANGE_STATE_PACKET : PACKET {
	int				id;
	uint8_t			state;
};

// ----------------------------------------------------------------------------------


struct SC_LOGIN_INFO_PACKET : PACKET {
	int				id;
};

struct SC_ADD_PLAYER_PACKET : PACKET {
	int				id;
	uint8_t			player_class;
	float			x;
	float			y;
	float			z;
	float			look_y;
};

struct SC_REMOVE_PLAYER_PACKET : PACKET {
	int				id;
};

struct SC_ADD_OBJECT_PACKET : PACKET {
	int				id;
};

struct SC_ALL_PLAYERS_POS_PACKET : PACKET {
	int clientId[3]{ -1, -1, -1 };
	float x[3], y[3], z[3];
	float look_y[3];
	uint8_t state[3]{ 0, 0, 0 };
};

struct SC_MONSTER_POS_PACKET : PACKET {
	int monsterId;
	float x, y, z;
	float look_y;
};

struct SC_ITEM_POS_PACKET : PACKET {
	int itemId;
	float x, y, z;
	float look_y;
};

struct SC_CHAT_PACKET : PACKET {
	int				id;
	char			mess[CHAT_SIZE];
};

struct SC_DROP_ITEM_PACKET : PACKET {
	int				item_id;
	uint8_t			item_enum;
	float			x;
	float			y;
	float			z;
};

struct SC_REMOVE_ITEM_PACKET : PACKET {
	int				item_id;
	char			player_id;
					// -1이면 단순 삭제, 
					// or player_id 플레이어가 아이템 획득
	uint8_t			item_type;
};

struct SC_USE_SKILL_PACKET : PACKET {
	int				player_id;
	uint8_t			skill_type;
};

struct SC_LOGIN_FAIL_PACKET : PACKET {
};

struct SC_CHANGE_SCENE_PACKET : PACKET {
	uint8_t			change_scene;
};

struct SC_ADD_MONSTER_PACKET : PACKET {
	int				monster_id;
	uint8_t			monster_type;
};
#pragma pack (pop)