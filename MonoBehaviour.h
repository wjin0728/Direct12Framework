#pragma once
#include"stdafx.h"
#include"Component.h"

class CMonoBehaviour : public CComponent
{
private:
	std::string mScriptName{};

public:
	CMonoBehaviour(const std::string& name);
	CMonoBehaviour(const CMonoBehaviour& other) : CComponent(other) {}
	virtual ~CMonoBehaviour(); 

	virtual void Awake() override; 
	virtual void Update() override; 

	const std::string& GetName() const { return mScriptName; }

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMonoBehaviour>(*this); }
};

