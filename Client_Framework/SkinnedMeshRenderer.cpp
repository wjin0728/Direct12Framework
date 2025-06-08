#include "stdafx.h"
#include "SkinnedMeshRenderer.h"
#include "SkinnedMesh.h"
#include "Transform.h"
#include "Camera.h"
#include "ObjectPoolManager.h"
#include "ResourceManager.h"

CSkinnedMeshRenderer::CSkinnedMeshRenderer() : CRenderer()
{
}

CSkinnedMeshRenderer::~CSkinnedMeshRenderer()
{
}

void CSkinnedMeshRenderer::Awake()
{
	CRenderer::Awake();
    owner->mRootBS = owner->mRootLocalBS = mWorldBS;
	
}

void CSkinnedMeshRenderer::Start()
{
	CRenderer::Start();
    mRootBone = owner->GetTransform()->GetRoot();

	for (auto& mat : m_materials) {
        auto& shaderName = mat->mShaderName;
        if (shaderName.contains("Animation")) continue;
		mat->SetShader(shaderName + "Animation");
	}
}

void CSkinnedMeshRenderer::Update()
{
}

void CSkinnedMeshRenderer::LateUpdate()
{
    if (mCbvOffset == -1) return;

    if(owner->GetName() == "weapon_archerbow_001" || owner->GetName() == "weapon")
		int a = 0; // Debugging breakpoint
	auto rootTransform = mRootBone.lock();
    mSkinnedMesh->oobs.Transform(mWorldBS, rootTransform->GetWorldMat(false));
	mSkinnedMesh->oobb.Transform(mWorldOOBB, rootTransform->GetWorldMat(false));

    CBObjectData objData;
    objData.worldMAt = rootTransform->mWorldMat.Transpose();
    objData.invWorldMAt = rootTransform->mWorldMat.Invert();
    objData.textureMat = rootTransform->mTextureMat.Transpose();
	objData.hitFactor = rootTransform->mHitFactor;

    CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->UpdateBuffer(mCbvOffset, &objData);
}

void CSkinnedMeshRenderer::Render(std::shared_ptr<CCamera> camera, int pass)
{
    if (!m_materials[0]->GetShader((PASS_TYPE)pass)) return;
    //if (camera && !camera->IsInFrustum(mWorldBS, pass)) return;

    CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->BindToShader(mCbvOffset);

    int subMeshNum = mSkinnedMesh->GetSubMeshNum();
    for (int i = 0; i < subMeshNum; i++) {
        m_materials[i]->BindShader((PASS_TYPE)pass);
        m_materials[i]->BindDataToShader();
        mSkinnedMesh->RenderSkinning(CMDLIST, i);
    }
}

void CSkinnedMeshRenderer::SetSkinnedMesh(const std::shared_ptr<CSkinnedMesh>& mesh)
{
    mSkinnedMesh = mesh;
    if (mSkinnedMesh) mWorldBS = mSkinnedMesh->oobs;
}

void CSkinnedMeshRenderer::SetSkinnedMesh(const std::string& name)
{
	mSkinnedMesh = INSTANCE(CResourceManager).Get<CSkinnedMesh>(name);
    if (mSkinnedMesh) mWorldBS = mSkinnedMesh->oobs;
}
