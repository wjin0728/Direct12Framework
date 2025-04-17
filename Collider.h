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

	const BoundingOrientedBox& GetWorldOOBB() const { return mWorldOOBB; }
};

