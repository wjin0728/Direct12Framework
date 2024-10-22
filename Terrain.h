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
	CTerrain(const std::shared_ptr<CHeightMapGridMesh>& mesh, const std::shared_ptr<CTerrainMaterial>& material);
	~CTerrain();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CTerrain>(mTerrainMesh, mTerrainMaterial); }

	void Render(const std::shared_ptr<class CCamera>& camera);

};

