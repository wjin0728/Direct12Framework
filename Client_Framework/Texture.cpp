#include "stdafx.h"
#include "Texture.h"
#include"DX12Manager.h"
#include"ResourceManager.h"


CTexture::CTexture(bool isSR, TEXTURE_TYPE texType) : isSR(isSR), texType(texType)
{
}

CTexture::CTexture(const std::string& name, const std::string& _fileName, bool isSR, TEXTURE_TYPE texType) : isSR(isSR), texType(texType)
{
	this->name = name;
	fileName = _fileName;
	texResource = nullptr;
	uploadBuffer = nullptr;
	srvIdx = -1;
}
CTexture::CTexture(const std::string& name, DXGI_FORMAT format, void* data, size_t dataSize, UINT width, UINT height, const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resFlags, XMFLOAT4 clearColor)
{
	this->name = name;
	texResource = nullptr;
	uploadBuffer = nullptr;
	srvIdx = -1;
	desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
	desc.Flags = resFlags;
	this->width = width;
	this->height = height;
	this->dataSize = dataSize;
	this->heapProperty = heapProperty;
	this->heapFlags = heapFlags;

	if(data) ddsData = (BYTE*)data;

	isSR = true;
}
CTexture::~CTexture()
{
	if (srvIdx != -1) {
		INSTANCE(CResourceManager).ReturnSRVIndex(srvIdx);
	}
}

void CTexture::LoadFromFile(const std::string& _fileName)
{
	if (texResource) {
		texResource.Reset();
	}

	std::wstring fileName = BinaryReader::stringToWstring(_fileName.data());
	std::wstring extension = std::filesystem::path(fileName).extension();
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DDS_ALPHA_MODE ddsAlphaMode = DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;

	if (extension == L".dds" || extension == L".DDS") {
		HRESULT hr = DirectX::LoadDDSTextureFromFileEx(DEVICE, fileName.data(), 0, D3D12_RESOURCE_FLAG_NONE, DDS_LOADER_DEFAULT,
			texResource.GetAddressOf(), ddsData, vSubresources, &ddsAlphaMode, &bIsCubeMap);
		if (hr < 0) {
			MessageBoxA(nullptr, (_fileName + "don't exist in file!").c_str(), nullptr, MB_OK);
		}
		ThrowIfFailed(hr);
	}
	else {
		vSubresources.push_back(D3D12_SUBRESOURCE_DATA());
		HRESULT hr = DirectX::LoadWICTextureFromFileEx(DEVICE, fileName.data(), 0, D3D12_RESOURCE_FLAG_NONE, WIC_LOADER_DEFAULT,
			texResource.GetAddressOf(), ddsData, vSubresources[0]);
		if (hr < 0) {
			MessageBoxA(nullptr, (_fileName + "don't exist in file!").c_str(), nullptr, MB_OK);
		}
		ThrowIfFailed(hr);
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

	CreateSRV();
}

void CTexture::Create2DTexture()
{
	D3D12_CLEAR_VALUE* optimizedClearValue = NULL;
	D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;

	if (desc.Flags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE;

		DXGI_FORMAT clearFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (desc.Format == DXGI_FORMAT_R32_TYPELESS) {
			clearFormat = DXGI_FORMAT_D32_FLOAT;
		}
		if (name == "DepthStencil") {
#ifdef REVERSE_Z
			optimizedClearValue = new CD3DX12_CLEAR_VALUE(clearFormat, 0.f, 0);
#else
			optimizedClearValue = new CD3DX12_CLEAR_VALUE(clearFormat, 1.0f, 0);
#endif // REVERSE_Z
		}
		else if(name == "ShadowMap") optimizedClearValue = new CD3DX12_CLEAR_VALUE(clearFormat, 1.0f, 0);
	}
	else if (desc.Flags & D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		resourceStates = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
		float arrFloat[4] = { 0.f,0.f,0.f,1.f };
		optimizedClearValue = new CD3DX12_CLEAR_VALUE(desc.Format, arrFloat);
	}

	ThrowIfFailed(DEVICE->CreateCommittedResource(&heapProperty, heapFlags, &desc, resourceStates, optimizedClearValue, IID_PPV_ARGS(&texResource)));

	if (optimizedClearValue) delete optimizedClearValue;

	if (ddsData) {
		const UINT num2DSubresources = desc.DepthOrArraySize * 1;
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texResource.Get(), 0, num2DSubresources);


		size_t rowPitch = dataSize * width;

		CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);

		DEVICE->CreateCommittedResource(
			&uploadHeapProps,
			D3D12_HEAP_FLAG_NONE,
			&uploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer)
		);

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = ddsData;
		textureData.RowPitch = rowPitch;
		textureData.SlicePitch = rowPitch * height;

		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			texResource.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			D3D12_RESOURCE_STATE_COPY_DEST
		);

		CMDLIST->ResourceBarrier(1, &barrier);

		UpdateSubresources(CMDLIST, texResource.Get(), uploadBuffer.Get(), 0, 0, 1, &textureData);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			texResource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ
		);

		CMDLIST->ResourceBarrier(1, &barrier);
	}

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
	if (resourceStates == D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_DEPTH_WRITE) {
		texType = TEXTURE_TYPE::DEPTH_STENCIL;
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


void CTexture::CreateGPUResource()
{
	if (isLoaded) return;
	if (fileName.empty()) Create2DTexture();
	else LoadFromFile(fileName);
	isLoaded = true;
}

void CTexture::ReleaseUploadBuffer()
{
	if (uploadBuffer)
	{
		uploadBuffer.Reset();
	}
}

void CTexture::AssignedSRVIndex()
{
	if (isSR && srvIdx == -1) {
		srvIdx = INSTANCE(CResourceManager).GetTopSRVIndex();
	}
}

int CTexture::CreateSRV()
{
	if ((srvIdx != -1)) {
		auto descriptorHeap = INSTANCE(CDX12Manager).GetDescriptorHeaps();

		if (texType == TEXTURECUBE) {
			descriptorHeap->CreateCubeMap(shared_from_this(), 0);
		}
		else {
			descriptorHeap->CreateSRV(shared_from_this(), srvIdx);
		}
	}

	return srvIdx;
}

void CTexture::CreateUAV()
{
}

void CTexture::ChangeResourceState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(texResource.Get(), before, after);
	CMDLIST->ResourceBarrier(1, &transition);
}

