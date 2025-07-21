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

#include "../../Server_Framework/ENUM.h"
#include "../../Server_Framework/protocol.h"
#include "../../Server_Framework/OVER_PLUS.h"
#include "../../Client_Framework/BinaryReader.h"
#include "../../Client_Framework/AnimationEnums.h"

#include "../../Client_Framework/SimpleMath.h"

using namespace std;