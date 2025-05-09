#pragma once
#include "stdafx.h"
#include "OVER_PLUS.h"
//#include "../../Direct12Framework/SimpleMath.h"
//
//using namespace DirectX;
//using Vec3 = SimpleMath::Vector3;


class SESSION
{
	OVER_PLUS _recv_over;

public:
	mutex				_s_lock;
	SESSION_STATE		_state;
	int					_id;
	SOCKET				_socket;
	int					_prev_remain;

	Vec3				_pos;	
	Vec3				_look_dir; // 캐릭터가 보고있는 방향
	Vec3				_velocity;     // 추가: 속도
	Vec3				_acceleration; // 추가: 가속도
	Quaternion			_look_rotation;
	uint8_t			    _class;

	SESSION() : 
		_state(SESSION_STATE::ST_FREE), 
		_id(0), 
		_prev_remain(0),
		_pos(10.f, 0.f, 10.f), 
		_look_rotation(Quaternion::Identity),
		_velocity(Vec3::Zero), 
		_look_dir(Vec3::Zero),
		_acceleration(Vec3::Zero) {};

	~SESSION();

	void do_recv();

	void do_send(void* packet);
	void send_login_info_packet();
	void send_add_player_packet(SESSION* client);
	void send_chat_packet(int c_id, const char* mess);
	void send_move_packet(SESSION* client);
};