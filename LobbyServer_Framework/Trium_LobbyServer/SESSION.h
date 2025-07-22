#ifndef SESSION_H
#define SESSION_H

#include "Lobbystdafx.h"
#include "OVER_PLUS.h"
#include "PlayerCharacter.h"


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
};

#endif
