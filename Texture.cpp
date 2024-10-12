#include "stdafx.h"
#include "Texture.h"
#include"DX12Manager.h"


void CTexture::LoadFromFile(std::wstring_view _fileName)
{
	if (texResource) {
		texResource.Reset();
	}

	std::wstring extension = std::filesystem::path(_fileName).extension();

	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;


	if (extension == L".dds" || extension == L".DDS") {
		ThrowIfFailed(DirectX::LoadDDSTextureFromFileEx(DEVICE, _fileName.data(), 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT,
			texResource.GetAddressOf(), ddsData, vSubresources, &ddsAlphaMode, &bIsCubeMap));
	}
	else {
		vSubresources.push_back(D3D12_SUBRESOURCE_DATA());

		ThrowIfFailed(DirectX::LoadWICTextureFromFileEx(DEVICE, _fileName.data(), 0, D3D12_RESOURCE_FLAG_NONE, WIC_LOADER_DEFAULT,
			texResource.GetAddressOf(), ddsData, vSubresources[0]));
	}
	

	UINT nSubResources = (UINT)vSubresources.size();
	UINT64 nBytes = GetRequiredIntermediateSize(texResource.Get(), 0, nSubResources);

	D3D12_HEAP_PROPERTIES heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(nBytes);

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperty,
		D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

	UpdateSubresources(CMDLIST, texResource.Get(), uploadBuffer.Get(), 0, 0, nSubResources, vSubresources.data());

	D3D12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texResource.Get(), 
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	
	CMDLIST->ResourceBarrier(1, &resourceBarrier);
}

void CTexture::Create2DTexture(DXGI_FORMAT format, UINT width, UINT height, 
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resFlags, XMFLOAT4 clearColor)
{
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
	desc.Flags = resFlags;

	D3D12_CLEAR_VALUE optimizedClearValue = {};
	D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;

	if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;
		optimizedClearValue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
	}
	else if (resFlags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
		float arrFloat[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
		optimizedClearValue = CD3DX12_CLEAR_VALUE(format, arrFloat);
	}

	DEVICE->CreateCommittedResource(&heapProperty,heapFlags, &desc, resourceStates, &optimizedClearValue, IID_PPV_ARGS(&texResource));

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		texType = TEXTURE_TYPE::TEXTURE2D;
		break;
	case D3D12_RESOURCE_DIMENSION_BUFFER:
		texType = TEXTURE_TYPE::BUFFER;
		break;
	default:
		break;
	}
}

void CTexture::CreateFromResource(ComPtr<ID3D12Resource> resource)
{
	texResource = resource;

	switch (texResource->GetDesc().Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
		texType = TEXTURE_TYPE::TEXTURE2D;
		break;
	case D3D12_RESOURCE_DIMENSION_BUFFER:
		texType = TEXTURE_TYPE::BUFFER;
		break;
	default:
		break;
	}
}


void CTexture::ReleaseUploadBuffer()
{
	if (uploadBuffer)
	{
		uploadBuffer.Reset();
	}
}

void CTexture::CreateSRV()
{
	auto descriptorHeap = INSTANCE(CDX12Manager).GetDescriptorHeaps();
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = GetShaderResourceViewDesc();

	descriptorHeap->CreateSRV(texResource, desc, srvIdx);
}

ComPtr<ID3D12Resource> CTexture::GetResource() const
{
	return texResource;
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetShaderResourceViewDesc()
{

	D3D12_RESOURCE_DESC resourceDesc = texResource->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (texType)
	{
	case TEXTURE_TYPE::TEXTURE2D: 
	case TEXTURE_TYPE::TEXTURE2D_ARRAY: //[]
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case TEXTURE_TYPE::TEXTURE2DARRAY:
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MipLevels = -1;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.PlaneSlice = 0;
		srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
		break;
	case TEXTURE_TYPE::TEXTURECUBE:
		srvDesc.Format = resourceDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case TEXTURE_TYPE::BUFFER:
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = 1;
		srvDesc.Buffer.StructureByteStride = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}

	return srvDesc;
}
