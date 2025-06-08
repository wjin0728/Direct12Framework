#pragma once

class CMonoBehaviour;
class CRenderer;
class CTransform;
class CCamera;
class CTerrain;
class CCollider;
class CComponent;
class CAnimationController;
class CSkinnedMesh;
class CPlayerController;
class CPlayerStateMachine;
class CEntityState;

class CGameObject : public std::enable_shared_from_this<CGameObject>
{
	friend CComponent;
	friend CTransform;

private:
	std::vector<std::shared_ptr<CComponent>> mComponents{};

	std::shared_ptr<CTransform> mTransform{};
	std::shared_ptr<CRenderer> mRenderer{};
	std::shared_ptr<CCollider> mCollider{};
	std::shared_ptr<CPlayerController> mPlayerController{};
	std::shared_ptr<CEntityState> mStateMachine{};

	std::vector<std::shared_ptr<CGameObject>> mChildren{};

private:
	bool mActive = true;
	bool mIsStatic{ false };
	bool mIsInstancing{ false };
	bool misAwake{ false };

	std::string mName{};
	std::string mTag{};
	std::string mRenderLayer{};
	OBJECT_TYPE mObjectType{ NONE };

	int mID{ -1 };

public:
	BoundingSphere mRootLocalBS = BoundingSphere();
	BoundingSphere mRootBS = BoundingSphere();


public:
	CGameObject(bool makeTransform = true);
	virtual ~CGameObject();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	void Render(std::shared_ptr<CCamera> camera, int pass = 0);

public:
	//������Ʈ�� ���纻�� �����Ѵ�.
	static std::shared_ptr<CGameObject> Instantiate(const std::shared_ptr<CGameObject>& original,
		const std::shared_ptr<CTransform>& parentTransform = nullptr);
	//������Ʈ�� ���纻�� �����Ѵ�.
	static std::shared_ptr<CGameObject> Instantiate(const std::unique_ptr<CGameObject>& original,
		const std::shared_ptr<CTransform>& parentTransform = nullptr);

	//�⺻ ������ ���� ī�޶� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CGameObject> CreateCameraObject(const std::string& tag, Vec2 rtSize,
		float nearPlane = 1.01f, float farPlane = 1000.f, float fovAngle = 60.f);
	static std::shared_ptr<CGameObject> CreateCameraObject(const std::string& tag, Vec2 rtSize, float nearPlane, float farPlane, Vec2 size);
	static std::shared_ptr<CGameObject> CreateUIObject(const std::string& shader, const std::string& texture, Vec2 pos, Vec2 size, float depth = 1.f);
	//�⺻ ������ ���� ���� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CGameObject> CreateTerrainObject(std::ifstream& ifs);
	//���̳ʸ� ������ ���� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CGameObject> CreateObjectFromFile(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs);
	static std::shared_ptr<CGameObject> CreateObjectFromFile(const std::string& name);

	std::shared_ptr<CTransform> GetTransform() { return mTransform; }
	std::shared_ptr<CRenderer> GetRenderer() { return mRenderer; }
	std::shared_ptr<CCollider> GetCollider() { return mCollider; }
	std::shared_ptr<CAnimationController> GetAnimationController() { return mAnimationController; }
	std::shared_ptr<CPlayerController> GetPlayerController() { return mPlayerController; }
	std::shared_ptr<CEntityState> GetStateMachine() { return mStateMachine; }

	std::shared_ptr<CGameObject> GetSptrFromThis();
	const std::string& GetName() const { return mName; }
	const std::string& GetTag() const { return mTag; }
	const std::string& GetIDString() const { return std::to_string(mID); }
	const int GetIDInt() const { return mID; }
	const std::string& GetRenderLayer() const { return mRenderLayer; }
	bool GetActive() const { return mActive; }
	bool GetStatic() const { return mIsStatic; }
	bool GetInstancing() const { return mIsInstancing; }
	OBJECT_TYPE GetObjectType() const { return mObjectType; }
	std::vector<std::shared_ptr<CGameObject>>& GetChildren() { return mChildren; }
	BoundingSphere GetRootBoundingSphere() const { return mRootBS; }


	void SetActive(bool active) { mActive = active; }
	void SetStatic(bool isStatic);
	void SetInstancing(bool isInstancing);
	void SetObjectType(OBJECT_TYPE type) { mObjectType = type; }
	void SetRenderLayer(const std::string& layer) { mRenderLayer = layer; }
	void SetName(const std::string& name) { mName = name; }
	void SetTag(const std::string& tag) { mTag = tag; }
	void SetID(int id) { mID = id; }
	void SetParent(const std::shared_ptr<CGameObject>& parent);
	void SetPlayerController(const std::shared_ptr<CPlayerController>& playerController) { mPlayerController = playerController; }
	void SetStateMachine(const std::shared_ptr<CEntityState>& stateMachine) { mStateMachine = stateMachine; }
	void SetTransform(const std::shared_ptr<CTransform>& transform) { mTransform = transform; }
	void SetRenderer(const std::shared_ptr<CRenderer>& renderer) { mRenderer = renderer; }

	void ReturnCBVIndex();

	std::shared_ptr<CGameObject> FindChildByName(const std::string& name);

	void AddChild(std::shared_ptr<CGameObject> child);
	void RemoveChild(std::shared_ptr<CGameObject> child);

	template<typename T, typename... Args>
	std::shared_ptr<T> AddComponent(Args&&... args);
	template<typename T>
	std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component);
	template<typename T>
	std::shared_ptr<T> GetComponent();
	template<typename T>
	std::shared_ptr<T> GetComponentFromHierarchy();

	template<typename T>
	void RemoveComponent();

