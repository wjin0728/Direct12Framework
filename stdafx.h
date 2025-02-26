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

using Vec2 = SimpleMath::Vector2;
using Vec3 = SimpleMath::Vector3;
using Vec4 = SimpleMath::Vector4;
using SimpleMath::Matrix;
using SimpleMath::Quaternion;
using SimpleMath::Plane;
using SimpleMath::Ray;
using SimpleMath::Color;

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include"Enums.h"

#define _DEBUG

#define FRAMEBUFFER_WIDTH		2000
#define FRAMEBUFFER_HEIGHT		1200

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))



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

ID3D12Resource* CreateBufferResource(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer);


constexpr auto DIRECTIONAL_LIGHT = 5;
constexpr auto POINT_LIGHT = 5;
constexpr auto SPOT_LIGHT = 5;
constexpr auto TERRAIN_SPLAT_COUNT = 7;


struct CBMaterialDate {
	Color albedoColor{};
	Color specularColor{};
	Color emissiveColor{};
	Vec3 fresnelR0{};
	int diffuseMapIdx = -1;
	int normalMapIdx = -1;
	Vec3 padding1{};
};

struct TerrainSplat
{
	int layerNum;
	struct Layer
	{
		int diffuseIdx;
		int normalIdx;

		float metallic;
		float smoothness;
	} layer[4];
};

struct CBTerrainDate {
	Vec3 scale = Vec3::One;
	int heightMapIdx = -1;

	int splatNum;
	TerrainSplat splats[TERRAIN_SPLAT_COUNT];
};

struct CBPassData
{
	Matrix viewProjMat = Matrix::Identity;
	Matrix shadowTransform = Matrix::Identity;

	Vec4 gFogColor = { 0.7f, 0.7f, 0.7f, 1.0f };

	Vec3 camPos = Vec3::Zero;
	UINT shadowMapIdx{};

	Vec2 renderTargetSize{};
	Vec2 padding2;

	float deltaTime{};
	float totalTime{};
	float gFogStart = 50.0f;
	float gFogRange = 300.0f;

	CBTerrainDate terrainMat;
};

struct CBObjectData
{
	Matrix worldMAt = Matrix::Identity;
	Matrix invWorldMAt = Matrix::Identity;
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

struct BillboardData
{
	Vec3 position{};
	Vec2 size{};
	int materialIdx = -1;
	Matrix textureMat = Matrix::Identity;
};