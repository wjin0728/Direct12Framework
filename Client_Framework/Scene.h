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

enum class FadeState
{
	In,
	Out,
	None
};

enum class FadeType
{
	Default,
	Circular, 
	End
};

class CScene
{
	friend class CSceneManager;
protected:
	class CRenderManager* mRenderMgr{};

	using ObjectList = std::vector<std::shared_ptr<CGameObject>>;

	std::queue<std::shared_ptr<CGameObject>> mAddQueue{};
	std::queue<std::shared_ptr<CGameObject>> mRemoveQueue{};
	std::queue<class CComponent*> mComponentStartQueue{};

	ObjectList mObjects{};
	std::array<ObjectList, OBJECT_TYPE::end> mObjectTypes;
	std::shared_ptr<CTerrain> mTerrain{};

	std::vector<int> renderTargetIndices{};
	int renderPasstype{};

	Matrix UIProjectionMatrix{};

	FadeState mFadeState{ FadeState::None };
	FadeType mFadeType{ FadeType::Default };

	Color mFadeColor{ 0.0f, 0.0f, 0.0f, 0.0f };
	float mFadeTime{ 0.0f };
	float mFadeDuration{ 0.5f };
	float mFadeRadius{ 0.0f };
	std::function<void()> mOnFadeFinish;


public:
	BoundingBox mSceneAABB{};
	bool mIsActive{ false };
	CScene();
	virtual ~CScene() = default;

	virtual void Initialize() {};

	virtual void Activate();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual void RenderScene() {};

	void RenderFadeOverlay();

public:
	void LoadSceneFromFile(const std::string& fileName);
	void CreatePrefabs(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs);

	std::shared_ptr<CGameObject> FindObjectWithTag(const std::string& tag);
	std::shared_ptr<CGameObject> FindObjectWithName(const std::string& name);

	void ExpandSceneAABB(std::shared_ptr<CGameObject> obj, BoundingBox& sceneAABB);

	void AddObjectImmediately(std::shared_ptr<CGameObject> object, bool activate = false);
	void AddObject(std::shared_ptr<CGameObject> object);
	void RemoveObject(std::shared_ptr<CGameObject> object);
	void SetTerrain(std::shared_ptr<CTerrain> terrain);
	
	ObjectList& GetObjectsWithType(OBJECT_TYPE type) { return mObjectTypes[(UINT)type]; }
	std::array<ObjectList, OBJECT_TYPE::end>& GetObjectsForType() { return mObjectTypes; }
	std::shared_ptr<CTerrain> GetTerrain() { return mTerrain; }

	void CollectVisibleObjects();
	void AddRemoveQueue(std::shared_ptr<CGameObject> object);
	void AddComponentToStartQueue(CComponent* component);

	void CommitObjectChanges();

	void FadeUpdate();
	void FadeIn(float duration = 0.5f, const Color& color = Color(0.0f, 0.0f, 0.0f, 1.0f), std::function<void()> onFinish = nullptr);
	void FadeOut(float duration = 0.5f, const Color& color = Color(0.0f, 0.0f, 0.0f, 1.0f), std::function<void()> onFinish = nullptr);

	void CircularFadeIn(float duration = 0.5f, const Color& color = Color(0.0f, 0.0f, 0.0f, 1.0f), std::function<void()> onFinish = nullptr);
	void CircularFadeOut(float duration = 0.5f, const Color& color = Color(0.0f, 0.0f, 0.0f, 1.0f), std::function<void()> onFinish = nullptr);

protected:
	void UpdatePassData();
	void RemoveObjects();
};