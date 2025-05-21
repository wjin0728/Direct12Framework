#pragma once
#include"stdafx.h"

struct InstanceKey
{
	class CMesh* mesh{ nullptr };
	class CMaterial* material{ nullptr };

	bool operator==(const InstanceKey& other) const
	{
		return mesh == other.mesh && material == other.material;
	}
};

namespace std {
	template <>
	struct hash<InstanceKey>
	{
		std::size_t operator()(const InstanceKey& key) const
		{
			return std::hash<void*>()(key.mesh) ^ (std::hash<void*>()(key.material) << 1);
		}
	};
}

class CInstancingManager
{
	MAKE_SINGLETON(CInstancingManager)

private:
	std::unordered_map<InstanceKey, std::shared_ptr<class CInstancingGroup>> mInstancingGroupMap{};

	int mInstancingBufferOffset{ 0 };

public:
	void Initialize();
	void Destroy();

	void AddInstancingGroup(const InstanceKey& key, std::shared_ptr<class CInstancingGroup> group)
	{
		mInstancingGroupMap[key] = group;
	}

	void AddInstancingObject(const InstanceKey& key, std::shared_ptr<class CGameObject> object);
	void AddInstancingObject(std::shared_ptr<class CGameObject> object);

	void RemoveInstancingGroup(const InstanceKey& key)
	{
		mInstancingGroupMap.erase(key);
	}

	std::shared_ptr<class CInstancingGroup> FindInstancingGroup(const InstanceKey& key)
	{
		auto itr = mInstancingGroupMap.find(key);
		if (itr != mInstancingGroupMap.end()) {
			return itr->second;
		}
		return nullptr;
	}

	std::shared_ptr<class CInstancingGroup> FindInstancingGroup(class CMesh* mesh, class CMaterial* material)
	{
		InstanceKey key{ mesh, material };
		return FindInstancingGroup(key);
	}

	void UpdateInstancingGroup(const std::shared_ptr<class CCamera>& camera);
	void RenderInstancingGroup(int pass = 0);
};

