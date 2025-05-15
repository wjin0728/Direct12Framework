#ifndef SESSION_H
#define SESSION_H

#include "stdafx.h"
#include "OVER_PLUS.h"
#include "Object.h"
#include "PlayerCharacter.h"
#include "Item.h"

//#include "../../Direct12Framework/SimpleMath.h"
//
//using namespace DirectX;
//using Vec3 = SimpleMath::Vector3;


class SESSION
{
	OVER_PLUS _recv_over;

public:
	mutex				_s_lock;
	SESSION_STATE		_state = ST_FREE;
	int					_id;
	SOCKET				_socket;
	int					_prev_remain;

	PlayerCharacter		_player;

	SESSION() :
		_id(0), 
		_prev_remain(0) {};

	~SESSION();

	void do_recv();
	void do_send(void* packet);

	void send_login_info_packet();
	void send_add_player_packet(SESSION* client);
	void send_chat_packet(int c_id, const char* mess);
	void send_drop_item_packet(Item& it, int item_id);
	void send_remove_item_packet(int item_id, int player_id, uint8_t item_type);
	void send_use_skill_packet(S_ITEM_TYPE skill_type, int player_id);
	void send_change_scene_packet(uint8_t scene);
};

#endif
