#pragma once
#include "Component.h"
#include"stdafx.h"

class CPortal :
    public CComponent
{
public:
    CPortal();
    virtual ~CPortal();
public:
    virtual void Awake() override;
    virtual void Start() override;
    virtual void Update() override;
};

