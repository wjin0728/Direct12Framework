#pragma once
#include "stdafx.h"
#include <mutex>

class ServerManager
{
	MAKE_SINGLETON(ServerManager)
public:
	SOCKET								send_socket, server_socket;
	WSAOVERLAPPED						wsaover;

	char								SERVER_ADDR[NAME_SIZE]{ "127.0.0.1" };

	std::shared_ptr<class CGameObject> mPlayer{ nullptr };
	std::shared_ptr<class CGameObject> mMainCamera{ nullptr };

	std::unordered_map<int, std::shared_ptr<class CGameObject>> mOtherPlayers{};
	std::unordered_map<int, std::shared_ptr<class CGameObject>> mEnemies{};
	std::unordered_map<int, std::shared_ptr<class CGameObject>> mItems{};

	int clientID{ -1 };
	bool RenderOK{ 0 };

	size_t save_data_size = 0;
	size_t one_packet_size = 0;
	char save_buf[CHAT_SIZE * 2];

	mutex object_lock;

public:
	void Initialize();
	void Client_Login();
	bool InitPlayerAndCamera();
	void RegisterPlayerInScene(class CScene* scene);
	void AddNewPlayer(int id, Vec3 pos);

	void Recv_Packet();
	static void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);
	void Recv_Loop();
	void Send_Packet(void* packet);
	static void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);
	void Using_Packet(char* packet_ptr);
	void print_error(const char* msg, int err_no);

	void send_cs_000_packet() {
		CS_000_PACKET p;
		p.size = sizeof(p);
		p.type = CS_000;
		p.id = clientID;
		Send_Packet(&p);
	}
	void send_cs_move_packet(uint8_t dir, Vec3 look) {
		CS_MOVE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.id = clientID;
		p.dir = dir;
		p.look_x = look.x;
		p.look_y = look.y;
		p.look_z = look.z;

		Send_Packet(&p);
	}
	void send_cS_skill_target_packet(uint8_t skill, int target_id) {
		CS_SKILL_TARGET_PACKET p;
		p.size = sizeof(p);
		p.type = CS_SKILL_TARGET;
		p.id = clientID;
		p.skill_enum = skill;
		p.target_id = target_id;
	}
	void send_cS_skill_nontarget_packet(uint8_t skill) {
		CS_SKILL_TARGET_PACKET p;
		p.size = sizeof(p);
		p.type = CS_SKILL_TARGET;
		p.id = clientID;
		p.skill_enum = skill;
	}
	void send_cs_mouse_vec3_packet(Vec3 dir) {
		CS_MOUSE_VEC3_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOUSE_VEC3;
		p.id = clientID;
		p.dir_x = dir.x;
		p.dir_y = dir.y;
		p.dir_z = dir.z;
		Send_Packet(&p);
	}
};

