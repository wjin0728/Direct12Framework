#include "stdafx.h"
#include "InstancingGroup.h"
#include"InstancingBuffer.h"
#include"GameObject.h"
#include"MeshRenderer.h"
#include"Transform.h"
#include"DX12Manager.h"
#include"Camera.h"
#include"VertexBuffer.h"
#include"ResourceManager.h"
#include"Shader.h"

CInstancingGroup::~CInstancingGroup()
{
}

void CInstancingGroup::Initialize(INSTANCE_BUFFER_TYPE type)
{

}

void CInstancingGroup::AddObject(std::shared_ptr<class CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr == mObjects.end()) {
		mObjects.push_back(object);
	}
}

int CInstancingGroup::Update(const std::shared_ptr<class CCamera>& camera, int startOffset)
{
	mInstancingBufferOffset = startOffset;
	mInstancingCnt = 0;
	auto instancingBuffer = INSTANCE(CDX12Manager).GetInstancingBuffer((UINT)mType);
	switch (mType)
	{
	case INSTANCE_BUFFER_TYPE::BILLBOARD: {
		for (const auto& obj : mObjects) {
			//if (!camera->IsInFrustum(obj)) continue;

			BillboardData objDate;
			objDate.position = obj->GetTransform()->GetWorldPosition();
			objDate.size = { obj->GetTransform()->GetLocalScale().x, obj->GetTransform()->GetLocalScale().y };
			objDate.textureMat = obj->GetTransform()->GetTexMat().Transpose();

			instancingBuffer->UpdateBuffer(mInstancingBufferOffset + (mInstancingCnt++), &objDate);
		}
		break;
	}
	case INSTANCE_BUFFER_TYPE::OBJECT: {
		for (const auto& obj : mObjects) {
			if(obj->GetName() == "SM_Env_Rock_Small_01")
				int a = 0; // Debugging purpose, remove if not needed
			BoundingSphere objBS = obj->GetRootBoundingSphere();
			if (!camera->IsInFrustum(objBS, 0)) continue;

			IBObjectData objDate;
			objDate.worldMAt = obj->GetTransform()->GetWorldMat().Transpose();
			objDate.invWorldMAt = objDate.worldMAt.Invert();

			instancingBuffer->UpdateBuffer(mInstancingBufferOffset + (mInstancingCnt++), &objDate);
		}
		break;
	}
	default:
		break;
	}

	return mInstancingBufferOffset + mInstancingCnt;
}

