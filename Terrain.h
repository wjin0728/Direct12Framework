#pragma once
#include"Component.h"
#include"QuadTree.h"
#include"Material.h"

class CTerrain : public CComponent
{
private:
	std::shared_ptr<CQuadTree> mQuadTree{};

	std::shared_ptr<CHeightMapGridMesh> mTerrainMesh{};
	std::shared_ptr<CTerrainMaterial> mTerrainMaterial{};

public:
	CTerrain();
	~CTerrain();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CTerrain>(); }

	void Render(const std::shared_ptr<class CCamera>& camera);

public:
	void SetHeightMapGridMesh(const std::shared_ptr<CHeightMapGridMesh>& mesh);
	void SetMaterial(const std::shared_ptr<CTerrainMaterial>& material);

	std::shared_ptr<CTerrainMaterial> GetMaterial() const { return mTerrainMaterial; }
};

