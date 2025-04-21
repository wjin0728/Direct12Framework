#include "GameManager.h"


//#include <DirectXMath.h>
//#include "../../Direct12Framework/SimpleMath.h"
//#include "../../Direct12Framework/Timer.h"
//using namespace DirectX;
//using Vec3 = SimpleMath::Vector3;

GameManager::GameManager()
{
	lastTime = std::chrono::high_resolution_clock::now();
	terrain.SetResolution(513); 
	terrain.SetNavMapResolution(terrain.GetResolution() * 2); 
	terrain.SetScale(45, 20, 45); 
	terrain.LoadHeightMap("LobbyHeightmap"); 
	terrain.LoadNavMap("LobbyTerrainNavMap");
	cout << "Map loaded.\n";

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	server_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	S_Bind_Listen();

	S_Accept();

	Make_threads();
}
GameManager::~GameManager()
{
	closesocket(server_socket);
	WSACleanup();
}

void GameManager::S_Bind_Listen()
{
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server_socket, SOMAXCONN);
}
void GameManager::S_Accept()
{
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	h_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(server_socket), h_iocp, 9999, 0);
	client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	accept_over._comp_type = OP_ACCEPT;
	AcceptEx(server_socket, client_socket, accept_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &accept_over._over);
}

void GameManager::Make_threads()
{
	vector <thread> worker_threads;
	int num_threads = std::thread::hardware_concurrency();
	for (int i = 0; i < num_threads; ++i)
		worker_threads.emplace_back( &GameManager::Worker_thread, this );
		
	for (auto& th : worker_threads)
		th.join();	
}

void GameManager::Worker_thread()
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	while (true) {
		DWORD num_bytes;
		ULONG_PTR key;
		WSAOVERLAPPED* over = nullptr;
		BOOL ret = GetQueuedCompletionStatus(h_iocp, &num_bytes, &key, &over, INFINITE);
		OVER_PLUS* ex_over = reinterpret_cast<OVER_PLUS*>(over);
		if (FALSE == ret) {
			if (ex_over->_comp_type == OP_ACCEPT) cout << "Accept Error";
			else {
				cout << "GQCS Error on client[" << key << "]\n";
				Disconnect(static_cast<int>(key));
				if (ex_over->_comp_type == OP_SEND) delete ex_over;
				continue;
			}
		}

		if ((0 == num_bytes) && ((ex_over->_comp_type == OP_RECV) || (ex_over->_comp_type == OP_SEND))) {
			Disconnect(static_cast<int>(key));
			if (ex_over->_comp_type == OP_SEND) delete ex_over;
			continue;
		}

		switch (ex_over->_comp_type) {
		case OP_ACCEPT: {
			int client_id = Get_new_Client_id();
			if (client_id != -1) {
				{
					lock_guard<mutex> ll(clients[client_id]._s_lock);
					clients[client_id]._state = ST_ALLOC;
				}
				clients[client_id]._id = client_id;
				clients[client_id]._prev_remain = 0;
				clients[client_id]._socket = client_socket;
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), h_iocp, client_id, 0);
				clients[client_id].do_recv();
				client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			}
			else {
				cout << "Max user exceeded.\n";
			}
			ZeroMemory(&accept_over._over, sizeof(accept_over._over));
			int addr_size = sizeof(SOCKADDR_IN);
			AcceptEx(server_socket, client_socket, accept_over._send_buf, 0, addr_size + 16, addr_size + 16, 0, &accept_over._over);			
			break;
		}
		case OP_RECV: {
			int remain_data = num_bytes + clients[key]._prev_remain;
			char* p = ex_over->_send_buf;
			while (remain_data > 0) {
				WORD* byte = reinterpret_cast<WORD*>(p);
				int packet_size = *byte;
				if (packet_size <= remain_data) {
					Process_packet(static_cast<int>(key), p);
					p = p + packet_size;
					remain_data = remain_data - packet_size;
				}
				else break;
			}
			clients[key]._prev_remain = remain_data;
			if (remain_data > 0) {
				memcpy(ex_over->_send_buf, p, remain_data);
			}
			clients[key].do_recv();
			break;
		}
		case OP_SEND: {
			delete ex_over;
			break;
		}
			break;
		}
	}

	CoUninitialize();
}

void GameManager::Disconnect(int c_id)
{
	closesocket(clients[c_id]._socket);

	lock_guard<mutex> ll(clients[c_id]._s_lock);
	clients[c_id]._state = ST_FREE;
}

int GameManager::Get_new_Client_id()
{
	for (int i = 0; i < MAX_USER; ++i) {
		lock_guard <mutex> ll{ clients[i]._s_lock };
		if (clients[i]._state == ST_FREE)
			return i;
	}
	return -1;
}

