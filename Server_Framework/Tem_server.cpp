#include "GameManager.h"

int main() {
    GameManager& gm = GameManager::GetInstance();
    gm.StartWorkerThreads(); // 워커 스레드 시작
    gm.Run(); // 게임 루프 실행
    return 829; // 서버는 무한 루프이므로 도달하지 않음
}