private:

	static std::shared_ptr<CGameObject> InitFromFile(std::ifstream& inFile, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs);
	void InitByObjectName();
	void CreateTransformFromFile(std::ifstream& inFile);
	void CreateRendererFromFile(std::ifstream& inFile);
	void CreateTerrainFromFile(std::ifstream& inFile);
	void CreateLightFromFile(std::ifstream& inFile);
	void CreateAnimationFromFile(const std::string& fileName);
	void CreateUIrendererFromFile(std::ifstream& inFile);

public:
	std::shared_ptr<CAnimationController> mAnimationController{};

	void ResetForAnimationBlending();
	void CacheFrameHierarchies(std::vector<std::shared_ptr<CGameObject>>& boneFrameCaches);

	void PrepareSkinning();

	void UpdateWorldMatrices(std::shared_ptr<CTransform> parent);
	void PrintSRT();
};



template<typename T, typename ...Args>
inline std::shared_ptr<T> CGameObject::AddComponent(Args&&... args)
{
	std::shared_ptr<T> result{};
	for (auto& component : mComponents) {
		if (result = std::dynamic_pointer_cast<T>(component)) {
			return result;
		}
	}

	result = std::make_shared<T>(args...);
	result->SetOwner(this);
	mComponents.push_back(result);

	return result;
}

template<typename T>
inline std::shared_ptr<T> CGameObject::AddComponent(const std::shared_ptr<T>& component)
{
	std::shared_ptr<T> result{};
	for (auto& comp : mComponents) {
		if (result = std::dynamic_pointer_cast<T>(comp)) {
			return result;
		}
	}

	mComponents.push_back(component);
	component->SetOwner(this);
	return component;
}

template<typename T>
inline std::shared_ptr<T> CGameObject::GetComponent()
{
	std::shared_ptr<T> result{};
	for (auto& component : mComponents) {
		if (result = std::dynamic_pointer_cast<T>(component)) {
			return result;
		}
	}

	return nullptr;
}

template<typename T>
inline std::shared_ptr<T> CGameObject::GetComponentFromHierarchy()
{
	std::shared_ptr<T> result = GetComponent<T>();
	if (result) {
		return result;
	}
	for (auto& child : mChildren) {
		result = child->GetComponentFromHierarchy<T>();
		if (result) {
			return result;
		}
	}
	return nullptr;
}

template<typename T>
inline void CGameObject::RemoveComponent()
{
	std::shared_ptr<T> result{};
	for (auto& component : mComponents) {
		if (result = std::dynamic_pointer_cast<T>(component)) {
			component->SetOwner(nullptr);
			mComponents.erase(std::remove(mComponents.begin(), mComponents.end(), component), mComponents.end());
			return;
		}
	}
}
