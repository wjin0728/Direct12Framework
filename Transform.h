#pragma once
#include "Component.h"

class CTransform : public CComponent, public std::enable_shared_from_this<CTransform>
{
private:
	friend class CAnimationController;

	Vec3 mLocalRight = Vec3::Right;
	Vec3 mLocalUp = Vec3::Up;
	Vec3 mLocalLook = Vec3::Backward;

	Vec3 mLocalPosition{};
	Vec3 mLocalScale = { 1.f,1.f,1.f };
	Quaternion mLocalRotation = Quaternion::Identity;
	Vec3 mLocalEulerAngle{};

	Vec3 mScaleLayerBlending{};
	Vec3 mRotationLayerBlending{};
	Vec3 mPositionLayerBlending{};

	Matrix mWorldMat = Matrix::Identity;
	Matrix mLocalMat = Matrix::Identity;
	Matrix mTextureMat = Matrix::Identity;

private:
	friend CGameObject;
	friend class CCamera;
	friend class CMeshRenderer;
	friend class CSkinnedMeshRenderer;

	bool mDirtyFlag{};

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
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	//첫 번째 인자에 nullptr를 넣을 시 부모와 분리되어 씬의 오브젝트 컨테이너에 합류됨
	void SetParent(std::shared_ptr<CTransform> parent, bool isKeepLocalMat = false);

	void SetLocalPosition(const Vec3& position) { mLocalPosition = position; mDirtyFlag = true; };
	void SetLocalRotation(const Vec3& rotation) { mLocalRotation = Quaternion::CreateFromYawPitchRoll(rotation); 
	mLocalEulerAngle = rotation; mDirtyFlag = true; };
	void SetLocalRotation(const Quaternion & rotation) { mLocalRotation = rotation; mDirtyFlag = true; }
	void SetLocalRotationX(float angle) { SetLocalRotation({ angle, 0.f, 0.f }); }
	void SetLocalRotationY(float angle) { SetLocalRotation({ 0.f, angle, 0.f }); }
	void SetLocalRotationZ(float angle) { SetLocalRotation({ 0.f, 0.f, angle }); }
	void SetLocalScale(const Vec3& scale) { mLocalScale = scale; mDirtyFlag = true; };
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

	Vec3 GetPositionLayerBlending() const { return mPositionLayerBlending; }
	Vec3 GetRotationLayerBlending() const { return mRotationLayerBlending; }
	Vec3 GetScaleLayerBlending() const { return mScaleLayerBlending; };

	Vec3 GetWorldLook() const { return mWorldMat.Backward(); };
	Vec3 GetWorldUp() const { return mWorldMat.Up(); };
	Vec3 GetWorldRight() const { return mWorldMat.Right(); };

	Vec3 GetLocalLook() const { return mLocalMat.Backward(); };
	Vec3 GetLocalUp() const { return mLocalMat.Up(); };
	Vec3 GetLocalRight() const { return mLocalMat.Right(); };

	Matrix GetLocalMat() const { return mLocalMat; }
	Matrix GetWorldMat() const { return mWorldMat; }
	const Matrix& GetWorldMat(bool update = true);
	const Matrix& GetTexMat() { return mTextureMat; }

	std::shared_ptr<CTransform> GetRoot();

private:
	void UpdateLocalMatrix();
	void UpdateWorldMatrix();
	void ApplyBlendedTransform();
};

