#include "stdafx.h"
#include "SkinnedMeshRenderer.h"
#include "SkinnedMesh.h"
#include "Transform.h"
#include "Camera.h"
#include "ObjectPoolManager.h"
#include "ResourceManager.h"

CSkinnedMeshRenderer::CSkinnedMeshRenderer() : CRenderer()
{
    type = COMPONENT_TYPE::SKINNED_RENDERER;
}

CSkinnedMeshRenderer::~CSkinnedMeshRenderer()
{
}

void CSkinnedMeshRenderer::Awake()
{
	CRenderer::Awake();
    for (int i = 0; const auto & boneName : mBoneNames) {
		mBoneTransforms[i++] = owner->FindChildByName(boneName)->GetTransform();
	}
}

void CSkinnedMeshRenderer::Update()
{
}

void CSkinnedMeshRenderer::LateUpdate()
{
    mSkinnedMesh->oobs.Transform(mWorldBS, GetTransform()->GetWorldMat());

}

void CSkinnedMeshRenderer::Render(std::shared_ptr<CCamera> camera, int pass)
{
    if (!m_materials[0]->GetShader((PASS_TYPE)pass)) return;
    if (camera && !camera->IsInFrustum(mWorldBS)) return;

    CBObjectData objData;
    objData.worldMAt = GetTransform()->mWorldMat.Transpose();
    objData.invWorldMAt = objData.worldMAt.Invert();
    objData.textureMat = GetTransform()->mTextureMat.Transpose();

    CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::OBJECT)->UpdateBuffer(mCbvOffset, &objData);
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