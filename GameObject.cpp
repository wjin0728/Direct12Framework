#include "GameObject.h"
#include"MonoBehaviour.h"

CGameObject::CGameObject()
{
}

CGameObject::~CGameObject()
{
}

void CGameObject::Awake()
{
}

void CGameObject::Start()
{
}

void CGameObject::Update()
{
	for (auto& [type, component] : mComponents) {
		component->Update();
	}
	for (auto& script : mScripts) {
		script->Update();
	}
	auto transform = std::static_pointer_cast<CTransform>(mComponents[COMPONENT_TYPE::TRANSFORM]);


}

void CGameObject::LateUpdate()
{
}

void CGameObject::FixedUpdate()
{
}

void CGameObject::AddComponent(const std::shared_ptr<CComponent>& component)
{
	COMPONENT_TYPE componentType = component->GetType();

	auto itr = mComponents.find(componentType);

	if (itr == mComponents.end()) {
		mComponents[componentType] = component;
	}
}

