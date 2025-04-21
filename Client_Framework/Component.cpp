#include "stdafx.h"
#include "Component.h"
#include"Transform.h"
#include"GameObject.h"

CComponent::CComponent(COMPONENT_TYPE type) : type(type)
{
}

CComponent::CComponent(const CComponent& other) : type(other.type)
{
}

CComponent::~CComponent()
{
}

void CComponent::SetOwner(CGameObject* _owner)
{
    owner = _owner;
}

CGameObject* CComponent::GetOwner() const
{
    return owner;
}

std::shared_ptr<CTransform> CComponent::GetTransform() const
{
    return owner->GetTransform();
}


