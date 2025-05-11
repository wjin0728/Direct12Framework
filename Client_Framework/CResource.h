#pragma once
#include"stdafx.h"

class CResource
{
public:
	bool isLoaded{ false };
protected:
	std::string name;
	RESOURCE_TYPE type;

protected:
	friend class CResourceManager;

	virtual void LoadFromFile(const std::string& _fileName) {};
	virtual void Save(const std::string& _fileName) {};
	virtual void CreateGPUResource() {};
	virtual void ReleaseUploadBuffer() {}

public:
	RESOURCE_TYPE GetType() const { return type; }
	const std::string& GetName() const { return name; }

	void SetName(const std::string& _name) { name = _name; }
	void SetType(RESOURCE_TYPE _type) { type = _type; }

};
