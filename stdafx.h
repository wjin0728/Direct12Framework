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
#include<stack>
#include<map>
#include"algorithm"
#include <Mmsystem.h>
#include <codecvt>

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
#include"BinaryReader.h"

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

using Vec2 = SimpleMath::Vector2;
using Vec3 = SimpleMath::Vector3;
using Vec4 = SimpleMath::Vector4;
using SimpleMath::Matrix;
using SimpleMath::Quaternion;
using SimpleMath::Plane;
using SimpleMath::Ray;
using SimpleMath::Color;

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
#define UPLOADBUFFER(T) INSTANCE(CDX12Manager).GetBuffer(static_cast<UINT>(T))
#define RESOURCE INSTANCE(CResourceManager)

template <typename T, template <typename, typename> class Container, typename Alloc = std::allocator<std::shared_ptr<T>>>
typename Container<std::shared_ptr<T>, Alloc>::iterator 
findByRawPointer(Container<std::shared_ptr<T>, Alloc>& container, T* rawPtr) {
	return std::find_if(container.begin(), container.end(),
		[rawPtr](const std::shared_ptr<T>& ptr) { return ptr.get() == rawPtr; });
}



enum class SCENE_TYPE : UINT8
{
	MENU,
	MAINSTAGE,
	MAIN,

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

enum {
	SCENE_TYPE_COUNT = SCENE_TYPE::END,
	RESOURCE_TYPE_COUNT = RESOURCE_TYPE::END
};

enum class COMPONENT_TYPE : UINT8
{
	TRANSFORM,
	COLLIDER,
	MESH_RENDERER,
	CAMERA,
	LIGHT,
	TERRAIN,
	RIGID_BODY,
	// ...
	SCRIPT,

	NOTHING,
	END
};

#define DIRECTIONAL_LIGHT 5
#define POINT_LIGHT 5
#define SPOT_LIGHT 5
#define DETAIL_MAP 5


struct CBMaterialDate {
	Color albedoColor{};
	Color specularColor{};
	Color emissiveColor{};
	Vec3 fresnelR0{};
	int diffuseMapIdx = -1;
	int normalMapIdx = -1;
	Vec3 padding1{};
};


struct CBTerrainMaterialDate {
	CBMaterialDate material;
	int detailMapIdx = -1;
	Vec3 padding1{};
};

struct CBPassData
{
	Matrix viewMat = Matrix::Identity;
	Matrix projMat = Matrix::Identity;
	Matrix viewProjMat = Matrix::Identity;
	Vec3 camPos = Vec3::Zero;
	float padding{};
	Vec2 renderTargetSize{};
	float deltaTime{};
	float totalTime{};
	CBTerrainMaterialDate terrainMat;
};

struct CBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix textureMat = Matrix::Identity;
	int materialIdx = -1;
	Vec3 padding;
};

struct CBDirectionalLightInfo
{
	Color color;
	Vec3 strength;
	float padding1;
	Vec3 direction;
	float padding2;
};

struct CBPointLightInfo
{
	Color color;
	Vec3 strength;
	float range;
	Vec3 position;
	float padding;
};

struct CBSpotLightInfo
{
	Color color;
	Vec3 strength;
	float range;
	Vec3 direction;
	float fallOffStart;
	Vec3 position;
	float fallOffEnd;
	Vec3 padding;
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