#include "stdafx.h"
#include "TPCameraState.h"
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

void CFreeCameraState::Enter()
{
}

void CFreeCameraState::Exit()
{
}

void CFreeCameraState::Update()
{
}

void CFreeCameraState::HandleInput()
{
	auto transform = mCamera.lock()->GetTransform();
	Vec3 position = transform->GetWorldPosition();
	Vec3 forward = transform->GetWorldLook();
	Vec3 right = transform->GetWorldRight();
	Vec3 up = transform->GetWorldUp();
	float speed = 10.f * DELTA_TIME;
	Vec2 mouseDelta = INPUT.GetMouseDelta();

	Vec3 moveDir = Vec3::Zero;
	uint8_t dir = 0;
	if (INPUT.IsKeyPress(KEY_TYPE::W)) dir |= 0x08;
	if (INPUT.IsKeyPress(KEY_TYPE::S)) dir |= 0x02;
	if (INPUT.IsKeyPress(KEY_TYPE::D)) dir |= 0x01;
	if (INPUT.IsKeyPress(KEY_TYPE::A)) dir |= 0x04;
	if (INPUT.IsKeyPress(KEY_TYPE::Q)) dir |= KEY_FLAG::KEY_Q;
	if (INPUT.IsKeyPress(KEY_TYPE::E)) dir |= KEY_FLAG::KEY_E;
	if (INPUT.IsKeyPress(KEY_TYPE::SHIFT)) speed *= 2.f; // Speed up when space is pressed

	if (dir == 0 && mouseDelta == Vec2::Zero) {
		// No movement keys pressed, return early
		return;
	}

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

void CTargetCameraState::Enter()
{
}

void CTargetCameraState::Exit()
{
}

void CTargetCameraState::Update()
{
}

void CTargetCameraState::HandleInput()
{
}

void CCutsceneCameraState::Enter()
{
}

void CCutsceneCameraState::Exit()
{
}

void CCutsceneCameraState::Update()
{
}

void CCutsceneCameraState::HandleInput()
{
}
