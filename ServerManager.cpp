#include "stdafx.h"
#include "ServerManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

void ServerManager::Client_Login()
{
	// ------- ���� ���̱� -------------------
	std::wcout.imbue(std::locale("korean")); // �ѱ۷� ���� ���

	WSADATA WSAData{};
	int err = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (0 != err) {
		print_error("WSAStartup", WSAGetLastError());
	}

	server_soket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);

	//cout << "������ ������ �ּҸ� �Է��ϼ��� : ";
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
	sdata->_over.hEvent = reinterpret_cast<HANDLE>(this);  // hEvent�� OVER_PLUS �����͸� ����

	// hEvent�� �ùٸ��� �ʱ�ȭ
	wsaover.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (wsaover.hEvent == nullptr) {
		// ���� ó��
		print_error("CreateEvent failed", GetLastError());
	}
	wsaover.hEvent = reinterpret_cast<HANDLE>(this);

	int res = WSARecv(server_soket, &sdata->_wsabuf, 1, 0, &recv_flag, &sdata->_over, recv_callback);
	if (0 != res) {
		int err_no = WSAGetLastError();
		// ���� ��ģ i/o �۾��� �����ϰ� �ֽ��ϴ�. ��� ������ �� ������
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

	if (sm->save_data_size > 0) { // ���� �߷��� �����ص� ��Ŷ�� ������ �װź��� �ϱ�
		memcpy(recv_buf, sm->save_buf, sm->save_data_size);
		memcpy(&recv_buf[sm->save_data_size], buf, sm->one_packet_size - sm->save_data_size);
		buf += sm->one_packet_size - sm->save_data_size;
		recv_size -= sm->one_packet_size - sm->save_data_size;
		sm->save_data_size = 0;
		sm->Using_Packet(recv_buf);
	}

	while (1) {
		if (recv_size == 0) break; // ���� �����Ͱ� ������ ��
		WORD* byte = reinterpret_cast<WORD*>(buf);
		sm->one_packet_size = *byte; // ��Ŷ �ϳ� ������ ����ϱ�
		if (sm->one_packet_size > recv_size) { // ��Ŷ �ϳ� ������� ���� ���� ũ�Ⱑ �� ������ �߸��Ŵϱ� save�ϱ�
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
		cout << "�α��� ����!!!!" << endl;
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
		// ���� ��ģ i/o �۾��� �����ϰ� �ֽ��ϴ�. ��� ������ �� ������
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
	std::wcout << L"\t���� : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}