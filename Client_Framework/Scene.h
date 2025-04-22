#pragma once
#include "stdafx.h"
#include "Shader.h"
#include "LightManager.h"
#include "Texture.h"
#include "FrameResource.h"

class CCamera;
class CGameObject;
class CTerrain;
class CShader;
class CTransform;

class CSceneManager;

class CScene
{
protected:
	friend class CSceneManager;
	using ObjectList = std::vector<std::shared_ptr<CGameObject>>;

	ObjectList mObjects{};

	std::unordered_map<std::string, std::shared_ptr<CShader>> mShaders{};
	std::unordered_map<std::string, ObjectList> mRenderLayers{};
	std::vector<std::shared_ptr<class CInstancingGroup>> instancingGroups{};
	std::array<ObjectList, OBJECT_TYPE::end> mObjectTypes;

	std::shared_ptr<CTerrain> mTerrain{};
	std::unique_ptr<CLightManager> lightMgr{};

	std::unordered_map<std::string, std::shared_ptr<CCamera>> mCameras;
	int clientID{ -1 };
  
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
	void LoadSceneFromFile(const std::string& fileName);
	void CreatePrefabs(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs);

	std::shared_ptr<CGameObject> FindObjectWithTag(const std::string& tag);
	std::shared_ptr<CGameObject> FindObjectWithTag(const std::string& renderLayer, const std::string& tag);

	void AddObject(const std::string& renderLayer, std::shared_ptr<CGameObject> object);
	void AddObject(std::shared_ptr<CGameObject> object);
	void RemoveObject(std::shared_ptr<CGameObject> object);
	void SetTerrain(std::shared_ptr<CTerrain> terrain);
	void SetClientID(int id) { clientID = id; }

	void AddCamera(std::shared_ptr<CCamera> camera);
	void RemoveCamera(const std::string& tag);

	std::shared_ptr<CGameObject> CreatePlayer(PLAYER_CLASS playerClass);
	std::shared_ptr<CGameObject> CreatePlayerCamera(std::shared_ptr<CGameObject> player);
	
	const std::unordered_map<std::string, ObjectList>& GetObjects() const { return mRenderLayers; }
	ObjectList& GetObjects(const std::string& layer) { return mRenderLayers[layer]; }
	std::array<ObjectList, OBJECT_TYPE::end>& GetObjectsForType() { return mObjectTypes; }
	std::shared_ptr<CTerrain> GetTerrain() { return mTerrain; }
	std::shared_ptr<CGameObject> GetPlayer(int idx) { return mObjectTypes[PLAYER][idx]; }
	int GetClientID() { return clientID; }

	// std::shared_ptr<CGameObject> GetPlayer(int id) { return mPlayers[id]; }
	// void SetPlayer(std::shared_ptr<CGameObject> mPlayer, Vec3 position) { mPlayer->GetTransform()->SetLocalPosition(position); };

protected:
	void RenderForLayer(const std::string& layer, std::shared_ptr<CCamera> camera, int pass = 0);
	void RenderTerrain(const std::string& layer);
	void UpdatePassData();
};