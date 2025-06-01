#pragma once
#include "Renderer.h"

class CSkinnedMesh;
class CTransform;

class CSkinnedMeshRenderer : public CRenderer
{
private:
	friend class CAnimationController;
	friend class CGameObject;

    std::shared_ptr<CSkinnedMesh> mSkinnedMesh{};
	std::vector<std::string> mBoneNames{};
	std::weak_ptr<CTransform> mRootBone{};


public:
    CSkinnedMeshRenderer();
    CSkinnedMeshRenderer(const CSkinnedMeshRenderer& other) : mSkinnedMesh(other.mSkinnedMesh), mBoneNames(other.mBoneNames), CRenderer(other) {}
    ~CSkinnedMeshRenderer();

    virtual void Awake() override;
	virtual void Start() override;
    virtual void Update() override;
    virtual void LateUpdate() override;

    virtual void Render(std::shared_ptr<CCamera> camera, int pass = 0) override;

    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CSkinnedMeshRenderer>(*this); }

public:
    void SetSkinnedMesh(const std::shared_ptr<CSkinnedMesh>& mesh);
    void SetSkinnedMesh(const std::string& name);
    std::shared_ptr<CSkinnedMesh> GetSkinnedMesh() const { return mSkinnedMesh; }
};

