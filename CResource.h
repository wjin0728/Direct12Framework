#pragma once
#include"stdafx.h"

class CResource
{
protected:
	std::wstring name;
	RESOURCE_TYPE type;

protected:
	friend class CResourceManager;

	virtual void LoadFromFile(std::wstring_view _fileName) {};
	virtual void Save(std::wstring_view _fileName) {};
	virtual void ReleaseUploadBuffer() {}

public:
	RESOURCE_TYPE GetType() const { return type; }
	const std::wstring& GetName() const { return name; }

	void SetName(std::wstring_view _name) { name = _name; }
	void SetType(RESOURCE_TYPE _type) { type = _type; }

};
