#pragma once
#include"stdafx.h"

class CResource
{
protected:
	std::string name;
	RESOURCE_TYPE type;

protected:
	friend class CResourceManager;

	virtual void LoadFromFile(std::string_view _fileName) {};
	virtual void Save(std::string_view _fileName) {};
	virtual void ReleaseUploadBuffer() {}

public:
	RESOURCE_TYPE GetType() const { return type; }
	const std::string& GetName() const { return name; }

	void SetName(std::string_view _name) { name = _name; }
	void SetType(RESOURCE_TYPE _type) { type = _type; }

};
