#pragma once

#include "stdafx.h"

class OVER_PLUS
{
public:
	WSAOVERLAPPED _over;
	WSABUF _wsabuf;
	char _send_buf[CHAT_SIZE * 2];
	COMP_TYPE _comp_type;

	OVER_PLUS() {
		_wsabuf.len = CHAT_SIZE * 2;
		_wsabuf.buf = _send_buf;
		_comp_type = OP_RECV;
		ZeroMemory(&_over, sizeof(_over));
	}

	OVER_PLUS(char* packet) {
		_wsabuf.len = *(unsigned short*)packet;
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_over, sizeof(_over));
		_comp_type = OP_SEND;
		memcpy(_send_buf, packet, _wsabuf.len);
	}
};