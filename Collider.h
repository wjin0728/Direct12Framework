#pragma once
#include"Component.h"
#include "Collider.h"

class CCollider : public CComponent
{
private:
	BoundingOrientedBox mLocalOobb = BoundingOrientedBox();
	BoundingOrientedBox mWorldOOBB = BoundingOrientedBox();

public:
	CCollider();
	~CCollider();

	virtual std::shared_ptr<CComponent> Clone() override;

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

public:

	void UpdateOOBB(const Matrix& worldMat);
	void Initialize(const Vec3& center, const Vec3& size);
	void Initialize(const BoundingOrientedBox& oobb);
	bool IsIntersect(const BoundingOrientedBox& oobb) const
	{
		return mWorldOOBB.Intersects(oobb);
	}
	bool IsIntersect(const BoundingSphere& sphere) const
	{
		return mWorldOOBB.Intersects(sphere);
	}
	bool IsIntersect(const Vec3& point) const
	{
		return mWorldOOBB.Intersects(point);
	}
	bool IsIntersect(const Ray& ray, float& dist) const
	{
		return mWorldOOBB.Intersects(ray.position, ray.direction, dist);
	}



	const BoundingOrientedBox& GetWorldOOBB() const { return mWorldOOBB; }
};

