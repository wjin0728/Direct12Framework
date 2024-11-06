#pragma once
#include"Component.h"
#include "Collider.h"

class CCollider : public CComponent
{
private:
	BoundingBox mLocalAabb{};
	BoundingOrientedBox mLocalOobb = BoundingOrientedBox();
	BoundingOrientedBox mWorldOOBB = BoundingOrientedBox();

	Vec3 mScale = Vec3::One;

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

	void SetLocalOOBB(const BoundingOrientedBox& oobb);
	void SetScale(const Vec3& scale) { mScale = scale; };

	const BoundingOrientedBox& GetWorldOOBB() const { return mWorldOOBB; }
	const BoundingBox& GetAABB() const { return mLocalAabb; }
};

