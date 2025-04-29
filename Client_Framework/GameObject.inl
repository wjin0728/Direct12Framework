#pragma once
#include"GameObject.h"


template<typename T, typename ...Args>
inline std::shared_ptr<T> CGameObject::AddComponent(Args&&... args)
{
	std::shared_ptr<T> component = std::make_shared<T>(args...);
	COMPONENT_TYPE componentType = component->GetType();

	if (componentType == COMPONENT_TYPE::NOTHING) {
		return nullptr;
	}

	if (componentType == COMPONENT_TYPE::SCRIPT)
	{
		auto itr = mScripts.find(typeid(T));

		if (itr != mScripts.end()) {
			return std::dynamic_pointer_cast<T>(itr->second);
		}

		mScripts[typeid(T)] = component;
	}
	else
	{
		auto itr = mComponents.find(componentType);

		if (itr != mComponents.end()) {
			return std::dynamic_pointer_cast<T>(itr->second);
		}

		mComponents[componentType] = component;
	}

	return component;
}

template<typename T>
inline std::shared_ptr<T> CGameObject::GetComponent()
{
	COMPONENT_TYPE componentType = GetComponentType<T>();

	if (componentType == COMPONENT_TYPE::NOTHING) {
		return nullptr;
	}

	if (componentType == COMPONENT_TYPE::SCRIPT)
	{
		auto itr = mScripts.find(typeid(T));

		if (itr != mScripts.end()) {
			return std::dynamic_pointer_cast<T>(itr->second);
		}
	}
	else
	{
		auto itr = mComponents.find(componentType);

		if (itr != mComponents.end()) {
			return std::dynamic_pointer_cast<T>(itr->second);
		}
	}

	return nullptr;
}


template<typename T>
inline constexpr COMPONENT_TYPE CGameObject::GetComponentType()
{
	if constexpr (std::is_same_v<T, CTransform>)
		return COMPONENT_TYPE::TRANSFORM;
	else if constexpr (std::is_same_v<T, CMeshRenderer>)
		return COMPONENT_TYPE::MESH_RENDERER;
	else if constexpr (std::is_same_v<T, CCamera>)
		return COMPONENT_TYPE::CAMERA;
	else if constexpr (std::is_same_v<T, CTerrain>)
		return COMPONENT_TYPE::TERRAIN;
	else if constexpr (std::is_same_v<T, CRigidBody>)
		return COMPONENT_TYPE::RIGID_BODY;
	else if constexpr (std::is_same_v<T, CCollider>)
		return COMPONENT_TYPE::COLLIDER;
	else if constexpr (std::is_base_of_v<T, CMonoBehaviour>)
		return COMPONENT_TYPE::SCRIPT;
	else
		return COMPONENT_TYPE::NOTHING;
}