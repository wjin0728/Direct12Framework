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
	SC_ADD_PLAYER_PACKET p;
	p.id = client->_id;
	p.size = sizeof(p);
	p.type = SC_ADD_PLAYER;
	p.player_class = (uint8_t)client->_player._class;
	do_send(&p);
}