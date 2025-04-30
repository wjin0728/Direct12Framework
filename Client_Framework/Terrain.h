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


	Vec3 mScale = Vec3::One;
	UINT mNavMapResolution{};
	std::vector<BYTE> mNavMapData{};
public:
	CTerrain();
	~CTerrain();

	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();

	virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CTerrain>(); }

	void Render(const std::shared_ptr<class CCamera>& camera, int pass);

public:
	void SetHeightMapGridMesh(const std::shared_ptr<CHeightMapGridMesh>& mesh);
	void SetMaterial(const std::shared_ptr<CTerrainMaterial>& material);
	void MakeNavMap(const std::string& fileName, UINT resolution);

	std::shared_ptr<CTerrainMaterial> GetMaterial() const { return mTerrainMaterial; }

	float GetHeight(float x, float z);
	Vec3 GetScale() const { return mScale; }
	bool CanMove(float x, float z);

	bool Raycast(const Ray& ray, float maxDist, float& distance);
};

