#pragma once
#include"stdafx.h"
#include"CResource.h"
#include"VertexBuffer.h"
#include"IndexBuffer.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CVertex
{
public:
	CVertex() { position = Vec3(0, 0, 0); normal = Vec3(0, 0, 0);};
	CVertex(float x, float y, float z) { position = Vec3(x, y, z); }
	CVertex(const Vec3& pos) { position = pos; }
	CVertex(const Vec3& pos, const Vec3& nor);
	CVertex(const Vec3& pos, const Vec2& uv, const Vec3& nor, const Vec3& tan)
	: position(pos), texCoord(uv), normal(nor), tangent(tan) {};
	~CVertex() { }

public:
	Vec3 position{};
	Vec2 texCoord{};
	Vec3 normal{};
	Vec3 tangent{};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMesh : public CResource 
{
protected:
	//버텍스
	std::vector<CVertex> vertices{};
	std::shared_ptr<CVertexBuffer> mVertexBuffer{};

protected:
	//인덱스
	int subMeshNum = 1;
	int curSubSet{};

	std::vector<std::vector<UINT>> indices{};
	std::vector<std::shared_ptr<CIndexBuffer>> mIndexBuffers{};

	D3D12_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

public:
	BoundingOrientedBox oobb{};
	BoundingSphere oobs{};

public:
	CMesh() = default;
	CMesh(std::ifstream& inFile);
	virtual ~CMesh() = default;

public:
	void CreateVertexBuffer();
	void CreateIndexBuffers();

public:
	static std::shared_ptr<CMesh> CreateMeshFromFile(const std::string& name);
	static std::shared_ptr<CMesh> CreateMeshFromFBX(std::string& fileName);
	static std::shared_ptr<CMesh> CreateCubeMesh(Vec3 scale = {1.f,1.f,1.f});
	static std::shared_ptr<CMesh> CreateRectangleMesh(Vec2 scale = { 1.f,1.f });
	static std::shared_ptr<CMesh> CreateSphereMesh(float radius, UINT stackCnt, UINT sliceCnt);

protected:
	void CreateOOBBAndOOBS();
	virtual void CalculateNormal();

public:
	virtual void ReleaseUploadBuffer();

	int CheckRayIntersection(const Ray& ray, float& nearHitDistance);
public:
	virtual void Render(ID3D12GraphicsCommandList* cmdList);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, int idx);
	virtual void Render(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum, int idx);

public:
	const std::vector<CVertex>& GetVertices() const { return vertices; }
	const ComPtr<ID3D12Resource>& GetVertexBuffer() const { return mVertexBuffer->mBuffer; }
	UINT GetSubMeshNum() const { return subMeshNum; }
};
