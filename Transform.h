#pragma once
#include "Component.h"

class CTransform : public CComponent, public std::enable_shared_from_this<CTransform>
{
private:
	Vec3 mRight = Vec3::Right;
	Vec3 mUp = Vec3::Up;
	Vec3 mLook = Vec3::Backward;

	Vec3 mPosition{};
	Vec3 mScale = { 1.f,1.f,1.f };
	Quaternion mRotation = Quaternion::Identity;

	Matrix mWorldMat = Matrix::Identity;
	Matrix mLocalMat = Matrix::Identity;

private:
	bool isMoved{};
	int dirtyFramesNum{};

	std::weak_ptr<CTransform> mParent{};
	std::vector<std::shared_ptr<CTransform>> mChildren{};

public:
	CTransform();
	~CTransform();

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;

	void SetParent(std::shared_ptr<CTransform> parent);

public:
	void MoveStrafe(float distance = 1.0f);
	void MoveUp(float distance = 1.0f);
	void MoveForward(float distance = 1.0f);
	void Move(const Vec3& direction, float distance = 1.f);

	void LookTo(const Vec3& lookDir, const Vec3& up = { 0.f,1.f,0.f });
	void LookAt(const Vec3& lookPos, const Vec3& up = { 0.f,1.f,0.f });
	void LookAt(const CTransform& target, const Vec3& up = { 0.f,1.f,0.f });


	void Rotate(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);
	void Rotate(const Vec3& rotation);
	void Rotate(const Vec3& axis, float angle);

	void SetPosition(const Vec3& position) { mPosition = position; };
	void SetRotation(const Vec3& rotation) { mRotation = Quaternion::CreateFromYawPitchRoll(rotation); };
	void SetScale(const Vec3& scale) { mScale = scale; };
	void SetLook(const Vec3& look) { mLook = look; };
	void SetUp(const Vec3& up) { mUp = up; };
	void SetRight(const Vec3& right) { mRight = right; };

	Vec3 GetPosition() const { return mPosition; };
	Vec3 GetRotation() const { return Vec3::GetAngleToQuaternion(mRotation); }
	Vec3 GetScale() const { return mScale; };
	Vec3 GetLook() const { return mLook; };
	Vec3 GetUp() const { return mUp; };
	Vec3 GetRight() const { return mRight; };

	std::wstring GetObjectName();

	std::shared_ptr<CTransform> GetChild(UINT idx) { return mChildren[idx]; };
	std::shared_ptr<CTransform> FindChild(const std::wstring& name);
};

