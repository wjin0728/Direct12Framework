#include "SESSION.h"
#include "Item.h"



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
	p.player_class = client->_player._class;
	p.x = client->_player._pos.x;
	p.y = client->_player._pos.y;
	p.z = client->_player._pos.z;
	p.look_y = client->_player._look_dir.y;
	do_send(&p);
}

void SESSION::send_chat_packet(int c_id, const char* mess) 
{
	SC_CHAT_PACKET p;
	p.id = c_id;
	strcpy_s(p.mess, mess);
	p.type = SC_CHAT;
	p.size = sizeof(p);
	do_send(&p);
}

void SESSION::send_move_packet(SESSION* client)
{
	SC_MOVE_PACKET p;

	p.id = client->_id;
	p.type = SC_MOVE_OBJECT;
	p.size = sizeof(p);

	p.x = client->_player._pos.x;
	p.y = client->_player._pos.y;
	p.z = client->_player._pos.z;

	p.look_y = client->_player._look_dir.y;

	do_send(&p);
}

void SESSION::send_drop_item_packet(Item& it, int item_id)
{
	SC_DROP_ITEM_PACKET p;
	p.type = SC_DROP_ITEM;
	p.size = sizeof(p);

	p.item_id = item_id;

	p.item_enum = it._item_type;

	p.x = it._pos.x;
	p.y = it._pos.y;
	p.z = it._pos.z;

	do_send(&p);
}

void SESSION::send_remove_item_packet(int item_id, int player_id)
{
	SC_REMOVE_ITEM_PACKET p;
	p.type = SC_REMOVE_ITEM;
	p.size = sizeof(p);
	p.item_id = item_id;
	p.player_id = player_id;
	do_send(&p);
	cout << "sssssssssssssssssssssssssssss";
}