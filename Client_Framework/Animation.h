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

struct CALLBACKKEY
{
    float  							m_fTime = 0.0f;
    void* m_pCallbackData = NULL;
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
    CAnimationSet(float fLength, int nFramesPerSecond, int nKeyFrameTransforms, int nSkinningBones, char* pstrName);
    ~CAnimationSet();

public:
    char							m_pstrAnimationSetName[64];

    float							m_fLength = 0.0f;
    int								m_nFramesPerSecond = 0; //m_fTicksPerSecond

    int								m_nKeyFrames = 0;
    float* m_pfKeyFrameTimes = NULL;
    XMFLOAT4X4** m_ppxmf4x4KeyFrameTransforms = NULL;

#ifdef _WITH_ANIMATION_SRT
    int								m_nKeyFrameScales = 0;
    float* m_pfKeyFrameScaleTimes = NULL;
    XMFLOAT3** m_ppxmf3KeyFrameScales = NULL;
    int								m_nKeyFrameRotations = 0;
    float* m_pfKeyFrameRotationTimes = NULL;
    XMFLOAT4** m_ppxmf4KeyFrameRotations = NULL;
    int								m_nKeyFrameTranslations = 0;
    float* m_pfKeyFrameTranslationTimes = NULL;
    XMFLOAT3** m_ppxmf3KeyFrameTranslations = NULL;
#endif

public:
    XMFLOAT4X4 GetSRT(int nBone, float fPosition);
};

class CAnimationSets
{
public:
    CAnimationSets(int nAnimationSets);
    ~CAnimationSets();

private:
    int								m_nReferences = 0;

public:
    void AddRef() { m_nReferences++; }
    void Release() { if (--m_nReferences <= 0) delete this; }

public:
    int								m_nAnimationSets = 0;
    CAnimationSet** m_pAnimationSets = NULL;

    int								m_nBoneFrames = 0;
    CGameObject** m_ppBoneFrameCaches = NULL; //[m_nBoneFrames]
};

class CAnimationTrack
{
public:
    CAnimationTrack() {}
    ~CAnimationTrack();

public:
    BOOL 							m_bEnable = true;
    float 							m_fSpeed = 1.0f;
    float 							m_fPosition = -ANIMATION_CALLBACK_EPSILON;
    float 							m_fWeight = 1.0f;

    int 							m_nAnimationSet = 0; //AnimationSet Index

    ANIMATION_TYPE					m_nType = ANIMATION_TYPE::LOOP; //Once, Loop, PingPong

    int 							m_nCallbackKeys = 0;
    CALLBACKKEY* m_pCallbackKeys = NULL;

    CAnimationCallbackHandler* m_pAnimationCallbackHandler = NULL;

public:
    void SetAnimationSet(int nAnimationSet) { m_nAnimationSet = nAnimationSet; }

    void SetEnable(bool bEnable) { m_bEnable = bEnable; }
    void SetSpeed(float fSpeed) { m_fSpeed = fSpeed; }
    void SetWeight(float fWeight) { m_fWeight = fWeight; }

    void SetPosition(float fPosition) { m_fPosition = fPosition; }
    float UpdatePosition(float fTrackPosition, float fTrackElapsedTime, float fAnimationLength);

    void SetCallbackKeys(int nCallbackKeys);
    void SetCallbackKey(int nKeyIndex, float fTime, void* pData);
    void SetAnimationCallbackHandler(CAnimationCallbackHandler* pCallbackHandler);

    void HandleCallback();
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
    std::vector<std::weak_ptr<CTransform>> mBoneCaches{};
	std::weak_ptr<CTransform> mRootTransform{};
    std::vector<Matrix> mBindPoseBoneOffsets{};
    std::vector<Matrix> finalTransforms;

    UINT mBoneTransformIdx = -1;

    void SetTrackAnimationSet(int trackIndex, int setIndex);
    void SetTrackEnabled(int trackIndex, bool enabled);
    void SetTrackPosition(int trackIndex, float position);
    void SetTrackSpeed(int trackIndex, float speed);
    void SetTrackWeight(int trackIndex, float weight);
    void SetTrackStartEndTime(int trackIndex, float start, float end);

    void SetAnimationType(std::shared_ptr<CAnimationSet>& animationSet, ANIMATION_TYPE type);

    void SetAnimationCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr <CAnimationCallbackHandler>& callbackHandler);

public:
    virtual void Awake();
    virtual void Start();

    virtual void Update();
    virtual void LateUpdate();

    void AdvanceTime(float elapsedTime, std::shared_ptr<CGameObject>& rootGameObject);
    void BindSkinningMatrix();
    void PrepareSkinning();
    void UploadBoneOffsets();
};
