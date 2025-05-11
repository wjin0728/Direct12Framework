#include "stdafx.h"
#include "SkinnedMesh.h"

CSkinnedMesh::CSkinnedMesh()
{
}

CSkinnedMesh::~CSkinnedMesh()
{
}

void CSkinnedMesh::CreateGPUResource()
{
	if (isLoaded) return;
	CMesh::CreateGPUResource();
	CreateSkinnedVertexBuffer();
	isLoaded = true;
}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	CMesh::ReleaseUploadBuffer();
	mSkinnedVertexBuffer->ReleaseUploadBuffer();
}

std::shared_ptr<CSkinnedMesh> CSkinnedMesh::CreateSkinnedMeshFromFile(const std::string& name)
{
	std::ifstream inFile{ MODEL_PATH(name), std::ios::binary };
	if (!inFile) {
		return nullptr;
	}

	std::shared_ptr<CSkinnedMesh> m = std::make_shared<CSkinnedMesh>();
	m->name = name;
	m->ReadMeshData(inFile);

	std::string token;

	while (true)
	{
		BinaryReader::ReadDateFromFile(inFile, token);

		if (token == "<BindPoses>:")
		{
			BinaryReader::ReadDateFromFile(inFile, m->mBoneNum);

			m->mBindPoseBoneOffsets.resize(m->mBoneNum);

			for (int i = 0; i < m->mBoneNum; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->mBindPoseBoneOffsets[i]);
			}
		}
		else if (token == "<BoneWeights>:")
		{
			int nVertices{};
			BinaryReader::ReadDateFromFile(inFile, nVertices);

			m->mSkinnedData.resize(nVertices);

			for (int i = 0; i < nVertices; i++) {
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneIndices[0]);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneWeights.x);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneIndices[1]);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneWeights.y);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneIndices[2]);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneWeights.z);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneIndices[3]);
				BinaryReader::ReadDateFromFile(inFile, m->mSkinnedData[i].BoneWeights.w);
			}
		}
		else if (token == "</SkinnedMesh>")
		{
			break;
		}
	}

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
