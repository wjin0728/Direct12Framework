#pragma once

#define WIN32_LEAN_AND_MEAN		
#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include<vector>
#include<unordered_map>
#include<memory>
#include<fstream>
#include<filesystem>
#include<chrono>
#include<random>
#include<array>
#include<queue>
#include<map>

#include <Mmsystem.h>

#include <d3d12.h>
#include"SimpleMath.h"
#include <dxgi1_4.h>
#include <wrl.h>
#include <shellapi.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <D3Dcompiler.h>
#include <DXGIDebug.h>
#include <comdef.h>
#include"d3dx12.h"
#include"DDSTextureLoader12.h"
#include"WICTextureLoader12.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")


#define _DEBUG

#define FRAMEBUFFER_WIDTH		1200
#define FRAMEBUFFER_HEIGHT		720

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define EXPLOSION_DEBRISES		60

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))


using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;


inline XMFLOAT4 GetRandomColor() {
	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_real_distribution<float> randColor(0, 1);

	return XMFLOAT4(randColor(dre), randColor(dre), randColor(dre), 1.f);
}

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::wstring ToString()const 
	{
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
	}

	HRESULT ErrorCode = S_OK;
	std::wstring FunctionName;
	std::wstring Filename;
	int LineNumber = -1;
};



#define ThrowIfFailed(x)															\
{																					\
    HRESULT hr__ = (x);																\
    std::wstring wfn = AnsiToWString(__FILE__);										\
    if(FAILED(hr__)) {																\
		throw DxException(hr__, L#x, wfn, __LINE__); 								\
	}																				\
}																					


#define MAKE_SINGLETON(T)		\
private:						\
	T() {}						\
	~T() {}						\
public:							\
	static T& GetInstance()		\
	{							\
		static T instance;		\
		return instance;		\
	}							\

#define INSTANCE(T) T::GetInstance()

//class CDX12Manager;

#define DEVICE INSTANCE(CDX12Manager).GetDevice()
#define CMDLIST INSTANCE(CDX12Manager).GetCommandList()
#define CONSTBUFFER(T) INSTANCE(CDX12Manager).GetConstBuffer(T)


namespace BinaryReader
{
	template<typename Type>
	inline void ReadDateFromFile(std::ifstream& inFile, Type& date)
	{
		inFile.read((char*)&date, sizeof(Type));

		if (!inFile) {
			ZeroMemory(&date, sizeof(Type));
		}
	}

	template<>
	inline void ReadDateFromFile(std::ifstream& inFile, std::string& date)
	{
		char strLen = 0;

		inFile.read(reinterpret_cast<char*>(&strLen), sizeof(strLen));

		if (!inFile) {
			date.clear();
		}



		date.resize((int)strLen);
		inFile.read(&date[0], strLen);

		if (!inFile) {
			date.clear();
		}
	}
}


enum class SCENE_TYPE : UINT8
{
	MENU,
	MAINSTAGE,

	END
};

enum class RESOURCE_TYPE : UINT8
{
	MATERIAL,
	MESH,
	SHADER,
	TEXTURE,

	END
};

enum class OBJECT_TYPE : UINT8
{
	ENEMY,
	PLAYER_BULLET,
	ENEMY_BULLET,

	END
};

enum {
	SCENE_TYPE_COUNT = SCENE_TYPE::END,
	RESOURCE_TYPE_COUNT = RESOURCE_TYPE::END,
	OBJECT_TYPE_COUNT = OBJECT_TYPE::END
};


#define DIRECTIONAL_LIGHT 5
#define POINT_LIGHT 5
#define SPOT_LIGHT 5


struct CBPassData
{
	XMFLOAT4X4 viewMat;
	XMFLOAT4X4 projMat;
	XMFLOAT4X4 viewProjMat;
	XMFLOAT3 camPos;
	float padding;
	XMFLOAT2 renderTargetSize;
	float deltaTime;
	float totalTime;
};

struct CBObjectData
{
	XMFLOAT4X4 worldMAt;
	UINT materialIdx;
	XMFLOAT3 padding;
};

struct CBDirectionalLightInfo
{
	XMFLOAT4 color;
	XMFLOAT3 strength;
	float padding1;
	XMFLOAT3 direction;
	float padding2;
};

struct CBPointLightInfo
{
	XMFLOAT4 color;
	XMFLOAT3 strength;
	float range;
	XMFLOAT3 position;
	float padding;
};

struct CBSpotLightInfo
{
	XMFLOAT4 color;
	XMFLOAT3 strength;
	float range;
	XMFLOAT3 direction;
	float fallOffStart;
	XMFLOAT3 position;
	float fallOffEnd;
	XMFLOAT3 padding;
	float spotPower;
};


struct CBLightsData
{
	CBDirectionalLightInfo dirLights[DIRECTIONAL_LIGHT];
	CBPointLightInfo pointLights[POINT_LIGHT];
	CBSpotLightInfo spotLights[SPOT_LIGHT];
	XMUINT3 lightNum;
	UINT padding;
};