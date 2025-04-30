#pragma once

constexpr int PORT_NUM = 4000;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 300;

constexpr int MAX_USER = 16;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_MOUSE_VEC2 = 3;

constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_LOGIN_FAIL = 3;
constexpr char SC_ADD_OBJECT = 4;
constexpr char SC_REMOVE_OBJECT = 5;
constexpr char SC_MOVE_OBJECT = 6;
constexpr char SC_CHAT = 7;
constexpr char SC_SCENE_CHANGE = 8;

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

struct CS_MOUSE_VEC2_PACKET : PACKET {
	int				id;
	float			dir_x;
	float			dir_z;
};


// ----------------------------------------------------------------------------------


struct SC_LOGIN_INFO_PACKET : PACKET {
	int				id;
};

struct SC_ADD_OBJECT_PACKET : PACKET {
	int				id;
};

struct SC_MOVE_PACKET : PACKET {
	int				id;
	float			x;
	float			y;
	float			z;
	float			look_x;
	float			look_y;
	float			look_z;
};

struct SC_CHAT_PACKET : PACKET {
	int				id;
	char			mess[CHAT_SIZE];
};

struct SC_LOGIN_FAIL_PACKET : PACKET {
};

#pragma pack (pop)