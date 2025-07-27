#include "stdafx.h"
#include "CutScene.h"
#include "Camera.h"
#include "Transform.h"
#include "Timer.h"
#include "PlayerController.h"
#include "ThirdPersonCamera.h"

void CCutScene::Start()
{
}

void CCutScene::PlayCutScene()
{
    if (!mThirdPersonCamera) return;

    mElapsed = 0.0f;
    mIsPlaying = true;
    mThirdPersonCamera->SetPlayingCutScene(true);
    mThirdPersonCamera->SetCanRotate(false);

    switch (mClass) {
    case PLAYER_CLASS::ARCHER:
        InitializeArcherCutScene();
        break;
    case PLAYER_CLASS::FIGHTER:
        InitializeFighterCutScene();
        break;
    case PLAYER_CLASS::MAGE:
        InitializeMageCutScene();
        break;
    }
}

void CCutScene::InitializeFighterCutScene()
{
    mKeyFrames.clear();
    Vec3 scale, translation;
    Quaternion rotation;
    Matrix camWorld;
    auto transform = mThirdPersonCamera->GetTransform();

    camWorld = GetPositionFromRelative(Vec3{ 1.27789, 1.11734, 1.92968 }, Quaternion{ -0.0333761, 0.94699, -0.104699, -0.301884 });
    camWorld.Decompose(scale, rotation, translation);

    mKeyFrames[0.0f] = {
        translation,
        rotation
    };
    mKeyFrames[1.3] = {
        translation,
        rotation
    };

    camWorld = GetPositionFromRelative(Vec3{ 1.56613, 0.00875664, 3.6023 }, Quaternion{ 0.0177714, 0.968301, 0.072159, -0.238475 });
    camWorld.Decompose(scale, rotation, translation);
    translation.y += 1.f;

    mKeyFrames[1.30001f] = {
        translation,
        rotation
    };

    mKeyFrames[1.9f] = {
        translation,
        rotation
    };

    camWorld = GetPositionFromRelative(Vec3{ 2.86335, 1.95796, -1.62943 }, Quaternion{ 0.165736, -0.449358, 0.0852413, 0.873695 });
    camWorld.Decompose(scale, rotation, translation);

    mKeyFrames[1.900001f] = {
        translation,
        rotation
    };
    
    mKeyFrames[2.5f] = {
        transform->GetLocalPosition(),
        transform->GetLocalRotation()
    };
}

void CCutScene::InitializeMageCutScene()
{
    mKeyFrames.clear();
    Vec3 scale, translation;
    Quaternion rotation;
    Matrix camWorld;
    auto transform = mThirdPersonCamera->GetTransform();

    camWorld = GetPositionFromRelative(Vec3{ 0.220673, 0.71169, 2.19035 }, Quaternion{ 0.00203271, 0.997964, 0.0437918, -0.0463231 });
    camWorld.Decompose(scale, rotation, translation);

    mKeyFrames[0.0f] = {
        MoveAlongViewDirection(translation, rotation, -2.0f),
        rotation
    };

    mKeyFrames[0.55] = {
        MoveAlongViewDirection(translation, rotation, -1.85f),
        rotation
    };

    mKeyFrames[0.7] = {
        MoveAlongViewDirection(translation, rotation, -0.15f),
        rotation
    };
    
    mKeyFrames[1.5] = {
        translation,
        rotation
    };

    camWorld = GetPositionFromRelative(Vec3{ -4.46388, 3.08531, -1.89764 }, Quaternion{ 0.139074, 0.500665, -0.0818722, 0.850465 });
    camWorld.Decompose(scale, rotation, translation);

    mKeyFrames[1.50001f] = {
        translation,
        rotation
    };

    mKeyFrames[2.5f] = {
        translation,
        rotation
    };
}

void CCutScene::InitializeArcherCutScene()
{
    mKeyFrames.clear();
    Vec3 scale, translation;
    Quaternion rotation;
    Matrix camWorld;
    auto transform = mThirdPersonCamera->GetTransform();

    camWorld = GetPositionFromRelative(Vec3{ -0.0274296, 0.581093, 4.65343 }, Quaternion{ 0.0038204, 0.984563, 0.173641, -0.0216622 });
    camWorld.Decompose(scale, rotation, translation);

    mKeyFrames[0.0f] = {
        MoveAlongViewDirection(translation, rotation, 2.5f),
        rotation
    };

    mKeyFrames[0.1f] = {
        MoveAlongViewDirection(translation, rotation, 2.2f),
        rotation
    };

    mKeyFrames[0.602212369] = {
        MoveAlongViewDirection(translation, rotation, 1.5f),
        rotation
    };

    mKeyFrames[1.3999f] = {
        MoveAlongViewDirection(translation, rotation, 1.2f),
        rotation
    };

    camWorld = GetPositionFromRelative(Vec3{ 1.23505, 3.43635, -1.69316 }, Quaternion{ 0.178597, -0.207647, 0.0385907, 0.960987 });
    camWorld.Decompose(scale, rotation, translation);

    mKeyFrames[1.4f] = {
        translation,
        rotation
    };

    mKeyFrames[2.5f] = {
        transform->GetLocalPosition(),
        transform->GetLocalRotation()
    };
}

void CCutScene::Update()
{
    if (!mIsPlaying || mKeyFrames.size() < 2) return;

    mElapsed += DELTA_TIME;

    if (mElapsed >= mDuration) {
        mIsPlaying = false;
        mThirdPersonCamera->SetPlayingCutScene(false);
        mThirdPersonCamera->SetCanRotate(true);
        return;
    }

    auto itUpper = mKeyFrames.upper_bound(mElapsed);
    if (itUpper == mKeyFrames.begin()) return;
    auto itLower = std::prev(itUpper);

    float t0 = itLower->first;
    float t1 = itUpper->first;
    float localT = (mElapsed - t0) / (t1 - t0);

    const CutSceneFrame& start = itLower->second;
    const CutSceneFrame& end = itUpper->second;

    Vec3 pos = Vec3::Lerp(start.position, end.position, localT);
    Quaternion rot = Quaternion::Slerp(start.rotation, end.rotation, localT);

    if (auto transform = mThirdPersonCamera->GetTransform()) {
        transform->SetLocalPosition(pos);
        transform->SetLocalRotation(rot);
    }
}

inline Matrix CCutScene::GetPositionFromRelative(const Vec3& relativePosition, const Quaternion& relativeRot)
{
    auto targetTransform = mThirdPersonCamera->GetTarget()->GetTransform();

    Matrix relMat = Matrix::CreateFromQuaternion(relativeRot);
    relMat._41 = relativePosition.x;
    relMat._42 = relativePosition.y;
    relMat._43 = relativePosition.z;

    Matrix targetWorld = targetTransform->GetWorldMat();

    return relMat * targetWorld;
}

inline Vec3 CCutScene::MoveAlongViewDirection(const Vec3& position, const Quaternion& rotation, float distance)
{
    Vec3 forward = Vec3::TransformNormal(Vec3{ 0.0f, 0.0f, 1.0f }, Matrix::CreateFromQuaternion(rotation));
    forward.Normalize();
    return position + forward * distance;
}