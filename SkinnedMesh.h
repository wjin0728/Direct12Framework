#pragma once
#include"stdafx.h"
#include "Mesh.h"
#include "GameObject.h"
#include "UploadBuffer.h"

#define SKINNED_ANIMATION_BONES		128
#define BONES_PER_VERTEX			4

struct SkinnedVertex
{
	Vec4 BoneWeights{ 0, 0, 0, 0 };
	UINT BoneIndices[BONES_PER_VERTEX]{ 0, 0, 0, 0 };
};

class CSkinnedMesh : public CMesh
{
private:
	friend class CAnimationController;

	std::vector<std::shared_ptr<SkinnedVertex>> mSkinnedData{};

	std::shared_ptr<CVertexBuffer> mSkinnedVertexBuffer{};
	UINT mBoneNum = 0;                                // 뼈대 수

	std::vector<std::string> mBoneNames{};
	std::vector<std::shared_ptr<CGameObject>> mBoneFrameCaches{};

	std::vector<std::shared_ptr<Matrix>> mBindPoseBoneOffsets{};

	ComPtr<ID3D12Resource> mBindPoseBoneOffsetsBuffer;      // 뼈 오프셋 상수 버퍼

	//std::shared_ptr<CConstantBuffer> mBoneTransforms;   // 뼈 변환 상수 버퍼 -> CFrameResource
	//Matrix* mMappedBoneTransforms;
	int mBoneTransformIndex = -1;

public:
	CSkinnedMesh();
	~CSkinnedMesh();

public:
	void CreateSkinnedVertexBuffer();
	void PrepareSkinning(std::shared_ptr<CGameObject>& pModelRootObject);

public:
	static std::shared_ptr<CSkinnedMesh> CreateSkinnedMeshFromFile(std::ifstream& inFile);

	virtual void UpdateShaderVariables();

	virtual void Render(ID3D12GraphicsCommandList* cmdList);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, int idx);
	virtual void Render(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum, int idx);

	virtual void ReleaseUploadBuffers();

public:
	void SetBoneTransformIndex(int idx) { mBoneTransformIndex = idx; }

	int  GetBoneTransformIndex() const { return mBoneTransformIndex; }
	UINT GetBoneNum() const { return mBoneNum; }
};
