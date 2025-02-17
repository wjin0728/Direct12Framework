#pragma once

enum class LAYER_TYPE : UINT8
{
	PLAYER,
	ENEMY,
	UI,
	TERRAIN,
};

class CMonoBehaviour;
class CMeshRenderer;
class CTransform;
class CCamera;
class CTerrain;
class CCollider;
class CComponent;


class CGameObject : public std::enable_shared_from_this<CGameObject>
{
	friend CComponent;
	friend CTransform;

private:
	std::vector<std::shared_ptr<CComponent>> mComponents{};

	std::shared_ptr<CTransform> mTransform{};
	std::shared_ptr<CMeshRenderer> mMeshRenderer{};
	std::shared_ptr<CCollider> mCollider{};

	std::vector<std::shared_ptr<CGameObject>> mChildren{};

private:
	bool mActive = true;
	bool mIsStatic{false};
	bool mIsInstancing{false};

	std::wstring mName{};
	std::wstring mTag{};
	std::wstring mRenderLayer{};
	LAYER_TYPE mLayerType{};

private:
	BoundingSphere mRootLocalBS = BoundingSphere();
	BoundingSphere mRootBS = BoundingSphere();

public:
	CGameObject(bool makeTransform = true);
	virtual ~CGameObject();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	void Render();

public:
	//오브젝트의 복사본을 생성한다.
	static std::shared_ptr<CGameObject> Instantiate(const std::shared_ptr<CGameObject>& original, 
		const std::shared_ptr<CTransform>& parentTransform = nullptr);
	//오브젝트의 복사본을 생성한다.
	static std::shared_ptr<CGameObject> Instantiate(const std::unique_ptr<CGameObject>& original,
		const std::shared_ptr<CTransform>& parentTransform = nullptr);

	//기본 설정을 가진 카메라 오브젝트를 생성한다.
	static std::shared_ptr<CGameObject> CreateCameraObject(const std::wstring& tag, Vec2 rtSize, 
		float nearPlane = 1.01f, float farPlane = 1000.f, float fovAngle = 60.f);
	//기본 설정을 가진 렌더 오브젝트를 생성한다.
	static std::shared_ptr<CGameObject> CreateRenderObject(const std::wstring& tag, 
		const std::wstring& meshName, const std::wstring& materialName);
	static std::shared_ptr<CGameObject> CreateUIObject(const std::wstring& tag, const std::wstring& materialName, Vec2 pos);
	//기본 설정을 가진 지형 오브젝트를 생성한다.
	static std::shared_ptr<CGameObject> CreateTerrainObject(const std::wstring& tag, const std::wstring& heightMapName,
		UINT width, UINT height, Vec3 scale = {1.f,1.f,1.f});
	//바이너리 파일을 통해 오브젝트를 생성한다.
	static std::shared_ptr<CGameObject> CreateObjectFromFile(const std::wstring& tag, const std::wstring& fileName);

	std::shared_ptr<CTransform> GetTransform() { return mTransform; }
	std::shared_ptr<CMeshRenderer> GetMeshRendere() { return mMeshRenderer; }
	std::shared_ptr<CCollider> GetCollider() { return mCollider; }

	std::shared_ptr<CGameObject> GetSptrFromThis();
	const std::wstring& GetName() const { return mName; }
	const std::wstring& GetTag() const { return mTag; }
	const std::wstring& GetRenderLayer() const { return mRenderLayer; }
	bool GetActive() const { return mActive; }
	std::vector<std::shared_ptr<CGameObject>>& GetChildren() { return mChildren; }
	BoundingSphere GetRootBoundingSphere() const { return mRootBS; }

	void SetActive(bool active) { mActive = active; }
	void SetStatic(bool isStatic);
	void SetInstancing(bool isInstancing);
	void SetLayerType(LAYER_TYPE type) { mLayerType = type; }
	void SetRenderLayer(const std::wstring& layer) { mRenderLayer = layer; }
	void SetName(const std::wstring& name) { mName = name; }
	void SetTag(const std::wstring& tag) { mTag = tag; }
	void SetParent(const std::shared_ptr<CGameObject>& parent);

	void ReturnCBVIndex();
	 
	void CalculateRootOOBB();

	std::shared_ptr<CGameObject> FindChildByName(const std::wstring& name);

	void AddChild(std::shared_ptr<CGameObject> child);
	void RemoveChild(std::shared_ptr<CGameObject> child);

	template<typename T, typename... Args>
	std::shared_ptr<T> AddComponent(Args&&... args);
	template<typename T>
	std::shared_ptr<T> AddComponent(const std::shared_ptr<T>& component);
	template<typename T>
	std::shared_ptr<T> GetComponent();

private:

	static void InitFromFile(std::shared_ptr<CGameObject> obj, std::ifstream& inFile);
	void CreateTransformFromFile(std::ifstream& inFile);
	void CreateMeshRendererFromFile(std::ifstream& inFile);
	const BoundingBox& CombineChildrenOOBB();
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
