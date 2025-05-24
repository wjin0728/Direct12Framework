#include "stdafx.h"
#include "ThirdPersonCamera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Timer.h"
#include "InputManager.h"
#include "Terrain.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Renderer.h"


CThirdPersonCamera::CThirdPersonCamera()
	: mCameraParams{}
	, mTarget{}
	, mTerrain{}
{
	mFreeLook = false;
}

CThirdPersonCamera::~CThirdPersonCamera()
{
}

void CThirdPersonCamera::Awake()
{
	mCamera = GetOwner()->GetComponent<CCamera>();
}

void CThirdPersonCamera::Start()
{
	auto scene = INSTANCE(CSceneManager).GetCurScene();
	mTerrain = scene->GetTerrain();
	mFreeLook = false;

	mCameraParams.trackingPosition = mTarget->GetTransform()->GetWorldPosition();
	mCameraParams.distance = 5.f;
	mCameraParams.framing = Vec2(0.f, -0.8f);
	mCameraParams.yaw = 0.f;
	mCameraParams.pitch = 25.f;

	mDeadZoneSize = { -0.05f, 0.05f};
	SetCameraParams(mCameraParams);

}

void CThirdPersonCamera::Update()
{
	if (!mTarget || !mCamera) return;
	float deltaTime = DELTA_TIME;
	float speed = deltaTime * 5.f;

	if (INPUT.IsKeyDown(KEY_TYPE::F1)) {
		mFreeLook = !mFreeLook;
	}
	
	if (mFreeLook) FreeMovement();
	else FollowTarget(speed, deltaTime);
	//RaycastObjects();

	auto transform = GetTransform();
	Vec3 camPos = transform->GetWorldPosition();
	//float terrainHeight = mTerrain.lock()->GetHeight(camPos.x, camPos.z);
	//if (camPos.y < terrainHeight + 0.5f)
	//{
	//	camPos.y = terrainHeight + 0.5f;
	//	transform->SetLocalPosition(camPos);
	//}

}

void CThirdPersonCamera::FollowTarget(float speed, float deltaTime)
{

	if (mCanRotate) {
		Vec2 mouseDelta = INPUT.GetMouseDelta();

		int direction = 0;
		if (INPUT.IsKeyPress(KEY_TYPE::D)) {
			direction += 1;
		}
		if (INPUT.IsKeyPress(KEY_TYPE::A)) {
			direction -= 1;
		}

		float yawOffsetSmooth = std::lerp(0.f, direction * 10.f, speed);
		mCameraParams.yaw += yawOffsetSmooth;

		float sensitivity = 0.5f;
		mCameraParams.yaw += mouseDelta.x * sensitivity;
		mCameraParams.pitch += mouseDelta.y * sensitivity;

		float minPitch = -60.f;
		float maxPitch = 45.f;

		mCameraParams.pitch = std::clamp(mCameraParams.pitch, minPitch, maxPitch);
		if (mCameraParams.yaw > 180.f) { mCameraParams.yaw -= 360.f; }
		else if (mCameraParams.yaw < -180.f) { mCameraParams.yaw += 360.f; }
	}

	mCameraParams.trackingPosition = mTarget->GetTransform()->GetWorldPosition();
	CameraBlend blendout = ComputeBlendout();
	float progress = deltaTime * 20.f;
	progress = std::clamp(progress, 0.f, 1.f);
	SetParamsBlended(mCameraParams, blendout, progress);
}

void CThirdPersonCamera::FreeMovement()
{
	auto transform = GetTransform();
	Vec3 position = transform->GetWorldPosition();
	Vec3 forward = transform->GetWorldLook();
	Vec3 right = transform->GetWorldRight();
	Vec3 up = transform->GetWorldUp();
	float speed = 5.f * DELTA_TIME;

	Vec3 moveDir = Vec3::Zero;
	uint8_t dir = 0;
	if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
	if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
	if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
	if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;
	if (INPUT.IsKeyPress(KEY_TYPE::Q)) dir |= KEY_FLAG::KEY_Q;
	if (INPUT.IsKeyPress(KEY_TYPE::E)) dir |= KEY_FLAG::KEY_E;
	if (INPUT.IsKeyPress(KEY_TYPE::SPACE)) speed *= 2.f; // Speed up when space is pressed

	if (dir & KEY_FLAG::KEY_W) moveDir += forward;
	if (dir & KEY_FLAG::KEY_S) moveDir -= forward;
	if (dir & KEY_FLAG::KEY_D) moveDir += right;
	if (dir & KEY_FLAG::KEY_A) moveDir -= right;
	if (dir & KEY_FLAG::KEY_Q) moveDir += up;
	if (dir & KEY_FLAG::KEY_E) moveDir -= up;

	if (moveDir.Length() > 0.01f) {
		moveDir.Normalize();
		position += moveDir * speed;
	}

	Vec2 mouseDelta = INPUT.GetMouseDelta();
	float sensitivity = 0.5f;

	float minPitch = -60.f;
	float maxPitch = 45.f;
	float rotationX = transform->GetLocalEulerAngles().x + mouseDelta.y * sensitivity;
	rotationX = std::clamp(rotationX, minPitch, maxPitch);
	float rotationY = transform->GetLocalEulerAngles().y + mouseDelta.x * sensitivity;
	if (rotationY > 180.f) { rotationY -= 360.f; }
	else if (rotationY < -180.f) { rotationY += 360.f; }
	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(rotationY * degToRad, rotationX * degToRad, 0);
	transform->SetLocalRotation(rotation);
	transform->SetLocalPosition(position);
}