ComPtr<ID3D12Resource>& CTexture::GetResource()
{
	return texResource;
}

UINT CTexture::GetSrvIndex()
{
	AssignedSRVIndex();
	return srvIdx;
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::GetSRVDesc()
{

	D3D12_RESOURCE_DESC resourceDesc = texResource->GetDesc();
	
	/*if (!fileName.empty()) {
		cout << fileName << " Format : ";
		switch (resourceDesc.Format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			cout << "R32G32B32A32_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R32G32B32A32_UINT:
			cout << "R32G32B32A32_UINT" << endl;
			break;
		case DXGI_FORMAT_R32G32B32A32_SINT:
			cout << "R32G32B32A32_SINT" << endl;
			break;
		case DXGI_FORMAT_R32G32B32_FLOAT:
			cout << "R32G32B32_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R32G32B32_UINT:
			cout << "R32G32B32_UINT" << endl;
			break;
		case DXGI_FORMAT_R32G32B32_SINT:
			cout << "R32G32B32_SINT" << endl;
			break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			cout << "R16G16B16A16_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R16G16B16A16_UINT:
			cout << "R16G16B16A16_UINT" << endl;
			break;
		case DXGI_FORMAT_R16G16B16A16_SINT:
			cout << "R16G16B16A16_SINT" << endl;
			break;
		case DXGI_FORMAT_R16G16B16A16_UNORM:
			cout << "R16G16B16A16_UNORM" << endl;
			break;
		case DXGI_FORMAT_R16G16B16A16_SNORM:
			cout << "R16G16B16A16_SNORM" << endl;
			break;
		case DXGI_FORMAT_R32G32_FLOAT:
			cout << "R32G32_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R32G32_UINT:
			cout << "R32G32_UINT" << endl;
			break;
		case DXGI_FORMAT_R32G32_SINT:
			cout << "R32G32_SINT" << endl;
			break;
		case DXGI_FORMAT_R32G8X24_TYPELESS:
			cout << "R32G8X24_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			cout << "D32_FLOAT_S8X24_UINT" << endl;
			break;
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			cout << "R32_FLOAT_X8X24_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			cout << "X32_TYPELESS_G8X24_UINT" << endl;
			break;
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			cout << "R10G10B10A2_UNORM" << endl;
			break;
		case DXGI_FORMAT_R10G10B10A2_UINT:
			cout << "R10G10B10A2_UINT" << endl;
			break;
		case DXGI_FORMAT_R11G11B10_FLOAT:
			cout << "R11G11B10_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			cout << "R8G8B8A8_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_R8G8B8A8_UINT:
			cout << "R8G8B8A8_UINT" << endl;
			break;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			cout << "R8G8B8A8_UNORM" << endl;
			break;
		case DXGI_FORMAT_R8G8B8A8_SNORM:
			cout << "R8G8B8A8_SNORM" << endl;
			break;
		case DXGI_FORMAT_R8G8B8A8_SINT:
			cout << "R8G8B8A8_SINT" << endl;
			break;
		case DXGI_FORMAT_R16G16_TYPELESS:
			cout << "R16G16_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_R16G16_FLOAT:
			cout << "R16G16_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R16G16_UINT:
			cout << "R16G16_UINT" << endl;
			break;
		case DXGI_FORMAT_R16G16_UNORM:
			cout << "R16G16_UNORM" << endl;
			break;
		case DXGI_FORMAT_R16G16_SNORM:
			cout << "R16G16_SNORM" << endl;
			break;
		case DXGI_FORMAT_R16G16_SINT:
			cout << "R16G16_SINT" << endl;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			cout << "R32_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_D32_FLOAT:
			cout << "D32_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R32_FLOAT:
			cout << "R32_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R32_UINT:
			cout << "R32_UINT" << endl;
			break;
		case DXGI_FORMAT_R32_SINT:
			cout << "R32_SINT" << endl;
			break;
		case DXGI_FORMAT_R24G8_TYPELESS:
			cout << "R24G8_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			cout << "D24_UNORM_S8_UINT" << endl;
			break;
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
			cout << "R24_UNORM_X8_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			cout << "X24_TYPELESS_G8_UINT" << endl;
			break;
		case DXGI_FORMAT_R8G8_TYPELESS:
			cout << "R8G8_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_R8G8_UINT:
			cout << "R8G8_UINT" << endl;
			break;
		case DXGI_FORMAT_R8G8_UNORM:
			cout << "R8G8_UNORM" << endl;
			break;
		case DXGI_FORMAT_R8G8_SNORM:
			cout << "R8G8_SNORM" << endl;
			break;
		case DXGI_FORMAT_R8G8_SINT:
			cout << "R8G8_SINT" << endl;
			break;
		case DXGI_FORMAT_R16_TYPELESS:
			cout << "R16_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_R16_FLOAT:
			cout << "R16_FLOAT" << endl;
			break;
		case DXGI_FORMAT_R16_UINT:
			cout << "R16_UINT" << endl;
			break;
		case DXGI_FORMAT_R16_UNORM:
			cout << "R16_UNORM" << endl;
			break;
		case DXGI_FORMAT_R16_SNORM:
			cout << "R16_SNORM" << endl;
			break;
		case DXGI_FORMAT_R16_SINT:
			cout << "R16_SINT" << endl;
			break;
		case DXGI_FORMAT_BC1_TYPELESS:
			cout << "BC1_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_BC1_UNORM:
			cout << "BC1_UNORM" << endl;
			break;
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			cout << "BC1_UNORM_SRGB" << endl;
			break;
		case DXGI_FORMAT_BC2_TYPELESS:
			cout << "BC2_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_BC2_UNORM:
			cout << "BC2_UNORM" << endl;
			break;
		case DXGI_FORMAT_BC2_UNORM_SRGB:
			cout << "BC2_UNORM_SRGB" << endl;
			break;
		case DXGI_FORMAT_BC3_TYPELESS:
			cout << "BC3_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_BC3_UNORM:
			cout << "BC3_UNORM" << endl;
			break;
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			cout << "BC3_UNORM_SRGB" << endl;
			break;
		case DXGI_FORMAT_BC4_TYPELESS:
			cout << "BC4_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_BC4_UNORM:
			cout << "BC4_UNORM" << endl;
			break;
		case DXGI_FORMAT_BC4_SNORM:
			cout << "BC4_SNORM" << endl;
			break;
		case DXGI_FORMAT_BC5_TYPELESS:
			cout << "BC5_TYPELESS" << endl;
			break;
		case DXGI_FORMAT_BC5_UNORM:
			cout << "BC5_UNORM" << endl;
			break;
		case DXGI_FORMAT_BC7_UNORM:
			cout << "BC7_UNORM" << endl;
			break;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			cout << "R8G8B8A8_UNORM_SRGB" << endl;
			break;
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			cout << "BC7_UNORM_SRGB" << endl;
			break;
		default:
			break;
		}
		if (resourceDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
			resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		if (resourceDesc.Format == DXGI_FORMAT_BC7_UNORM_SRGB) {
			resourceDesc.Format = DXGI_FORMAT_BC7_UNORM;
		}
		if (resourceDesc.Format == DXGI_FORMAT_BC1_UNORM_SRGB) {
			resourceDesc.Format = DXGI_FORMAT_BC1_UNORM;
		}
		if (resourceDesc.Format == DXGI_FORMAT_BC2_UNORM_SRGB) {
			resourceDesc.Format = DXGI_FORMAT_BC2_UNORM;
		}
		if (resourceDesc.Format == DXGI_FORMAT_BC3_UNORM_SRGB) {
			resourceDesc.Format = DXGI_FORMAT_BC3_UNORM;
		}
		if (resourceDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) {
			resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
	}*/

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
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
	case TEXTURE_TYPE::DEPTH_STENCIL: //[]
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		if (resourceDesc.Format == DXGI_FORMAT_R32_TYPELESS) {
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}

		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
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

D3D12_DEPTH_STENCIL_VIEW_DESC CTexture::GetDSVDesc()
{
	D3D12_RESOURCE_DESC resourceDesc = texResource->GetDesc();

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	switch (resourceDesc.Format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	default:
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		break;
	}
	return dsvDesc;
}
