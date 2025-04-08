#include "stdafx.h"
#include "ServerManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

void ServerManager::Client_Login()
{
	// ------- 서버 붙이기 -------------------
	std::wcout.imbue(std::locale("korean")); // 한글로 오류 출력

	WSADATA WSAData{};
	int err = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (0 != err) {
		print_error("WSAStartup", WSAGetLastError());
	}

	server_soket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);

	//cout << "연결할 서버의 주소를 입력하세용 : ";
	//cin >> SERVER_ADDR;

	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	err = connect(server_soket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (0 != err) {
		print_error("connect", WSAGetLastError());
	}

	WSAOVERLAPPED wsaover;
	ZeroMemory(&wsaover, sizeof(wsaover));

	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;

	Send_Packet(&p);

	std::thread recv_thread{ [this]() { Recv_Loop(); } };
	recv_thread.detach();
}

void ServerManager::Recv_Packet()
{
	DWORD recv_flag = 0;

	OVER_PLUS* sdata = new OVER_PLUS();
	sdata->_over.hEvent = reinterpret_cast<HANDLE>(this);  // hEvent에 OVER_PLUS 포인터를 설정

	// hEvent를 올바르게 초기화
	wsaover.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (wsaover.hEvent == nullptr) {
		// 오류 처리
		print_error("CreateEvent failed", GetLastError());
	}
	wsaover.hEvent = reinterpret_cast<HANDLE>(this);

	int res = WSARecv(server_soket, &sdata->_wsabuf, 1, 0, &recv_flag, &sdata->_over, recv_callback);
	if (0 != res) {
		int err_no = WSAGetLastError();
		// 에러 겹친 i/o 작업을 진행하고 있습니다. 라고 나오는 게 정상임
		if (WSA_IO_PENDING != err_no)
			print_error("Recv_Packet - WSARecv", WSAGetLastError());
	}
}
void CALLBACK ServerManager::recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	OVER_PLUS* over = reinterpret_cast<OVER_PLUS*>(pwsaover);

	ServerManager* sm = reinterpret_cast<ServerManager*>(pwsaover->hEvent);
	if (sm == nullptr) {
		std::cout << "DDd" << std::endl;
		return;
	}
	char* buf = over->_wsabuf.buf;
	char recv_buf[CHAT_SIZE * 2];

	if (sm->save_data_size > 0) { // 전에 잘려서 저장해둔 패킷이 있으면 그거부터 하기
		memcpy(recv_buf, sm->save_buf, sm->save_data_size);
		memcpy(&recv_buf[sm->save_data_size], buf, sm->one_packet_size - sm->save_data_size);
		buf += sm->one_packet_size - sm->save_data_size;
		recv_size -= sm->one_packet_size - sm->save_data_size;
		sm->save_data_size = 0;
		sm->Using_Packet(recv_buf);
	}

	while (1) {
		if (recv_size == 0) break; // 남은 데이터가 없으면 끝
		WORD* byte = reinterpret_cast<WORD*>(buf);
		sm->one_packet_size = *byte; // 패킷 하나 사이즈 등록하기
		if (sm->one_packet_size > recv_size) { // 패킷 하나 사이즈보다 남은 버퍼 크기가 더 작으면 잘린거니까 save하기
			memcpy(sm->save_buf, buf, recv_size);
			sm->save_data_size = recv_size;
			break;
		}
		memcpy(recv_buf, buf, sm->one_packet_size);
		sm->Using_Packet(recv_buf);
		buf += sm->one_packet_size;
		recv_size -= sm->one_packet_size;
	}

	//delete over;
}

void ServerManager::Recv_Loop()
{
	while (1) {
		Recv_Packet();

		SleepEx(16, true);
	}
}

void ServerManager::Using_Packet(char* packet_ptr)
{
	switch (packet_ptr[2])
	{
	case SC_LOGIN_INFO: {
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(packet_ptr);

		my_info.SetID(packet->id);

		players[packet->id].SetID(packet->id);

		break;
	}
	case SC_LOGIN_FAIL: {
		cout << "로그인 실패!!!!" << endl;
		break;
	}
	case SC_MOVE_OBJECT: {
		SC_MOVE_PACKET* packet = reinterpret_cast<SC_MOVE_PACKET*>(packet_ptr);

		cout << "SC_MOVE_PACKET" << endl;
		//auto obj = SCENE.GetCurScene()->FindObjectWithID(packet->id);
		//if (obj) {
		//	obj->GetTransform()->SetLocalPosition({ packet->x, packet->y, packet->z });
		//	// obj->GetTransform()->SetLocalRotation({ packet->rot_x, packet->rot_y, packet->rot_z });
		//}

		break;
	}
	default:
		break;
	}
}

void ServerManager::Send_Packet(void* packet)
{
	OVER_PLUS* sdata = new OVER_PLUS{ reinterpret_cast<char*>(packet) };
	int sed = WSASend(server_soket, &sdata->_wsabuf, 1, 0, 0, &sdata->_over, send_callback);
	if (0 != sed) {
		int err_no = WSAGetLastError();
		// 에러 겹친 i/o 작업을 진행하고 있습니다. 라고 나오는 게 정상임
		if (WSA_IO_PENDING != err_no)
			print_error("Send_Packet - WSASend", WSAGetLastError());
	}
}
void CALLBACK ServerManager::send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
	cout << "Send_Packet" << endl;
	OVER_PLUS* over = reinterpret_cast<OVER_PLUS*>(pwsaover);
	delete over;
}

void ServerManager::print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf{};
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L"\t에러 : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}