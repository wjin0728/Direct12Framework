#include "SESSION.h"




SESSION::~SESSION() {}

void SESSION::do_recv()
{
	DWORD recv_flag = 0;
	memset(&_recv_over._over, 0, sizeof(_recv_over._over));
	_recv_over._wsabuf.len = (CHAT_SIZE * 2) - _prev_remain;
	_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;

	WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._over, 0);
}

void SESSION::do_send(void* packet)
{
	OVER_PLUS* sdata = new OVER_PLUS{ reinterpret_cast<char*>(packet) };
	WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, 0);
}

void SESSION::send_login_info_packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = _id;
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	do_send(&p);
}

void SESSION::send_add_player_packet(SESSION* client)
{
	SC_ADD_OBJECT_PACKET add_packet;
	add_packet.id = client->_id;
	add_packet.size = sizeof(add_packet);
	add_packet.type = SC_ADD_OBJECT;
	do_send(&add_packet);
}

void SESSION::send_chat_packet(int c_id, const char* mess) 
{
	SC_CHAT_PACKET chat_packet;
	chat_packet.id = c_id;
	strcpy_s(chat_packet.mess, mess);
	chat_packet.type = SC_CHAT;
	chat_packet.size = sizeof(chat_packet);
	do_send(&chat_packet);
}

void SESSION::send_move_packet(SESSION* client)
{
	SC_MOVE_PACKET p;

	p.id = client->_id;
	p.type = SC_MOVE_OBJECT;
	p.size = sizeof(p);
	p.x = client->_pos.x;
	p.y = client->_pos.y;
	p.z = client->_pos.z;
	//p.look_x = client->_look_dir.x;
	p.look_y = client->_look_dir.y;
	//p.look_z = client->_look_dir.z;
	cout << client->_look_dir.y << std::endl;

	// std::cout << "Move client << " << p.id << " : { " << p.x << ", " << p.y << ", " << p.z << " }" << std::endl;
	do_send(&p);
}