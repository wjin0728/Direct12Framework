#include "stdafx.h"
#include "MonoBehaviour.h"


CMonoBehaviour::CMonoBehaviour(const std::string& name) : mScriptName(name), CComponent(COMPONENT_TYPE::SCRIPT)
{
}

CMonoBehaviour::~CMonoBehaviour()
{
}

void CMonoBehaviour::Awake()
{
}

void CMonoBehaviour::Update()
{
}
