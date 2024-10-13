#pragma once
#include "Component.h"

class CTransform : public CComponent
{
private:
	XMFLOAT3 mRight = { 1.f,0.f,0.f };
	XMFLOAT3 mUp = { 0.f,1.f,0.f };
	XMFLOAT3 mLook = { 0.f,0.f,1.f };

	XMFLOAT3 mRotation{};
	XMFLOAT3 mPosition{};
	XMFLOAT3 mScale = { 1.f,1.f,1.f };

	XMFLOAT4X4 mWorldMat = Matrix4x4::Identity();
	XMFLOAT4X4 mLocalMat = Matrix4x4::Identity();

public:
	CTransform();
	~CTransform();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

public:
	void MoveStrafe(float distance = 1.0f);
	void MoveUp(float distance = 1.0f);
	void MoveForward(float distance = 1.0f);
	void Move(const XMFLOAT3& direction, float speed = 1.f);

	void LookTo(const XMFLOAT3& lookDir, const XMFLOAT3& up = { 0.f,1.f,0.f });
	void LookAt(const XMFLOAT3& lookPos, const XMFLOAT3& up = { 0.f,1.f,0.f });

	void Rotate(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);
	void Rotate(const XMFLOAT3& rotation);
	void Rotate(const XMFLOAT3& axis, float angle);

	void SetPosition(const XMFLOAT3& position) { mPosition = position; };
	void SetRotation(const XMFLOAT3& rotation) { mRotation = rotation; };
	void SetScale(const XMFLOAT3& scale) { mScale = scale; };
	void SetLook(const XMFLOAT3& look) { mLook = look; };
	void SetUp(const XMFLOAT3& up) { mUp = up; };
	void SetRight(const XMFLOAT3& right) { mRight = right; };

	XMFLOAT3 GetPosition() const { return mPosition; };
	XMFLOAT3 GetRotation() const { return mRotation; };
	XMFLOAT3 GetScale() const { return mScale; };
	XMFLOAT3 GetLook() const { return mLook; };
	XMFLOAT3 GetUp() const { return mUp; };
	XMFLOAT3 GetRight() const { return mRight; };
};