void GameManager::Process_packet(int c_id, char* packet)
{
	switch (packet[2]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		{
			lock_guard<mutex> ll{ clients[c_id]._s_lock };
			clients[c_id]._state = ST_INGAME;
		}

		clients[c_id].send_login_info_packet();

		cout << "login : " << c_id << endl;
		break;
	}
	case CS_CHAT: {
		CS_CHAT_PACKET* p = reinterpret_cast<CS_CHAT_PACKET*>(packet);

		for (auto& cl : clients) {
			if (cl._state != ST_INGAME) continue;
			cl.send_chat_packet(c_id, p->mess);
		}

		std::cout << p->mess << std::endl;
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(packet);

		// 이동 방향 계산
		Vec3 moveDir = Vec3::Zero;
		Vec3 local_lookDir = Vec3(p->look_x, p->look_y, p->look_z);
		Vec3 right_dir = local_lookDir.Cross(Vec3::Up);
		right_dir.y = 0.f;
		right_dir.Normalize();
		local_lookDir.y = 0.f;
		local_lookDir.Normalize();

		if (p->dir & KEY_FLAG::KEY_W) moveDir += local_lookDir;
		if (p->dir & KEY_FLAG::KEY_S) moveDir -= local_lookDir;
		if (p->dir & KEY_FLAG::KEY_D) moveDir += right_dir;
		if (p->dir & KEY_FLAG::KEY_A) moveDir -= right_dir;
		if (p->dir & KEY_FLAG::KEY_SHIFT) moveDir += Vec3(0, -1, 0); // 하강
		if (p->dir & KEY_FLAG::KEY_CTRL) moveDir += Vec3(0, 1, 0);  // 상승

		// 가속도 및 속도 계산
		Vec3 accel = Vec3::Zero;
		bool isDecelerate = true;

		if (moveDir.LengthSquared() > 0.0001f) {
			moveDir.Normalize();

			Vec3 currentVelocity = clients[c_id]._velocity;
			float currentSpeed = currentVelocity.Length();
			if (currentVelocity.LengthSquared() > 0.0001f) {
				Vec3 currentDir = currentVelocity.GetNormalized();
				float dot = currentDir.Dot(moveDir);

				if (dot < 0.0f) {
					accel = -currentDir * 20.f; // 단항 마이너스 연산자
				}
				else {
					float scale = currentSpeed * dot;
					clients[c_id]._velocity = moveDir * scale; // 스칼라-벡터 곱셈
					accel = moveDir * 10.f;
					isDecelerate = false;
				}
			}
			else {
				accel = moveDir * 10.f;
				isDecelerate = false;
			}
		}

		// 속도 업데이트
		Vec3 currentVelocity = clients[c_id]._velocity;
		currentVelocity += accel * (1.f / 110.f);
		if (isDecelerate && currentVelocity.LengthSquared() > 0.0001f) {
			Vec3 friction = -currentVelocity.GetNormalized() * 20.f;
			currentVelocity += friction * (1.f / 110.f);
			if (currentVelocity.LengthSquared() < 0.01f) {
				currentVelocity = Vec3::Zero;
			}
		}
		clients[c_id]._velocity = currentVelocity;
		clients[c_id]._acceleration = accel;

		// 위치 업데이트 (지형 검사 가정)
		Vec3 newPos = clients[c_id]._pos + currentVelocity * (1.f / 110.f);			
		
		if (CanMove(newPos.x, newPos.z)) {
			moveDir.Normalize();

			Vec3 currentVelocity = clients[c_id]._velocity;
			float currentSpeed = currentVelocity.Length();
			if (currentVelocity != Vec3::Zero) {
				Vec3 currentDir = currentVelocity.GetNormalized();
				float dot = currentDir.Dot(moveDir);

				if (dot < 0.0f) {
					accel = -currentDir * 20.f;
				}
				else {
					clients[c_id]._velocity = (currentSpeed * dot * moveDir);
					accel = moveDir * 10.f;
					isDecelerate = false;
				}
			}
			else {
				accel = moveDir * 10.f; // Á¤Áö »óÅÂ¿¡¼­ °¡¼Ó
				isDecelerate = false;
			}

			float rotationSpeed = 10.f; 
			float deltaTime = GetDeltaTime();

			if (moveDir.LengthSquared() > 0.001f)
			{
				Quaternion targetRot = Quaternion::LookRotation(moveDir);
				Quaternion rotation = Quaternion::Slerp(clients[c_id]._look_rotation, targetRot, rotationSpeed * deltaTime);
				clients[c_id]._look_rotation = rotation;
			}

			float terrainHeight = terrain.GetHeight(clients[c_id]._pos.x, clients[c_id]._pos.z);
			clients[c_id]._pos.y = terrainHeight;

			// 회전 업데이트
			if (moveDir.LengthSquared() > 0.001f) {
				clients[c_id]._look_dir = moveDir;
			}

			// 모든 클라이언트에게 이동 패킷 전송
			for (auto& cl : clients) {
				if (cl._state != ST_INGAME) continue;
				cl.send_move_packet(&clients[c_id]);
			}
		}

		break;
	}
	}
}

bool GameManager::CanMove(float x, float z)
{
	if (terrain.mNavMapData.empty()) {
		return false;
	}
	float localX = x - terrain.GetOffset().x;
	float localZ = z - terrain.GetOffset().z;
	if (localX < 0.0f || localZ < 0.0f || localX >= terrain.GetScale().x || localZ >= terrain.GetScale().z) {
		return false;
	}
	float xIndex = localX / (terrain.GetScale().x / (terrain.GetNavMapResolution()));
	float zIndex = localZ / (terrain.GetScale().z / (terrain.GetNavMapResolution()));
	zIndex = terrain.GetNavMapResolution() - zIndex;
	xIndex = static_cast<int>(xIndex);
	zIndex = static_cast<int>(zIndex);

	int idx = xIndex + (zIndex * terrain.GetNavMapResolution());
	if (terrain.mNavMapData[idx] != 0) {
		return true;
	}

	return false;
}