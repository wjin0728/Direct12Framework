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
class CSkinnedMesh;
class CAnimationController;
class CModelInfo;
class CAnimationTrack;
class CAnimationLayer;
class CAnimationSet;
class CModelInfo;


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
	//������Ʈ�� ���纻�� �����Ѵ�.
	static std::shared_ptr<CGameObject> Instantiate(const std::shared_ptr<CGameObject>& original, 
		const std::shared_ptr<CTransform>& parentTransform = nullptr);
	//������Ʈ�� ���纻�� �����Ѵ�.
	static std::shared_ptr<CGameObject> Instantiate(const std::unique_ptr<CGameObject>& original,
		const std::shared_ptr<CTransform>& parentTransform = nullptr);

	//�⺻ ������ ���� ī�޶� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CGameObject> CreateCameraObject(const std::wstring& tag, Vec2 rtSize, 
		float nearPlane = 1.01f, float farPlane = 1000.f, float fovAngle = 60.f);
	//�⺻ ������ ���� ���� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CGameObject> CreateRenderObject(const std::wstring& tag, 
		const std::wstring& meshName, const std::wstring& materialName);
	static std::shared_ptr<CGameObject> CreateUIObject(const std::wstring& tag, const std::wstring& materialName, Vec2 pos);
	//�⺻ ������ ���� ���� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CGameObject> CreateTerrainObject(const std::wstring& tag, const std::wstring& heightMapName,
		UINT width, UINT height, Vec3 scale = {1.f,1.f,1.f});
	//���̳ʸ� ������ ���� ������Ʈ�� �����Ѵ�.
	static std::shared_ptr<CModelInfo> CreateObjectFromFile(const std::wstring& tag, const std::wstring& fileName);

	std::shared_ptr<CTransform> GetTransform() { return mTransform; }
	std::shared_ptr<CMeshRenderer> GetMeshRenderer() { return mMeshRenderer; }
	std::shared_ptr<CCollider> GetCollider() { return mCollider; }
	std::shared_ptr<CAnimationController> GetAnimationController() { return mAnimationController; }

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

public:
	void UpdateTransform(const Matrix* parent = nullptr);
	virtual void Animate(float elapsedTime);
	void ResetForAnimationBlending();
	void CacheFrameHierarchies(std::vector<std::shared_ptr<CGameObject>>& boneFrameCaches);

	std::shared_ptr<CAnimationController> mAnimationController{};

	void FindAndSetSkinnedMesh(std::vector<std::shared_ptr<CSkinnedMesh>>& skinnedMeshes, int meshNum);
	
	void SetTrackAnimationSet(int trackIndex, int setIndex);
	void SetTrackAnimationPosition(int trackIndex, float position);

private:

	static void InitFromFile(std::shared_ptr<CModelInfo> model, std::ifstream& inFile);
	static void InitHierarchyFromFile(std::shared_ptr<CGameObject> obj, std::ifstream& inFile, int* skinnedMeshCnt);
	static void InitAnimationFromFile(std::shared_ptr<CModelInfo> model, std::ifstream& inFile);

	void CreateTransformFromFile(std::ifstream& inFile);
	void CreateMeshRendererFromFile(std::ifstream& inFile, std::string& meshType);

	static void CreateAnimationSetFromFile(std::shared_ptr<CModelInfo>& model, std::ifstream& inFile);
	static void CreateAnimationLayerFromFile(std::shared_ptr<CModelInfo>& model, std::ifstream& inFile, std::shared_ptr<CAnimationSet>& animSet, int layerIndex);
	static void CreateAnimationCurvesFromFile(std::ifstream& inFile, std::shared_ptr<CAnimationLayer>& layer, int curveIndex);
	static int CalculateCommonBoneNum(std::shared_ptr<CAnimationSet>& animSet);

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
