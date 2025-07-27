#pragma once
#include "stdafx.h"
#include <mutex>

class ServerManager
{
	MAKE_SINGLETON(ServerManager)
public:
	SOCKET								send_socket, server_socket;
	WSAOVERLAPPED						wsaover;

	//char								SERVER_ADDR[NAME_SIZE]{ "127.0.0.1" };
	char								SERVER_ADDR[NAME_SIZE]{ "10.30.2.4" };

	std::shared_ptr<class CGameObject> mPlayer{ nullptr };
	std::shared_ptr<class CGameObject> mMainCamera{ nullptr };

	std::map<int, std::shared_ptr<class CGameObject>> mOtherPlayers{};
	std::unordered_map<int, std::shared_ptr<class CGameObject>> mEnemies{};
	std::unordered_map<int, std::shared_ptr<class CGameObject>> mItems{};
	std::unordered_map<int, std::shared_ptr<class CGameObject>> mProjectiles{};

	bool mIsConnected{ false };
	bool mIsLoggedIn{ false };
	int clientID{ -1 };
	bool RenderOK{ 0 };

	size_t save_data_size = 0;
	size_t one_packet_size = 0;
	char save_buf[CHAT_SIZE * 3];

	mutex object_lock;

	std::unordered_map<std::string, std::function<void(std::vector<std::any>)>> mEventMap;

public:
	void Initialize();
	void Destroy();
	void Connect(int port_num);
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

	void send_cs_000_packet(int key) {
		CS_000_PACKET p;
		p.size = sizeof(p);
		p.type = CS_000;
		p.key = key;
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
		Send_Packet(&p);
	}
	void send_cS_skill_nontarget_packet(uint8_t skill) {
		CS_SKILL_TARGET_PACKET p;
		p.size = sizeof(p);
		p.type = CS_SKILL_NONTARGET;
		p.id = clientID;
		p.skill_enum = skill;
		Send_Packet(&p);
	}
	void send_cs_mouse_ldown_packet(Vec3 dir) {
		CS_MOUSE_LDOWN_PACKET p;
		p.size = sizeof(p);
		p.type = CS_MOUSE_LDOWN;
		p.id = clientID;
		p.dir_x = dir.x;
		p.dir_y = dir.y;
		p.dir_z = dir.z;
		Send_Packet(&p);
	}
	void send_cs_attack_packet() {
		CS_ATTACK_PACKET p;
		p.size = sizeof(p);
		p.type = CS_ATTACK;
		p.id = clientID;
		Send_Packet(&p);
	}
	void send_cs_change_state_packet(uint8_t state) {
		CS_CHANGE_STATE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_CHANGE_STATE;
		p.id = clientID;
		p.state = state;
		Send_Packet(&p);
	}
	void send_cs_game_server_login_packet() {
		CS_GAME_SERVER_LOGIN_PACKET p;
		p.size = sizeof(p);
		p.type = CS_GAME_SERVER_LOGIN;
		p.id = clientID;
		Send_Packet(&p);
	}
	void send_cs_click_button_packet(uint8_t button) {
		CS_CLICK_BUTTON_PACKET p;
		p.size = sizeof(p);
		p.type = CS_CLICK_BUTTON;
		p.button_type = button;
		Send_Packet(&p);
	}
	void send_cs_ready_for_next_stage_packet(bool ready) {
		CS_READY_FOR_NEXT_STAGE_PACKET p;
		p.size = sizeof(p);
		p.type = CS_READY_FOR_NEXT_STAGE;
		p.ready = ready;
		Send_Packet(&p);
	}

	// 몬스터 죽이기 위한 편의용 패킷
	void send_hp_packet(int id, int hp) {
		CS_HP_PACKET p;
		p.type = CS_HP;
		p.size = sizeof(p);
		p.object_id = id;
		p.hp = hp;
		Send_Packet(&p);
	}

	void AddEvent(const std::string& name, std::function<void(std::vector<std::any>)> func) {
		mEventMap[name] = func;
	}
	void TriggerEvent(const std::string& name, const std::vector<std::any>& args) {
		if (mEventMap.count(name)) mEventMap[name](args);
	}
};