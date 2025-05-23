#pragma once
#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <thread>
#include <vector>
#include <mutex>
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