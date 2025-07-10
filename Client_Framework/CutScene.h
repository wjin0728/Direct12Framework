#pragma once
#pragma once
#include "MonoBehaviour.h"

struct CutSceneFrame
{
    Vec3 position;
    Quaternion rotation;
};

class CCutScene : public CMonoBehaviour
{
private:
    friend class CThirdPersonCamera;

    float mElapsed = 0.0f;
    float mDuration = 2.5f; // ÄÆ½Å Áö¼Ó ½Ã°£
    bool mIsPlaying = false;

    std::map<float, CutSceneFrame> mKeyFrames;
    std::shared_ptr<CThirdPersonCamera> mThirdPersonCamera{};

public:
    CCutScene() : CMonoBehaviour("CutScene") {}
    virtual ~CCutScene() = default;

    virtual void Start() override;
    virtual void Update() override;

    void SetThirdPersonCamera(const std::shared_ptr<CThirdPersonCamera>& cam) { mThirdPersonCamera = cam; }
    bool GetEnable() const { return mIsPlaying; }

    void PlayCutScene();
    void InitializeCutSceneFrames();

    Matrix GetPositionFromRelative(const Vec3& relativePosition, const Quaternion& relativeRot);
    Vec3 MoveAlongViewDirection(const Vec3& position, const Quaternion& rotation, float distance);
};
