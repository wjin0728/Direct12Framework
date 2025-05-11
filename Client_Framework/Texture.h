#pragma once
#include"stdafx.h"
#include"CResource.h"

enum TEXTURE_TYPE {
	TEXTURE2D,
	TEXTURE2D_ARRAY,
	TEXTURE2DARRAY,
	TEXTURECUBE,
	BUFFER,
	DEPTH_STENCIL
};

class CTexture : public CResource, public std::enable_shared_from_this<CTexture>
{
public:
	CTexture(bool isSR = true, TEXTURE_TYPE texType = TEXTURE2D);
	CTexture(const std::string& name, const std::string& _fileName, bool isSR = true, TEXTURE_TYPE texType = TEXTURE2D);
	CTexture(const std::string& name, DXGI_FORMAT format, void* data, size_t dataSize, UINT width, UINT height,
		const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
		D3D12_RESOURCE_FLAGS resFlags, XMFLOAT4 clearColor = { 0.f,0.f,0.f,1.f });

	~CTexture();

protected:
	std::string fileName{};
	ComPtr<ID3D12Resource> texResource = nullptr;
	ComPtr<ID3D12Resource> uploadBuffer = nullptr;

	D3D12_RESOURCE_DESC desc{};
	D3D12_HEAP_PROPERTIES heapProperty;
	D3D12_HEAP_FLAGS heapFlags;

	BYTE* ddsData = nullptr;
	UINT width = 0;
	UINT height = 0;
	size_t dataSize = 0;

	TEXTURE_TYPE texType{};
	INT srvIdx = -1;

public:
	bool isSR{true};
	virtual void LoadFromFile(const std::string& _fileName) override;

	void Create2DTexture();
	void CreateFromResource(ComPtr<ID3D12Resource> resource);

	virtual void CreateGPUResource() override;
	virtual void ReleaseUploadBuffer();


	ComPtr<ID3D12Resource>& GetResource();
	UINT GetSrvIndex();
	TEXTURE_TYPE GetTextureType() const { return texType; }
	D3D12_SHADER_RESOURCE_VIEW_DESC GetSRVDesc();
	D3D12_DEPTH_STENCIL_VIEW_DESC GetDSVDesc();

	void SetSrvIndex(UINT idx) { srvIdx = idx; }
	void SetTextureType(TEXTURE_TYPE type) { texType = type; }

	void AssignedSRVIndex();
	int CreateSRV();
	void CreateUAV();

	void ChangeResourceState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
};