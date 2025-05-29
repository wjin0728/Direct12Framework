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
    auto root = owner->GetTransform()->GetRoot()->owner;

    for (int i = 0; const auto & boneName : mBoneNames) {
        mBoneTransforms.push_back(root->FindChildByName(boneName)->GetTransform());
    }
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
    mSkinnedMesh->oobs.Transform(mWorldBS, GetTransform()->GetWorldMat());
	mSkinnedMesh->oobb.Transform(mWorldOOBB, GetTransform()->GetWorldMat());

    if (mCbvOffset == -1) return;
    CBObjectData objData;
    objData.worldMAt = GetTransform()->mWorldMat.Transpose();
    objData.invWorldMAt = GetTransform()->mWorldMat.Invert();
    objData.textureMat = GetTransform()->mTextureMat.Transpose();

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
        mSkinnedMesh->Render(CMDLIST, i);
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

void CSkinnedMeshRenderer::AddBone(const std::shared_ptr<CTransform>& bone)
{
    mBoneTransforms.push_back(bone);
}
