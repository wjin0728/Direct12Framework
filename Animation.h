#pragma once
#include "stdafx.h"
#include"Component.h"
#include"Mesh.h"

enum class ANIMATION_TYPE : UINT
{
    ONCE,
    LOOP,
    PINGPONG,

    END
};

enum class ANIMATION_BLEND_TYPE : UINT
{
    ADDITIVE,
    OVERRIDE,
    OVERRIDE_PASSTHROUGH,

    END
};

struct CallbackKey 
{
    float mTime = 0.0f;
    void* mData = nullptr;

    void SetCallbackKey(float time, void* data)
    {
        mTime = time;
        mData = data;
    }
};

class CCallbackHandler 
{
public:
    CCallbackHandler() {}
    ~CCallbackHandler() {}

public:
    virtual void HandleCallback(void* callbackData, float trackPosition) {}
};

class CAnimationCurve 
{
public:
    CAnimationCurve(int keyNum);
    ~CAnimationCurve();

    std::vector<float> mKeyTimes{};
    std::vector<float> mKeyValues{};

    float GetInterpolatedValue(const float position);
};

class CAnimationLayer
{
public:
    CAnimationLayer();
    ~CAnimationLayer();

    float mWeight = 1.0f;
    ANIMATION_BLEND_TYPE mBlendMode = ANIMATION_BLEND_TYPE::OVERRIDE;

    std::vector<std::array<std::shared_ptr<CAnimationCurve>, 9>> mAnimationCurves;
    std::vector<std::string> mBoneNames{};
    std::vector<std::weak_ptr<CTransform>> mBoneFrameCaches{};

    void LoadKeyValues(int boneFrame, int curve, std::ifstream& file);
    void GetSRT(const std::array<std::shared_ptr<CAnimationCurve>, 9>& curves, float position, Vec3& scale, Vec3& rotation, Vec3& translation);
};

class CAnimationSet 
{
public:
    CAnimationSet(float startTime, float endTime, const std::string& name);
    ~CAnimationSet();

    std::string mName;

    float mStartTime = 0.0f;
    float mEndTime = 0.0f;
    float mLength = 0.0f;

    float mPosition = 0.0f;
    ANIMATION_TYPE mType = ANIMATION_TYPE::LOOP;

    std::vector<std::shared_ptr<CAnimationLayer>> mLayers;
    std::vector<std::string> mBoneNames{};
    //std::vector<std::weak_ptr<CTransform>> mBoneFrameCaches{};

    std::vector<std::vector<Vec3>> mScales;
    std::vector<std::vector<Vec3>> mRotations;
    std::vector<std::vector<Vec3>> mTranslations;

    std::vector<CallbackKey> mCallbackKeys;
    std::shared_ptr<CCallbackHandler> mCallbackHandler;

    float UpdatePosition(float position, float start, float end);

    void Animate(float trackPos, float trackWeight, float start, float end, bool override);

    void SetPosition(float position) { mPosition = position; }
    void SetAnimationType(ANIMATION_TYPE type) { mType = type; }

    void SetCallbackHandler(std::shared_ptr<CCallbackHandler>& callbackHandler);

    void HandleCallback();
};

class CAnimationSets 
{
public:
    CAnimationSets(int setsNum);
    ~CAnimationSets();

    std::vector<std::shared_ptr<CAnimationSet>> mAnimationSet{};
    std::vector<std::string> mBoneNames{};
    std::shared_ptr<CTransform> mSkinnedMeshCache{};

public:
    void SetCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr<CCallbackHandler>& callbackHandler);
};

class CAnimationTrack
{
public:
    CAnimationTrack() {}
    ~CAnimationTrack() {}

public:
    bool mEnabled = true;
    float mSpeed = 1.0f;
    float mPosition = 0.0f;
    float mWeight = 1.0f;

    int mAnimationSetIndex = 0;

    float mStartTime = 0.0f;
    float mEndTime = 0.0f;
    float mLength = 0.0f;

    void SetAnimationSet(int index) { mAnimationSetIndex = index; }

    void SetEnable(bool enable) { mEnabled = enable; }
    void SetSpeed(float s) { mSpeed = s; }
    void SetWeight(float w) { mWeight = w; }
    void SetPosition(float p) { mPosition = p; }

    void SetStartEndTime(float start, float end) { mStartTime = start; mEndTime = end; mLength = end - start; }
};

class CAnimationController : public CComponent
{
public:
    CAnimationController();
    CAnimationController(std::shared_ptr<CAnimationSets>& sets, bool applyRootMotion = false);
    virtual ~CAnimationController();

    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CAnimationController>(*this); }

    float mTime = 0.0f;
    bool mApplyRootMotion = false;

    std::vector<std::shared_ptr<CAnimationTrack>> mTracks;
    std::shared_ptr<CAnimationSets> mAnimationSets;

    UINT mBoneTransformIdx = -1;

    void SetTrackAnimationSet(int trackIndex, int setIndex);
    void SetTrackEnabled(int trackIndex, bool enabled);
    void SetTrackPosition(int trackIndex, float position);
    void SetTrackSpeed(int trackIndex, float speed);
    void SetTrackWeight(int trackIndex, float weight);
    void SetTrackStartEndTime(int trackIndex, float start, float end);

    void SetAnimationType(std::shared_ptr<CAnimationSet>& animationSet, ANIMATION_TYPE type);

    void SetAnimationCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr <CCallbackHandler>& callbackHandler);

public:
    virtual void Awake();
    virtual void Start();

    virtual void Update();
    virtual void LateUpdate();

    void AdvanceTime(float elapsedTime, std::shared_ptr<CGameObject>& rootGameObject);
    void UpdateShaderVariables();
    void PrepareSkinning();
    void UploadBoneOffsets();
};
