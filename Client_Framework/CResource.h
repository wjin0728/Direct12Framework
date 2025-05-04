#pragma once
#include"stdafx.h"

class CResource
{
protected:
	std::string name;
	RESOURCE_TYPE type;

protected:
	friend class CResourceManager;

	virtual void LoadFromFile(const std::string& _fileName) {};
	virtual void Save(const std::string& _fileName) {};
	virtual void ReleaseUploadBuffer() {}

public:
	RESOURCE_TYPE GetType() const { return type; }
	const std::string& GetName() const { return name; }

	void SetName(const std::string& _name) { name = _name; }
	void SetType(RESOURCE_TYPE _type) { type = _type; }

};
