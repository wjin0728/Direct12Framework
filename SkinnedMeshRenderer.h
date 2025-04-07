#pragma once
#include "Renderer.h"

class CSkinnedMesh;
class CTransform;

class CSkinnedMeshRenderer : public CRenderer
{
private:
	friend class CAnimationController;

	std::shared_ptr<CSkinnedMesh> mSkinnedMesh;
	std::vector<std::shared_ptr<Matrix>> mBoneTransforms{};

	BoundingSphere mWorldBS{};
	int mCbvIdx = -1;
	UINT mCbvOffset{};

public:
    CSkinnedMeshRenderer();
    CSkinnedMeshRenderer(const CSkinnedMeshRenderer& other) : mSkinnedMesh(other.mSkinnedMesh), mBoneTransforms(other.mBoneTransforms), CRenderer(other) {}
    ~CSkinnedMeshRenderer();

    virtual void Awake() override;
    virtual void Update() override;
    virtual void LateUpdate() override;

    virtual void Render(std::shared_ptr<CCamera> camera, int pass = 0) override;

    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CSkinnedMeshRenderer>(*this); }

public:
    void SetSkinnedMesh(const std::shared_ptr<CSkinnedMesh>& mesh);
    void AddBone(const std::shared_ptr<Matrix>& bone);
    std::shared_ptr<CSkinnedMesh> GetSkinnedMesh() const { return mSkinnedMesh; }
private:
    void SetCBVIndex();
    void ReturnCBVIndex();
};

