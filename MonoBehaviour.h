#pragma once
#include"Component.h"

class CMonoBehaviour : public CComponent
{
public:
	CMonoBehaviour(); 
	~CMonoBehaviour(); 

	virtual void Awake() override; 
	virtual void Update() override; 
};

