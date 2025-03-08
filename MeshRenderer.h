#pragma once
#include "Component.h"

class CMesh;
class CMaterial;
class CCamera;

class CMeshRenderer : public CComponent
{
private:
	std::shared_ptr<CMesh> m_mesh;
	std::vector<std::shared_ptr<CMaterial>> m_materials;


public:
	CMeshRenderer();
	CMeshRenderer(const CMeshRenderer& other) : m_mesh(other.m_mesh), m_materials(other.m_materials), CComponent(other) {}
	~CMeshRenderer();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	void Render();
	void InstancingRender(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum);

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMeshRenderer>(*this); } 

public:
	void SetMesh(const std::shared_ptr<CMesh>& mesh);
	void SetMesh(const std::wstring& name);
	void AddMaterial(const std::shared_ptr<CMaterial>& material);
	void AddMaterial(const std::wstring& name);

	std::shared_ptr<CMesh> GetMesh() const { return m_mesh; }
	std::shared_ptr<CMaterial> GetMaterial(UINT idx = 0) const { return m_materials[idx]; }
};

