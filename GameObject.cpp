#include "GameObject.h"
#include"MonoBehaviour.h"
#include"Transform.h"
#include"Camera.h"

CGameObject::CGameObject(bool makeTransform)
{
	if (makeTransform) {
		mTransform = std::make_shared<CTransform>();
		mComponents[COMPONENT_TYPE::TRANSFORM] = mTransform;
	}
}

CGameObject::~CGameObject()
{
}

void CGameObject::Awake()
{
	for (auto& [type, component] : mComponents) {
		component->Awake();
	}
	for (auto& script : mScripts) {
		script->Awake();
	}

	auto& children = mTransform->mChildren;
	for (auto& child : children) {
		child->GetOwner()->Awake();
	}
}

void CGameObject::Start()
{
	for (auto& [type, component] : mComponents) {
		component->Start();
	}
	for (auto& script : mScripts) {
		script->Start();
	}

	auto& children = mTransform->mChildren;
	for (auto& child : children) {
		child->GetOwner()->Start();
	}
}

void CGameObject::Update()
{
	for (auto& [type, component] : mComponents) {
		component->Update();
	}
	for (auto& script : mScripts) {
		script->Update();
	}

	auto& children = mTransform->mChildren;
	for (auto& child : children) {
		child->GetOwner()->Update();
	}

}

void CGameObject::LateUpdate()
{
	for (auto& [type, component] : mComponents) {
		component->LateUpdate();
	}
	for (auto& script : mScripts) {
		script->LateUpdate();
	}

	auto& children = mTransform->mChildren;
	for (auto& child : children) {
		child->GetOwner()->LateUpdate();
	}
	mTransform->isMoved = false;
}

void CGameObject::FixedUpdate()
{
	for (auto& [type, component] : mComponents) {
		component->FixedUpdate();
	}
	for (auto& script : mScripts) {
		script->FixedUpdate();
	}

	auto& children = mTransform->mChildren;
	for (auto& child : children) {
		child->GetOwner()->FixedUpdate();
	}
}

bool CGameObject::AddComponent(const std::shared_ptr<CComponent>& component)
{
	COMPONENT_TYPE componentType = component->GetType();

	auto itr = mComponents.find(componentType);

	if (itr != mComponents.end()) {
		return false;
	}

	mComponents[componentType] = component;

	return true;
}

std::shared_ptr<CGameObject> CGameObject::FindByTag(const std::wstring& tag)
{
	if (mTag == tag) {
		return std::shared_ptr<CGameObject>(this);
	}

	std::shared_ptr<CGameObject> obj = nullptr;

	auto& children = mTransform->mChildren;
	for (auto& child : children) {
		obj = child->GetOwner()->FindByTag(tag);
	}

	return obj;
}

std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::shared_ptr<CGameObject>& original)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>();

	for (const auto& [key, component] : original->mComponents) {
		instance->mComponents[key] = component->Clone();
		instance->mComponents[key]->SetOwner(instance);
	}

	for (const auto& originChildTransform : original->mTransform->mChildren) {
		auto childInstance = Instantiate(originChildTransform->GetOwner());
		childInstance->mTransform->SetParent(instance->mTransform);          
	}

	return instance;
}

std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::unique_ptr<CGameObject>& original)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>(false);

	for (const auto& [key, component] : original->mComponents) {
		instance->mComponents[key] = component->Clone();
		instance->mComponents[key]->SetOwner(instance);
	}

	instance->mTransform = instance->GetComponent<CTransform>();
	instance->mTag = original->mTag;
	instance->mLayerType = original->mLayerType;
	


	for (const auto& originChildTransform : original->mTransform->mChildren) {
		auto childInstance = Instantiate(originChildTransform->GetOwner());
		childInstance->mTransform->SetParent(instance->mTransform);
	}

	return instance;
}


