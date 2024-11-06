#pragma once
#include "Mesh.h"
#include"UploadBuffer.h"
#include"Material.h"
#include <typeindex>

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
private:
	std::map<COMPONENT_TYPE, std::shared_ptr<CComponent>> mComponents{};
	std::unordered_map<std::string, std::shared_ptr<CMonoBehaviour>> mScripts;

	std::shared_ptr<CTransform> mTransform{};
	std::shared_ptr<CMeshRenderer> mMeshRenderer{};
	std::shared_ptr<CCollider> mCollider{};

	std::vector<std::shared_ptr<CGameObject>> mChildren{};

private:
	bool mActive = true;
	std::wstring mName{};
	std::wstring mTag{};
	std::wstring mRenderLayer{};
	LAYER_TYPE mLayerType{};

public:
	CGameObject(bool makeTransform = true);
	virtual ~CGameObject();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	void Render(const std::shared_ptr<CCamera>& camera);
	void Render();

	bool AddComponent(const std::shared_ptr<CComponent>& component);
	bool AddScript(const std::shared_ptr<CMonoBehaviour>& component);

public:
	static std::shared_ptr<CGameObject> Instantiate(const std::shared_ptr<CGameObject>& original, 
		const std::shared_ptr<CTransform>& parentTransform = nullptr);
	static std::shared_ptr<CGameObject> Instantiate(const std::unique_ptr<CGameObject>& original,
		const std::shared_ptr<CTransform>& parentTransform = nullptr);

	static std::shared_ptr<CGameObject> CreateCameraObject(const std::wstring& tag, Vec2 rtSize, 
		float nearPlane = 1.01f, float farPlane = 1000.f, float fovAngle = 60.f);
	static std::shared_ptr<CGameObject> CreateRenderObject(const std::wstring& tag, 
		const std::wstring& meshName, const std::wstring& materialName);
	static std::shared_ptr<CGameObject> CreateTerrainObject(const std::wstring& tag, const std::wstring& heightMapName,
		UINT width, UINT height, Vec3 scale = {1.f,1.f,1.f});

	static std::shared_ptr<CGameObject> CreateObjectFromFile(const std::wstring& tag, const std::wstring& fileName);

	std::shared_ptr<CComponent> AddComponent(COMPONENT_TYPE type);
	std::shared_ptr<CMonoBehaviour> AddComponent(const std::string& name);

	std::shared_ptr<CComponent> GetComponent(COMPONENT_TYPE type);
	std::shared_ptr<CMonoBehaviour> GetComponent(const std::string& name);
	std::shared_ptr<CTransform> GetTransform() { return mTransform; }
	std::shared_ptr<CMeshRenderer> GetMeshRendere() { return mMeshRenderer; }
	std::shared_ptr<CCollider> GetCollider() { return mCollider; }

	const std::wstring& GetName() const { return mName; }
	const std::wstring& GetTag() const { return mTag; }
	const std::wstring& GetRenderLayer() const { return mRenderLayer; }
	bool GetActive() const { return mActive; }
	std::vector<std::shared_ptr<CGameObject>>& GetChildren() { return mChildren; }

	void SetActive(bool active) { mActive = active; }
	void SetLayerType(LAYER_TYPE type) { mLayerType = type; }
	void SetRenderLayer(const std::wstring& layer) { mRenderLayer = layer; }
	void SetName(const std::wstring& name) { mName = name; }
	void SetTag(const std::wstring& tag) { mTag = tag; }
	void SetComponentOwner(const std::shared_ptr<CGameObject>& owner);
	void SetParent(const std::shared_ptr<CGameObject>& parent);

	void ReturnCBVIndex();
	 
	void CalculateRootOOBB();

	std::shared_ptr<CGameObject> FindChildByName(const std::wstring& name);
private:

	static void InitFromFile(std::shared_ptr<CGameObject> obj, std::ifstream& inFile);
	void CreateTransformFromFile(std::ifstream& inFile);
	void CreateMeshRendererFromFile(std::ifstream& inFile);
	const BoundingBox& CombineChildrenOOBB();
};

