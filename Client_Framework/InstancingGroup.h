#pragma once
#include"FrameResource.h"

class CInstancingGroup
{
	friend class CRenderManager;
private:
	INSTANCE_BUFFER_TYPE mType{};
	std::vector<std::shared_ptr<class CGameObject>> mObjects{};
	UINT mMaxInstanceNum{};
	UINT mInstancingCnt{};
	UINT mInstancingBufferOffset{};

public:
	CInstancingGroup() = default;
	~CInstancingGroup();

public:
	void Initialize(INSTANCE_BUFFER_TYPE type);
	void AddObject(std::shared_ptr<class CGameObject> object);

	int Update(class CCamera* camera, int startOffset);
};

