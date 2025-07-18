#include "stdafx.h"
#include "TargetMarker.h"
#include "GameObject.h"
#include "Transform.h"
#include"UIRenderer.h"
#include"Camera.h"
#include "RenderManager.h"
#include"Timer.h"
#include "HealthSystem.h"


CHealthSystem::CHealthSystem()
{
}

CHealthSystem::~CHealthSystem()
{
}

void CHealthSystem::Awake()
{

}

void CHealthSystem::Start()
{
	mHealthBarBackground = owner->AddComponent<CUIRenderer>();
}

void CHealthSystem::Update()
{
}

void CHealthSystem::LateUpdate()
{
	
}

void CHealthSystem::UpdateHealthBar()
{
}
