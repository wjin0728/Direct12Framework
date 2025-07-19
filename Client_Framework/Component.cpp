#include "stdafx.h"
#include "Component.h"
#include"Transform.h"
#include"GameObject.h"
#include"SceneManager.h"
#include"Scene.h"

CComponent::CComponent()
{
}

CComponent::CComponent(const CComponent& other)
{
}

CComponent::~CComponent()
{
}

void CComponent::SetOwner(CGameObject* _owner)
{
    owner = _owner;
}

void CComponent::EnqueueAwake()
{
    if(owner->mActive)
    {
        if(!mIsAwake){
            Awake();
            mIsAwake = true;
        }
        if(!mIsStart){
            INSTANCE(CSceneManager).GetCurScene()->AddComponentToStartQueue(this);
			mIsStart = true;
        }
	}
}

CGameObject* CComponent::GetOwner() const
{
    return owner;
}

const std::string& CComponent::GetName() const
{
	return owner->GetName();
}

std::shared_ptr<CTransform> CComponent::GetTransform() const
{
    return owner->GetTransform();
}


