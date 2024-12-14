#pragma once
#include"FrameResource.h"

class CInstancingGroup
{
private:
	std::vector<std::shared_ptr<class CGameObject>> mObjects{};
	D3D12_VERTEX_BUFFER_VIEW mInstancingBufferView{};

	std::shared_ptr<class CVertexBuffer> mVertexBuffer{};

	INSTANCE_BUFFER_TYPE mType;

public:
	CInstancingGroup() = default;
	~CInstancingGroup();

public:
	void Initialize(INSTANCE_BUFFER_TYPE type);
	void AddObject(std::shared_ptr<class CGameObject> object);

	void Render(const std::shared_ptr<class CCamera>& camera);
};

