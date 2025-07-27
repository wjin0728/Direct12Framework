#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <unordered_set>
#include <concurrent_priority_queue.h>
#include <fstream>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")
#include <windows.h>  
#include <sqlext.h>  

#include "ENUM.h"
#include "protocol.h"
#include "../Client_Framework/AnimationEnums.h"
#include "../BinaryReader.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "../Client_Framework/SimpleMath.h"
#include <DirectXCollision.h>

using namespace std;

using namespace DirectX;
using namespace DirectX::PackedVector;
using Vec3 = SimpleMath::Vector3;
using SimpleMath::Quaternion;
using SimpleMath::Matrix;

// 고정 틱 간격 (60Hz)
constexpr float TICK_INTERVAL = 1.0f / 60.0f; // 약 0.016667초

constexpr float SPAWN_INTERVAL = 3.f; // 몬스터 스폰 간격 (초 단위)
constexpr float WAVE_INTERVAL = 1.f; // 웨이브 시작 대기 간격 (초 단위)
constexpr float MESSAGE_INTERVAL = 4.f; // 메시지 출력 간격 (초 단위)

constexpr int P_FIRE_EXPLOSION_DAMAGE = 15;
constexpr int P_GRASS_VINE_DAMAGE = 5;

constexpr int P_FIGHTER_DAMAGE = 10;
constexpr int P_ARROW_MAGICBALL_DAMAGE = 5;
constexpr int P_ULTIMAGE_DAMAGE = 25;

constexpr int M_SMALL_DAMAGE = 50;
constexpr int M_BIG_DAMAGE = 100;
constexpr int M_BOSS_DAMAGE = 200;