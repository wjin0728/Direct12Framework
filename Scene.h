#pragma once
#include"stdafx.h"
#include"Shader.h"
#include"LightManager.h"
#include"Texture.h"
#include"FrameResource.h"

class CCamera;
class CGameObject;
class CTerrain;
class CShader;

class CSceneManager;

class CScene
{
protected:
	friend class CSceneManager;
	using ObjectList = std::vector<std::shared_ptr<CGameObject>>;

	ObjectList mObjects{};

	std::unordered_map<std::wstring, std::shared_ptr<CShader>> mShaders{};
	std::unordered_map<std::wstring, ObjectList> mRenderLayers{};
	std::vector<std::shared_ptr<class CInstancingGroup>> instancingGroups{};

	std::shared_ptr<CTerrain> mTerrain{};
	std::unique_ptr<CLightManager> lightMgr{};

	std::unordered_map<std::wstring, std::shared_ptr<CCamera>> mCameras;
		 
public:
	CScene();
	virtual ~CScene() = default;

	virtual void Initialize() {};

	void Awake();
	void Start();

	virtual void Update();
	virtual void LateUpdate();

	void RenderShadowPass();
	void RenderForwardPass();
	void RenderGBufferPass();
	void RenderLightingPass();
	void RenderFinalPass();

public:
	std::shared_ptr<CGameObject> FindObjectWithTag(const std::wstring& tag);
	std::shared_ptr<CGameObject> FindObjectWithTag(const std::wstring& renderLayer, const std::wstring& tag);

	void AddObject(const std::wstring& renderLayer, std::shared_ptr<CGameObject> object);
	void AddObject(std::shared_ptr<CGameObject> object);
	void RemoveObject(std::shared_ptr<CGameObject> object);

	void AddCamera(std::shared_ptr<CCamera> camera);
	void RemoveCamera(const std::wstring& tag);
	
	const std::unordered_map<std::wstring, ObjectList>& GetObjects() const { return mRenderLayers; }
	ObjectList& GetObjects(const std::wstring& layer) { return mRenderLayers[layer]; }
	std::shared_ptr<CTerrain> GetTerrain() { return mTerrain; }

protected:
	void RenderForLayer(const std::wstring& layer, bool frustumCulling = true);
	void RenderTerrain(const std::wstring& layer);
	void UpdatePassData();
};