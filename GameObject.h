#pragma once
#include "Mesh.h"
#include "Camera.h"
#include"UploadBuffer.h"
#include"Material.h"
#include"Component.h"

enum class LAYER_TYPE : UINT8
{
	PLAYER,
	ENEMY,
	UI,
	TERRAIN,
};

class CPlayer;
class CMonoBehaviour;
class CMeshRenderer;
class CTransform;
class CCamara;
class CTerrain;

class CGameObject : public std::enable_shared_from_this<CGameObject>
{
private:
	std::map<COMPONENT_TYPE, std::shared_ptr<CComponent>> mComponents{};
	std::vector<std::shared_ptr<CMonoBehaviour>> mScripts{};

	std::shared_ptr<CTransform> mTransform{};

private:
	bool mActive = true;
	std::wstring mTag{};
	LAYER_TYPE mLayerType{};

public:
	CGameObject(bool makeTransform = true);
	virtual ~CGameObject();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

public:
	static std::shared_ptr<CGameObject> Instantiate(const std::shared_ptr<CGameObject>& original);
	static std::shared_ptr<CGameObject> Instantiate(const std::unique_ptr<CGameObject>& original);

	template<typename T, typename... Args>
	std::shared_ptr<T> AddComponent(Args&&... args);
	bool AddComponent(const std::shared_ptr<CComponent>& component);

	template<typename T>
	std::shared_ptr<T> GetComponent();
	std::shared_ptr<CTransform> GetTransform() { return mTransform; }
	const std::wstring& GetTag() const { return mTag; }
	bool GetActive() const { return mActive; }

	void SetActive(bool active) { mActive = active; }
	void SetLayerType(LAYER_TYPE type) { mLayerType = type; }
	void SetTag(const std::wstring& tag) { mTag = tag; }

	std::shared_ptr<CGameObject> FindByTag(const std::wstring& tag);

private:
	template<typename T>
	COMPONENT_TYPE GetComponentType();
};


template<typename T, typename ...Args>
inline std::shared_ptr<T> CGameObject::AddComponent(Args&&... args)
{
	std::shared_ptr<T> component = std::make_shared<T>(args...);
	COMPONENT_TYPE componentType = component->GetType();

	auto itr = mComponents.find(componentType);

	if (itr != mComponents.end()) {
		return std::static_pointer_cast<T>(itr->second);
	}

	mComponents[componentType] = component;

	return component;
}

template<typename T>
inline std::shared_ptr<T> CGameObject::GetComponent()
{
	COMPONENT_TYPE componentType = GetComponentType<T>();
	auto itr = mComponents.find(componentType);

	if (itr != mComponents.end()) {
		return std::static_pointer_cast<T>(itr->second);
	}

	return nullptr;
}

template<typename T>
inline COMPONENT_TYPE CGameObject::GetComponentType()
{
	if (std::is_same_v<T, CTransform>)
		return COMPONENT_TYPE::TRANSFORM;
	else if (std::is_same_v<T, CMeshRenderer>)
		return COMPONENT_TYPE::MESH_RENDERER;
	else if (std::is_same_v<T, CCamera>)
		return COMPONENT_TYPE::CAMERA;
	else if (std::is_same_v<T, CTerrain>)
		return COMPONENT_TYPE::TERRAIN;
}
