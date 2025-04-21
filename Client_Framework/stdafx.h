#pragma once

#define WIN32_LEAN_AND_MEAN		
#define _HAS_STD_BYTE 0

#include "../Server_Framework/OVER_PLUS.h"
#include "../Server_Framework/protocol.h"
// #include "../Server_Framework/ENUM.h"
#include <Windows.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "MSWSock.lib")

#include <stdlib.h>
#include <iostream>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <random>
#include <array>
#include <queue>
#include <stack>
#include <map>
#include "algorithm"
#include <Mmsystem.h>
#include <codecvt>

#include <d3d12.h>
#include "SimpleMath.h"
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
#include "d3dx12.h"
#include "DDSTextureLoader12.h"
#include "WICTextureLoader12.h"
#include "BinaryReader.h"

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

#include "Enums.h"
#include "CBData.h"

#define _DEBUG

#define FRAMEBUFFER_WIDTH		2000
#define FRAMEBUFFER_HEIGHT		1200

#define RANDOM_COLOR			(0xFF000000 | ((rand() * 0xFFFFFF) / RAND_MAX))

#define TEXTURE_PATH(name)			"..\\Resources\\Textures\\" + std::string(name) + ".dds"
#define MODEL_PATH(name)			"..\\Resources\\Models\\" + (name) + ".bin"
#define SCENE_PATH(name)			"..\\Resources\\Scenes\\" + (name) + ".bin"
#define ANIMATION_PATH(name)		"..\\Resources\\Animation\\" + (name) + ".bin"
#define SHADER_PATH(name)			"..\\Resources\\Shaders\\" + std::string(name) + ".hlsl"

#define ALIGNED_SIZE(size)				((size + 255) & ~255)

template<typename T>
using StrDic = std::unordered_map<std::string, std::shared_ptr<T>>;

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

inline void PrintDebug(const std::string& str)
{
	TCHAR pstrDebug[256] = { 0 };

	std::wstring wtag;
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	if (size > 0) {
		wtag.resize(size);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wtag[0], size);
	}

	_stprintf_s(pstrDebug, 256, _T("%s\n"), wtag.c_str());
	OutputDebugString(pstrDebug);
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
#define CONSTANTBUFFER(T) INSTANCE(CDX12Manager).GetConstantBuffer(static_cast<UINT>(T))
#define STRUCTEDBUFFER(T) INSTANCE(CDX12Manager).GetStructedBuffer(static_cast<UINT>(T))
#define INSTANCINGBUFFER(T) INSTANCE(CDX12Manager).GetInstancingBuffer(static_cast<UINT>(T))
#define RESOURCE INSTANCE(CResourceManager)


template <typename T, template <typename, typename> class Container, typename Alloc = std::allocator<std::shared_ptr<T>>>
typename Container<std::shared_ptr<T>, Alloc>::iterator 
findByRawPointer(Container<std::shared_ptr<T>, Alloc>& container, T* rawPtr) {
	return std::find_if(container.begin(), container.end(),
		[rawPtr](const std::shared_ptr<T>& ptr) { return ptr.get() == rawPtr; });
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer);

void PrintMatrix(const Matrix& mat);