void CThirdPersonCamera::LateUpdate()
{
}

void CThirdPersonCamera::SetCameraParams(CameraParams& params)
{
	auto transform = GetTransform();
	float fov = mCamera->GetFov();
	float aspect = mCamera->GetAspect();

	float tanFOVY = tan(0.5f * fov * degToRad);
	float tanFOVX = tanFOVY * aspect;
	Vec3 localOffset = Vec3(
		params.distance * tanFOVX * params.framing.x,
		params.distance * tanFOVY * params.framing.y,
		params.distance
	);

	Vec3 trackingPosition = mTarget->GetTransform()->GetWorldPosition();
	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(params.yaw * degToRad, params.pitch * degToRad, 0);
	Vec3 position = trackingPosition - (rotation * localOffset);
	mOriginalPosition = position;

	transform->SetLocalPosition(position);
	transform->SetLocalRotation(rotation);
}

CameraParams CThirdPersonCamera::GetCameraParams(Vec3 trackingPosition)
{
	CameraParams result;
	auto transform = GetTransform();

	Vec3 position = mIsHit ? mOriginalPosition : transform->GetWorldPosition();
	Quaternion rotation = transform->GetLocalRotation();

	result.trackingPosition = trackingPosition;

	Vec3 eulerAngles = transform->GetLocalEulerAngles();
	result.pitch = eulerAngles.x;
	result.yaw = eulerAngles.y;

	// distance
	Vec3 toTrackingOff = trackingPosition - position;
	Vec3 fwd = rotation * -Vec3::Forward;
	result.distance = toTrackingOff.Dot(fwd);

	Vec3 toCameraOff = position - trackingPosition;
	Vec3 parallax = rotation.Inverse() * toCameraOff;
	float fov = mCamera->GetFov();
	float aspect = mCamera->GetAspect();
	float tanFOVY = tan(0.5f * XMConvertToRadians(fov));
	float tanFOVX = tanFOVY * aspect;
	Vec2 screenToWorld = result.distance * Vec2(tanFOVX, tanFOVY);
	result.framing.x = -parallax.x / screenToWorld.x;
	result.framing.y = -parallax.y / screenToWorld.y;

	if (result.framing.x > mDeadZoneSize.x && result.framing.x < mDeadZoneSize.y) {
		mCameraParams.framing.x = result.framing.x;
	}

	return result;
}

CameraBlend CThirdPersonCamera::ComputeBlendout()
{
	CameraBlend blend;
	CameraParams oldParams = GetCameraParams(mCameraParams.trackingPosition);

	blend.framing = oldParams.framing - mCameraParams.framing;
	blend.pitch = oldParams.pitch - mCameraParams.pitch;
	blend.yaw = oldParams.yaw - mCameraParams.yaw;
	blend.distance = oldParams.distance - mCameraParams.distance;

	if (blend.yaw > 180.f) { blend.yaw -= 360.f; }
	else if (blend.yaw < -180.f) { blend.yaw += 360.f; }

	return blend;
}

void CThirdPersonCamera::SetParamsBlended(CameraParams camParams, CameraBlend blendout, float progress)
{
	float multi = 1.f - progress;

	CameraParams result = camParams;
	result.framing += multi * blendout.framing;
	result.pitch += multi * blendout.pitch;
	result.yaw += multi * blendout.yaw;
	result.distance += multi * blendout.distance;

	SetCameraParams(result);
}

void CThirdPersonCamera::RaycastObjects()
{
	auto transform = GetTransform();
	Vec3 camPos = transform->GetWorldPosition();
	Vec3 targetPos = mCameraParams.trackingPosition;

	Ray ray;
	ray.position = targetPos;
	ray.direction = camPos - targetPos;
	float targetToCamDist = ray.direction.Length();
	ray.direction.Normalize();
	float minDistance = D3D12_FLOAT32_MAX;

	mIsHit = false;
	auto& objectList = CUR_SCENE->GetObjectsForType();
	auto& obstacles = objectList[OBJECT_TYPE::OBSTACLE];
	for (auto& obstacle : obstacles) {
		auto renderer = obstacle->GetRenderer();
		float dist = 0.f;
		if (renderer && renderer->IsIntersect(ray, dist)) {
			if (dist < targetToCamDist && dist < minDistance) {
				minDistance = dist;
				mIsHit = true;
			}
		}
	}

	if (!mIsHit) return;

	Vec3 hitPoint = ray.position + ray.direction * minDistance;
	transform->SetLocalPosition(hitPoint);
}
