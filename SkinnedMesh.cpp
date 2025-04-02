#include "stdafx.h"
#include "SkinnedMesh.h"

CSkinnedMesh::CSkinnedMesh()
{
}

CSkinnedMesh::~CSkinnedMesh()
{
}

void CSkinnedMesh::PrepareSkinning(std::shared_ptr<CGameObject>& modelRootObject)
{
	for (int i = 0; auto & cashe : mBoneFrameCaches) {
		cashe = modelRootObject->FindChildByName(mBoneNames[i++]);
	}
}

void CSkinnedMesh::UpdateShaderVariables()
{
	auto currFrameResource = INSTANCE(CDX12Manager).GetCurFrameResource();

	if (mBoneTransformIndex >= 0) {
		currFrameResource
			->GetBuffer((UINT)CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)
			->UpdateBuffer(mBoneTransformIndex);
	}
}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffer();
	mSkinnedVertexBuffer->ReleaseUploadBuffer();
}

std::shared_ptr<CSkinnedMesh> CSkinnedMesh::CreateSkinnedMeshFromFile(std::ifstream& inFile)
{
	std::shared_ptr<CSkinnedMesh> m = std::make_shared<CSkinnedMesh>();
	std::string token;

	while (true)
	{
		BinaryReader::ReadDateFromFile(inFile, token);

		if (token == "<BoneNames>:")
		{
			BinaryReader::ReadDateFromFile(inFile, m->mBoneNum);

			m->mBoneNames.resize(m->mBoneNum);
			m->mBoneFrameCaches.resize(m->mBoneNum);

			for (int i = 0; i < m->mBoneNum; i++) {
				std::string name;
				BinaryReader::ReadDateFromFile(inFile, name);
				m->mBoneNames[i] = BinaryReader::stringToWstring(name);
			}
		}
		else if (token == "<BoneOffsets>:")
		{
			BinaryReader::ReadDateFromFile(inFile, m->mBoneNum);

			m->mBindPoseBoneOffsets.resize(m->mBoneNum);

			for (int i = 0; i < m->mBoneNum; i++) {
				Matrix offset{};
				BinaryReader::ReadDateFromFile(inFile, offset);
				m->mBindPoseBoneOffsets[i] = std::make_shared<Matrix>(offset);
			}

			UINT elementBytes = (((sizeof(XMFLOAT4X4) * SKINNED_ANIMATION_BONES) + 255) & ~255);
			m->mBindPoseBoneOffsetsBuffer = CreateBufferResource(DEVICE, CMDLIST, m->mBindPoseBoneOffsets.data(), elementBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);
		}
		else if (token == "<BoneIndices>:")
		{
			int nVertices{};
			BinaryReader::ReadDateFromFile(inFile, nVertices);

			m->mSkinnedData.resize(nVertices);

			for (int i = 0; i < nVertices; i++) {
				m->mSkinnedData[i] = std::make_shared<SkinnedVertex>();

				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i]->BoneIndices);
			}
		}
		else if (token == "<BoneWeights>:")
		{
			int nVertices{};
			BinaryReader::ReadDateFromFile(inFile, nVertices);

			m->mSkinnedData.resize(nVertices);

			for (int i = 0; i < nVertices; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i]->BoneWeights);
			}
		}
		else if (token == "</SkinDeformations>")
		{
			break;
		}
	}
	m->CreateSkinnedVertexBuffer();

	return m;
}

void CSkinnedMesh::CreateSkinnedVertexBuffer() {
	mSkinnedVertexBuffer = std::make_shared<CVertexBuffer>();
	mSkinnedVertexBuffer->CreateBuffer(mSkinnedData);
}

void CSkinnedMesh::Render(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->IASetPrimitiveTopology(primitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW vbViews[2] = {
		mVertexBuffer->GetVertexBufferView(),
		mSkinnedVertexBuffer->GetVertexBufferView()
	};
	cmdList->IASetVertexBuffers(0, 2, vbViews);

	if (!indices.empty() && indices.size() > curSubSet) {
		mIndexBuffers[curSubSet]->SetIndexBuffer();
		cmdList->DrawIndexedInstanced(indices[curSubSet].size(), 1, 0, 0, 0);
	}
	else {
		cmdList->DrawInstanced(vertices.size(), 1, mVertexBuffer->mOffset, 0);
	}
}

void CSkinnedMesh::Render(ID3D12GraphicsCommandList* cmdList, int idx)
{
	cmdList->IASetPrimitiveTopology(primitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW vbViews[2] = { 
		mVertexBuffer->GetVertexBufferView(),
		mSkinnedVertexBuffer->GetVertexBufferView()
	};
	cmdList->IASetVertexBuffers(0, 2, vbViews);

	if (!indices.empty() && indices.size() > idx) {
		mIndexBuffers[idx]->SetIndexBuffer();
		cmdList->DrawIndexedInstanced(indices[idx].size(), 1, 0, 0, 0);
	}
	else {
		cmdList->DrawInstanced(vertices.size(), 1, mVertexBuffer->mOffset, 0);
	}
}

void CSkinnedMesh::Render(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum, int idx)
{
	CMDLIST->IASetPrimitiveTopology(primitiveTopology);

	D3D12_VERTEX_BUFFER_VIEW instanceData[3] = {
		mVertexBuffer->GetVertexBufferView(),
		mSkinnedVertexBuffer->GetVertexBufferView(),
		ibv
	};
	CMDLIST->IASetVertexBuffers(0, 3, instanceData);

	if (!indices.empty() && indices.size() > idx) {
		mIndexBuffers[idx]->SetIndexBuffer();
		CMDLIST->DrawIndexedInstanced(indices[idx].size(), instancingNum, 0, 0, 0);
	}
	else {
		CMDLIST->DrawInstanced(vertices.size(), instancingNum, mVertexBuffer->mOffset, 0);
	}
}
