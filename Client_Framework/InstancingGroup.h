#pragma once
#include"FrameResource.h"

class CInstancingGroup
{
	friend class CInstancingManager;
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

	int Update(const std::shared_ptr<class CCamera>& camera, int startOffset);
};

