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
	friend class CSceneManager;
protected:
	class CRenderManager* mRenderMgr{};

	using ObjectList = std::vector<std::shared_ptr<CGameObject>>;

	std::queue<std::shared_ptr<CGameObject>> mAddQueue{};
	std::queue<std::shared_ptr<CGameObject>> mRemoveQueue{};

	ObjectList mObjects{};
	std::array<ObjectList, OBJECT_TYPE::end> mObjectTypes;
	std::shared_ptr<CTerrain> mTerrain{};

	std::vector<int> renderTargetIndices{};
	int renderPasstype{};

	Matrix UIProjectionMatrix{};
	float finalTargetAlpha{ 1.f };

public:
	BoundingBox mSceneAABB{};
	bool mIsActive{ false };
	CScene();
	virtual ~CScene() = default;

	virtual void Initialize() {};

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual void RenderScene() {};

public:
	void LoadSceneFromFile(const std::string& fileName);
	void CreatePrefabs(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs);

	std::shared_ptr<CGameObject> FindObjectWithTag(const std::string& tag);

	void ExpandSceneAABB(std::shared_ptr<CGameObject> obj, BoundingBox& sceneAABB);

	void AddObjectImmediately(std::shared_ptr<CGameObject> object);
	void AddObject(std::shared_ptr<CGameObject> object);
	void RemoveObject(std::shared_ptr<CGameObject> object);
	void SetTerrain(std::shared_ptr<CTerrain> terrain);
	
	ObjectList& GetObjectsWithType(OBJECT_TYPE type) { return mObjectTypes[(UINT)type]; }
	std::array<ObjectList, OBJECT_TYPE::end>& GetObjectsForType() { return mObjectTypes; }
	std::shared_ptr<CTerrain> GetTerrain() { return mTerrain; }

	void CollectVisibleObjects();
	void AddRemoveQueue(std::shared_ptr<CGameObject> object);

	void CommitObjectChanges();

protected:
	void UpdatePassData();
	void RemoveObjects();
};