#pragma once
#include"stdafx.h"
#include"Shader.h"
#include"Texture.h"
#include"FrameResource.h"

class CCamera;
class CGameObject;
class CTerrain;
class CShader;
class CLight;

class CSceneManager;

class CScene
{
protected:
	friend class CSceneManager;
	using ObjectList = std::vector<std::shared_ptr<CGameObject>>;
	using ObjectMap = std::unordered_map<UINT, std::shared_ptr<CGameObject>>;
	std::array<UINT, OBJECT_TYPE::end> mObjectCount{};

	ObjectList mObjects{};

	std::unordered_map<std::string, ObjectList> mRenderLayers{};
	std::vector<std::shared_ptr<class CInstancingGroup>> instancingGroups{};
	std::array<ObjectList, OBJECT_TYPE::end> mObjectTypes;

	std::shared_ptr<CTerrain> mTerrain{};
	std::array<std::vector<std::shared_ptr<CLight>>, (UINT)LIGHT_TYPE::END> mLights{};

	std::unordered_map<std::string, std::shared_ptr<CCamera>> mCameras;

	std::vector<int> renderTargetIndices{};
	int renderPasstype{};

	Matrix UIProjectionMatrix{};
  
public:
	CScene();
	virtual ~CScene() = default;

	virtual void Initialize() {};

	void Awake();
	void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual void RenderScene() {};
	void RenderShadowPass();
	void RenderForwardPass();
	void RenderGBufferPass();
	void RenderLightingPass();
	void RenderFinalPass();

public:
	void LoadSceneFromFile(const std::string& fileName);
	void CreatePrefabs(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs);

	std::shared_ptr<CGameObject> FindObjectWithTag(const std::string& tag);
	std::shared_ptr<CGameObject> FindObjectWithTag(const std::string& renderLayer, const std::string& tag);

	
	void AddObject(const std::string& renderLayer, std::shared_ptr<CGameObject> object);
	void AddObject(std::shared_ptr<CGameObject> object);
	void RemoveObject(std::shared_ptr<CGameObject> object);
	void SetTerrain(std::shared_ptr<CTerrain> terrain);

	void AddCamera(std::shared_ptr<CCamera> camera);
	void RemoveCamera(const std::string& tag);
	
	void AddLight(std::shared_ptr<CLight> light);
	
	const std::unordered_map<std::string, ObjectList>& GetObjects() const { return mRenderLayers; }
	ObjectList& GetObjects(const std::string& layer) { return mRenderLayers[layer]; }
	std::array<ObjectList, OBJECT_TYPE::end>& GetObjectsForType() { return mObjectTypes; }
	std::shared_ptr<CTerrain> GetTerrain() { return mTerrain; }
	std::shared_ptr<CCamera> GetCamera(const std::string& tag) { return mCameras[tag]; }

	std::vector <std::shared_ptr<CLight>> GetLight(LIGHT_TYPE type) { return mLights[(UINT)type]; }

protected:
	void RenderForLayer(const std::string& layer, std::shared_ptr<CCamera> camera, int pass = 0);
	void UpdatePassData();
};