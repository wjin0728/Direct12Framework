#pragma once
#include "Renderer.h"
#include"InstancingManager.h"

class CMesh;
class CMaterial;
class CCamera;
class CSkinnedMesh;

class CMeshRenderer : public CRenderer
{
private:
	std::shared_ptr<CMesh> m_mesh;

public:
	CMeshRenderer();
	CMeshRenderer(const CMeshRenderer& other) : m_mesh(other.m_mesh), CRenderer(other) {}
	~CMeshRenderer();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	virtual void Render(std::shared_ptr<CCamera> camera, int pass = 0) override;
	void InstancingRender(D3D12_VERTEX_BUFFER_VIEW ibv, UINT instancingNum);

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CMeshRenderer>(*this); } 

public:
	void SetMesh(const std::shared_ptr<CMesh>& mesh);
	void SetMesh(const std::string& name);

	std::shared_ptr<CMesh> GetMesh() const { return m_mesh; }

	InstanceKey GetInstancingKey() const
	{
		InstanceKey key{};
		key.mesh = m_mesh.get();
		key.material = m_materials[0].get();
		return key;
	}
};

