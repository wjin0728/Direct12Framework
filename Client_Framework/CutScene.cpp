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
    auto& transform = mThirdPersonCamera->GetTransform();

    //camWorld = GetPositionFromRelative(Vec3{ 1.21935, 0.499366, 0.68519 }, Quaternion{ 0.0381727, 0.85889, 0.0647156, -0.506619 });
    //camWorld.Decompose(scale, rotation, translation);

    //mKeyFrames[0.0f] = {
    //    translation,
    //    rotation
    //};

    //mKeyFrames[1.00128579] = {
    //    translation,
    //    rotation
    //};

    //mKeyFrames[1.16748214] = {
    //    translation,
    //    rotation
    //};


    //mKeyFrames[2.499999] = {
    //    translation,
    //    rotation
    //};
    //
    //mKeyFrames[2.5f] = {
    //transform->GetLocalPosition(),
    //transform->GetLocalRotation()
    //};
}

void CCutScene::InitializeMageCutScene()
{
    mKeyFrames.clear();
    Vec3 scale, translation;
    Quaternion rotation;
    Matrix camWorld;
    auto& transform = mThirdPersonCamera->GetTransform();

    mKeyFrames[2.5f] = {
        transform->GetLocalPosition(),
        transform->GetLocalRotation()
    };
}

void CCutScene::InitializeArcherCutScene()
{
    mKeyFrames.clear();
    Vec3 scale, translation;
    Quaternion rotation;
    Matrix camWorld;
    auto& transform = mThirdPersonCamera->GetTransform();

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