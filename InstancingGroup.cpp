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
	mType = type;

	std::vector<Vec3> vertex;
	vertex.push_back({ 0.f,0.f,0.f });

	mVertexBuffer = std::make_shared<CVertexBuffer>();
	mVertexBuffer->CreateBuffer(vertex, 0);
}

void CInstancingGroup::AddObject(std::shared_ptr<class CGameObject> object)
{
	auto itr = findByRawPointer(mObjects, object.get());
	if (itr == mObjects.end()) {
		mObjects.push_back(object);
	}
}

void CInstancingGroup::Render(const std::shared_ptr<class CCamera>& camera)
{
	int instancingCnt{};
	auto instancingBuffer = INSTANCINGBUFFER(mType);
	mInstancingBufferView = instancingBuffer->GetInstancingBufferView();

	switch (mType)
	{
	case INSTANCE_BUFFER_TYPE::BILLBOARD: {
		RESOURCE.Get<CShader>(L"Billboard")->SetPipelineState(CMDLIST);

		for (const auto& obj : mObjects) {
			//if (!camera->IsInFrustum(obj)) continue;

			BillboardData objDate;
			objDate.position = obj->GetTransform()->GetWorldPosition();
			objDate.size = { obj->GetTransform()->GetLocalScale().x, obj->GetTransform()->GetLocalScale().y };
			objDate.textureMat = obj->GetTransform()->GetTexMat().Transpose();

			instancingBuffer->UpdateBuffer(instancingCnt++, &objDate);
		}

		if (!mObjects.empty()) {
			CMDLIST->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

			D3D12_VERTEX_BUFFER_VIEW buffers[] = { mVertexBuffer->GetVertexBufferView(), mInstancingBufferView };
			CMDLIST->IASetVertexBuffers(0, 2, buffers);

			CMDLIST->DrawInstanced(1, instancingCnt, 0, 0);
		}
	}
		break;
	default:

		break;
	}
	
	
}
