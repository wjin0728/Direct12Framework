#pragma once
#include"stdafx.h"
#include"CResource.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CVertex
{
public:
	CVertex() { position = XMFLOAT3(0, 0, 0); normal = XMFLOAT3(0, 0, 0);};
	CVertex(float x, float y, float z) { position = XMFLOAT3(x, y, z); }
	CVertex(const XMFLOAT3& pos, const XMFLOAT3& nor);
	CVertex(const XMFLOAT3& pos, const XMFLOAT2& uv, const XMFLOAT3& nor, const XMFLOAT3& tan)
	: position(pos), texCoord(uv), normal(nor), tangent(tan) {};
	~CVertex() { }

public:
	XMFLOAT3 position{};
	XMFLOAT2 texCoord{};
	XMFLOAT3 normal{};
	XMFLOAT3 tangent{};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh : public CResource 
{
protected:
	//버텍스
	std::vector<CVertex> vertices{};
	ComPtr<ID3D12Resource> vertexBuffer{};
	ComPtr<ID3D12Resource> vertexUploadBuffer{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	UINT slot{};
	UINT stride = sizeof(CVertex);
	UINT offset{};

protected:
	//인덱스
	int subMeshNum = 1;
	int curSubSet{};

	std::vector<std::vector<UINT>> indices{};
	std::vector<ComPtr<ID3D12Resource>> indexBuffer{};
	std::vector<ComPtr<ID3D12Resource>> indexUploadBuffer{};
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView{};
	UINT startIndex{};
	int baseVertex{};

	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

public:
	BoundingOrientedBox oobb{};
	BoundingSphere oobs{};

public:
	CMesh() = default;
	CMesh(std::ifstream& inFile);
	virtual ~CMesh() = default;

public:
	void CreateBufferViews();

public:
	static std::shared_ptr<CMesh> CreateMeshFromFile(std::ifstream& inFile);
	static std::shared_ptr<CMesh> CreateCubeMesh(XMFLOAT3 scale = {1.f,1.f,1.f});
	static std::shared_ptr<CMesh> CreateAlphabetMesh(char type, XMFLOAT3 scale = { 1.f,1.f,1.f });
	static std::shared_ptr<CMesh> CreateRectangleMesh(XMFLOAT2 scale = { 1.f,1.f });
	static std::shared_ptr<CMesh> CreateSphereMesh(float radius, UINT stackCnt, UINT sliceCnt);

protected:
	void CreateVertexBufferView(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	void CreateIndexBufferView(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
	void CreateOOBBAndOOBS();
	virtual void CalculateNormal();

public:
	void ReleaseUploadBuffers();

	BOOL RayIntersectionByTriangle(const XMFLOAT3& xmRayOrigin, const XMFLOAT3& xmRayDirection,
		const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, float* pfNearHitDistance);

	int CheckRayIntersection(const XMFLOAT3& xmvPickRayOrigin, const XMFLOAT3& xmvPickRayDirection, float* pfNearHitDistance);
public:
	virtual void Render(ID3D12GraphicsCommandList* cmdList);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, int idx);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, UINT nInstances, const D3D12_VERTEX_BUFFER_VIEW& d3dInstancingBufferView);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, UINT nInstances);
};


ID3D12Resource* CreateBufferResource(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
	void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer);