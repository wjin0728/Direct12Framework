#pragma once
#include"stdafx.h"
#include "Mesh.h"
#include "GameObject.h"
#include "UploadBuffer.h"

#define SKINNED_ANIMATION_BONES		300
#define BONES_PER_VERTEX			4

struct SkinnedVertex
{
	Vec4 BoneWeights{ 0, 0, 0, 0 };
	UINT BoneIndices[BONES_PER_VERTEX]{ 0, 0, 0, 0 };
};

class CSkinnedMesh : public CMesh
{
private:
	friend class CSkinnedMeshRenderer;

	std::vector<SkinnedVertex> mSkinnedData{};

	std::shared_ptr<CVertexBuffer> mSkinnedVertexBuffer{};
	UINT mBoneNum = 0;                                // »À´ë ¼ö

	std::vector<Matrix> mBindPoseBoneOffsets{};

public:
	CSkinnedMesh();
	virtual ~CSkinnedMesh();

public:
	void CreateSkinnedVertexBuffer();

public:
	static std::shared_ptr<CSkinnedMesh> CreateSkinnedMeshFromFile(const std::string& name);

	virtual void Render(ID3D12GraphicsCommandList* cmdList);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, int idx);
	virtual void Render(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum, int idx);

	virtual void CreateGPUResource() override;
	virtual void ReleaseUploadBuffers();

public:
	UINT GetBoneNum() const { return mBoneNum; }
	std::vector<Matrix>& GetBindPoseBoneOffsets() { return mBindPoseBoneOffsets; }
};
