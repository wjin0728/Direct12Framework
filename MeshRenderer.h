#pragma once
#include "Component.h"

class CMesh;
class CMaterial;

class CMeshRenderer : public CComponent
{
private:
	std::shared_ptr<CMesh> m_mesh;
	std::shared_ptr<CMaterial> m_material;

public:
	CMeshRenderer();
	CMeshRenderer(const CMeshRenderer& other) : m_mesh(other.m_mesh), m_material(other.m_material), CComponent(other) {}
	~CMeshRenderer();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMeshRenderer>(*this); } 

public:
	void SetMesh(const std::shared_ptr<CMesh>& mesh) { m_mesh = mesh; }
	void SetMesh(const std::wstring& name);
	void SetMaterial(const std::shared_ptr<CMaterial>& material) { m_material = material; }
	void SetMaterial(const std::wstring& name);

	std::shared_ptr<CMesh> GetMesh() const { return m_mesh; }
	std::shared_ptr<CMaterial> GetMaterial() const { return m_material; }
};

