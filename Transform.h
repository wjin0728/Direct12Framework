#pragma once
#include "Component.h"

class CTransform : public CComponent, public std::enable_shared_from_this<CTransform>
{
private:
	Vec3 mLocalRight = Vec3::Right;
	Vec3 mLocalUp = Vec3::Up;
	Vec3 mLocalLook = Vec3::Backward;

	Vec3 mLocalPosition{};
	Vec3 mLocalScale = { 1.f,1.f,1.f };
	Quaternion mLocalRotation = Quaternion::Identity;
	Vec3 mLocalEulerAngle{};

	Matrix mWorldMat = Matrix::Identity;
	Matrix mLocalMat = Matrix::Identity;
	Matrix mTextureMat = Matrix::Identity;

private:
	int mCbvIdx = -1;

private:
	friend CGameObject;
	friend class CCamera;

	bool isMoved{};

	std::weak_ptr<CTransform> mParent{};

public:
	int dirtyFramesNum{};
	CTransform();
	~CTransform();

	virtual std::shared_ptr<CComponent> Clone() override;

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	void Reset();
	void SetCBVIndex();
	void ReturnCBVIndex();
	void BindConstantBuffer();

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

	void SetParent(std::shared_ptr<CTransform> parent);
	void SetLocalPosition(const Vec3& position) { mLocalPosition = position; isMoved = true; };
	void SetLocalRotation(const Vec3& rotation) { mLocalRotation = Quaternion::CreateFromYawPitchRoll(rotation); 
	mLocalEulerAngle = rotation; isMoved = true; };
	void SetLocalRotation(const Quaternion & rotation) { mLocalRotation = rotation; isMoved = true; }
	void SetLocalScale(const Vec3& scale) { mLocalScale = scale; isMoved = true; };
	void SetLocalLook(const Vec3& look) { mLocalLook = look; };
	void SetLocalUp(const Vec3& up) { mLocalUp = up; };
	void SetLocalRight(const Vec3& right) { mLocalRight = right; };

	void SetTexMat(const Matrix& mat) { mTextureMat = mat; }

	std::shared_ptr<CTransform> GetParent() const { return mParent.lock(); }
	Vec3 GetLocalPosition() const { return mLocalPosition; }
	Vec3 GetWorldPosition();
	Vec3 GetLocalEulerAngles() const { return mLocalEulerAngle; }
	Quaternion GetLocalRotation() const { return mLocalRotation; }
	Vec3 GetLocalScale() const { return mLocalScale; };

	Vec3 GetWorldLook() const { return mWorldMat.Backward(); };
	Vec3 GetWorldUp() const { return mWorldMat.Up(); };
	Vec3 GetWorldRight() const { return mWorldMat.Right(); };

	Vec3 GetLocalLook() const { return mLocalMat.Backward(); };
	Vec3 GetLocalUp() const { return mLocalMat.Up(); };
	Vec3 GetLocalRight() const { return mLocalMat.Right(); };

	const Matrix& GetWorldMat();
	const Matrix& GetTexMat() { return mTextureMat; }

private:
	void UpdateLocalMatrix();
	void UpdateWorldMatrix();
	void CopyToCbvBuffer();
};

