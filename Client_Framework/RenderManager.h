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

class CRenderManager
{
	MAKE_SINGLETON(CRenderManager)

	friend class CRenderer;
	friend class CShadowManager;
private:
	enum class RenderPass
	{
		Opaque,
		Transparent,
		ShadowCast,
		GeometryPass,
		LightingPass,
		PostProcess,
		End
	};
	std::array<std::list<class CRenderer*>, (UINT)RENDER_LAYER::End> mRenderLayerLists{};

	std::unordered_map<InstanceKey, std::shared_ptr<class CInstancingGroup>> mInstancingGroupMap{};
	int mInstancingBufferOffset{ 0 };

	std::unordered_map<std::string, class CCamera*> mCameras;
	std::array<std::vector<class CLight*>, (UINT)LIGHT_TYPE::END> mLights{};

public:
	class CTerrain* mTerrain{ nullptr };
	void Initialize();
	void Destroy();

	
	void AddRenderer(class CRenderer* renderer, RENDER_LAYER layer = RENDER_LAYER::Opaque);
	void RenderLayer(int pass, RENDER_LAYER layer, CCamera* camera);
	void ClearRenderLayer(RENDER_LAYER layer = RENDER_LAYER::Opaque)
	{
		mRenderLayerLists[(UINT)layer].clear();
	}
	void ClearAllRenderLayers()
	{
		for (auto& layerList : mRenderLayerLists) {
			layerList.clear();
		}
	}

	void AddCamera(const std::string& name, CCamera* camera)
	{
		mCameras[name] = camera;
	}
	void RemoveCamera(CCamera* camera)
	{
		for (auto itr = mCameras.begin(); itr != mCameras.end(); ++itr) {
			if (itr->second == camera) {
				mCameras.erase(itr);
				break;
			}
		}
	}
	CCamera* GetCamera(const std::string& name)
	{
		auto itr = mCameras.find(name);
		if (itr != mCameras.end()) {
			return itr->second;
		}
		return nullptr;
	}
	CCamera* GetMainCamera()
	{
		if (!mCameras.empty()) {
			return mCameras["MainCamera"];
		}
		return nullptr;
	}

	void AddLight(class CLight* light);
	void RemoveLight(class CLight* light);

public:
	void AddInstancingGroup(const InstanceKey& key, std::shared_ptr<class CInstancingGroup> group);

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
	void UpdateInstancingGroup();

public:
	void RenderShadowPass();
	void RenderForwardPass();
	void RenderGBufferPass();
	void RenderLightingPass();
	void RenderFinalPass();
	void RenderInstancingGroup(int pass = 0);
};

