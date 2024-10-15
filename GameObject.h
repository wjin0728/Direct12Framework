#pragma once
#include "Mesh.h"
#include "Camera.h"
#include"UploadBuffer.h"
#include"Material.h"
#include"Component.h"

enum class LAYER_TYPE {

};

class CPlayer;
class CMonoBehaviour;
class CMeshRenderer;
class CTransform;

class CGameObject
{
private:
	std::map<COMPONENT_TYPE, std::shared_ptr<CComponent>> mComponents{};
	std::vector<std::shared_ptr<CMonoBehaviour>> mScripts{};

private:
	bool mActive = true;
	std::wstring mName{};
	LAYER_TYPE mLayerType{};

public:
	CGameObject();
	virtual ~CGameObject();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

public:
	void AddComponent(const std::shared_ptr<CComponent>& component);

	void SetActive(bool active) { mActive = active; }

	template<typename T>
	std::shared_ptr<T> GetComponent();
	bool GetActive() const { return mActive; }
	std::wstring GetName() const { return mName; }

private:
	template<typename T>
	COMPONENT_TYPE GetComponentType();
};


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
}
