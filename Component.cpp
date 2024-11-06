#include "stdafx.h"
#include "Component.h"
#include"GameObject.h"
#include"Transform.h"

CComponent::CComponent(COMPONENT_TYPE type) : type(type)
{
}

CComponent::CComponent(const CComponent& other) : type(other.type)
{
}

CComponent::~CComponent()
{
}

void CComponent::SetOwner(const std::shared_ptr<CGameObject>& _owner)
{
    owner = _owner;
}

std::shared_ptr<CGameObject> CComponent::GetOwner()
{
    return owner.lock();
}

const std::shared_ptr<CTransform> CComponent::GetTransform()
{
    return owner.lock()->GetTransform();
}


