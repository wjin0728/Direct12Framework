 #pragma once
#include "stdafx.h"
#include"Component.h"
#include"Mesh.h"
#include"AnimationEnums.h"

enum class ANIMATION_TYPE : UINT
{
    ONCE,
    LOOP,
    PINGPONG,
    END,

    end
};

enum class ANIMATION_BLEND_TYPE : UINT
{
    ADDITIVE,
    OVERRIDE,
    OVERRIDE_PASSTHROUGH,

    end
};

struct CALLBACKKEY
{
    float mTime = 0.0f;
    void* mCallbackData = NULL;

    void SetCallbackKey(float time, void* data) { mTime = time; mCallbackData = data; }
};

#define _WITH_ANIMATION_INTERPOLATION

class CAnimationCallbackHandler
{
public:
    CAnimationCallbackHandler() {}
    ~CAnimationCallbackHandler() {}

public:
    virtual void HandleCallback(void* pCallbackData, float fTrackPosition) {}
};

class CAnimationSet
{
public:
    CAnimationSet() {}
    CAnimationSet(float fLength, int nFramesPerSecond, int nKeyFrameTransforms, int nSkinningBones, string pstrName);
    ~CAnimationSet();

public:
    string						    	mAnimationSetName;

    float						    	mLength = 0.0f;
    int						    		mFramesPerSecond = 0; //m_fTicksPerSecond

    int								    mKeyFrames = 0;
    std::vector<float>                  mKeyFrameTimes{};
    std::vector<std::vector<Matrix>>    mKeyFrameTransforms{};

public:
    Matrix GetSRT(int nBone, float fPosition);
};

class CAnimationSets
{
public:
    CAnimationSets() {}
    CAnimationSets(int nAnimationSets);
    CAnimationSets(const CAnimationSets& other) = default;
    ~CAnimationSets();

private:
    int								mReferences = 0;

public:
    std::vector<std::shared_ptr<CAnimationSet>> mAnimationSet{};
    std::vector<std::weak_ptr<CTransform>>      mBoneFrameCaches{};
    std::vector<string>                         mBoneNames{};
};

class CAnimationTrack
{
public:
    CAnimationTrack() {}
    CAnimationTrack(const CAnimationTrack& other) = default;
    ~CAnimationTrack();

public:
    BOOL 							mEnable = true;
    float 							mSpeed = 1.0f;
    float 							mPosition = -ANIMATION_CALLBACK_EPSILON;
    float 							mWeight = 1.0f;

    int 							mSetIndex = 0; //AnimationSet Index

    ANIMATION_TYPE					mType = ANIMATION_TYPE::LOOP; //Once, Loop, PingPong

    std::vector<CALLBACKKEY>        mCallbackKeys{};

    std::shared_ptr<CAnimationCallbackHandler> mAnimationCallbackHandler = NULL;

public:
    void SetAnimationSet(int nAnimationSet) { mSetIndex = nAnimationSet; }

    void SetEnable(bool bEnable) { mEnable = bEnable; }
    void SetSpeed(float fSpeed) { mSpeed = fSpeed; }
    void SetWeight(float fWeight) { mWeight = fWeight; }
    void SetType(ANIMATION_TYPE fType) { mType = fType; }

    void SetPosition(float fPosition) { mPosition = fPosition; }
    float UpdatePosition(float fTrackPosition, float fTrackElapsedTime, float fAnimationLength);

    void SetCallbackKeys(int nCallbackKeys);
    void SetCallbackKey(int nKeyIndex, float fTime, void* pData);
    void SetAnimationCallbackHandler(std::shared_ptr<CAnimationCallbackHandler> pCallbackHandler);

    void HandleCallback();
};

class CAnimationController : public CComponent
{
public:
    CAnimationController();
    CAnimationController(int nAnimationTracks, std::shared_ptr<CAnimationSets>& sets, bool applyRootMotion = false);
    CAnimationController(const CAnimationController& other);
    virtual ~CAnimationController();

    virtual std::shared_ptr<CComponent> Clone() override { return std::make_shared<CAnimationController>(*this); }

    float mTime = 0.0f;

    std::vector<std::shared_ptr<CAnimationTrack>>   mTracks;
    std::shared_ptr<CAnimationSets>                 mAnimationSets;
    std::vector<Matrix>                             mBindPoseBoneOffsets{};
    std::vector<std::weak_ptr<CTransform>>          mSkinningBoneTransforms{};
    std::vector<Matrix>                             finalTransforms;

    UINT                                            mBoneTransformIdx = -1;

    void SetTrackAnimationSet(int trackIndex, int setIndex);

    void SetTrackEnabled(int trackIndex, bool enabled);
    void SetTrackPosition(int trackIndex, float position);
    void SetTrackSpeed(int trackIndex, float speed);
    void SetTrackWeight(int trackIndex, float weight);
    void SetTrackType(int trackIndex, ANIMATION_TYPE type);

    void SetCallbackKeys(int nAnimationTrack, int nCallbackKeys);
    void SetCallbackKey(int nAnimationTrack, int nKeyIndex, float fTime, void* pData);
    void SetAnimationCallbackHandler(int nAnimationTrack, std::shared_ptr<CAnimationCallbackHandler> pCallbackHandler);

public:
    virtual void Awake();
    virtual void Start();

    virtual void Update();
    virtual void LateUpdate();

    void AdvanceTime(float elapsedTime, std::shared_ptr<CGameObject>& rootGameObject);
    void BindSkinningMatrix();
    void PrepareSkinning();
    void UploadBoneOffsets();

public:
    bool                        mApplyRootMotion = false;
    std::weak_ptr<CTransform>   mModelRootObject;

    std::weak_ptr<CTransform>   mRootMotionObject;
    Vec3                        mFirstRootMotionPosition = Vec3(0.0f, 0.0f, 0.0f);

    void SetRootMotion(bool bRootMotion) { mApplyRootMotion = bRootMotion; }

    virtual void OnRootMotion(std::weak_ptr<CTransform> pRootGameObject) {}
    virtual void OnAnimationIK(std::weak_ptr<CTransform> pRootGameObject) {}

    void PrintMatrix(const Matrix& mat);

public:
    static std::unordered_map<PLAYER_STATE, ARCHER_ANIMATION> ARCHER_MAP;
    static std::unordered_map<PLAYER_STATE, FIGHTER_ANIMATION> FIGHTER_MAP;
    static std::unordered_map<PLAYER_STATE, MAGE_ANIMATION> MAGE_MAP;

    static std::unordered_map<PLAYER_STATE, GRASS_SMALL_ANIMATION> GRASS_SMALL_MAP;
    static std::unordered_map<PLAYER_STATE, GRASS_BIG_ANIMATION> GRASS_BIG_MAP;
    static std::unordered_map<PLAYER_STATE, FIRE_SMALL_ANIMATION> FIRE_SMALL_MAP;
    static std::unordered_map<PLAYER_STATE, FIRE_BIG_ANIMATION> FIRE_BIG_MAP;
    static std::unordered_map<PLAYER_STATE, WATER_SMALL_ANIMATION> WATER_SMALL_MAP;
    static std::unordered_map<PLAYER_STATE, WATER_BIG_ANIMATION> WATER_BIG_MAP;
};
