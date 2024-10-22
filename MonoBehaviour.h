#pragma once
#include"Component.h"

class CMonoBehaviour : public CComponent
{
public:
	CMonoBehaviour(); 
	CMonoBehaviour(const CMonoBehaviour& other) : CComponent(other) {}
	~CMonoBehaviour(); 

	virtual void Awake() override; 
	virtual void Update() override; 

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMonoBehaviour>(*this); }
};

