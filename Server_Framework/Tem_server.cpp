#include "GameManager.h"

int main() {
    GameManager& gm = GameManager::GetInstance();
    gm.StartWorkerThreads(); // ��Ŀ ������ ����
    gm.Run(); // ���� ���� ����
    return 829; // ������ ���� �����̹Ƿ� �������� ����
}