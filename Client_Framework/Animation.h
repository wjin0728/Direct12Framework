 #pragma once
#include "stdafx.h"
#include"Component.h"
#include"Mesh.h"
#include"AnimationEnums.h"

struct EventKey
{
    float mTime = 0.0f;
    std::string mName;
    bool mEnable = true;

    EventKey(float time, const std::string& name) : mTime(time), mName(name) {}
};

#define _WITH_ANIMATION_INTERPOLATION

class CAnimationEventHandler
{
public:
    using Event = std::function<void(float)>;

    void Register(const std::string& name, Event event) {
        mEvents[name] = std::move(event);
    }

    Event GetEvent(const std::string& name) const {
        auto it = mEvents.find(name);
        if (it != mEvents.end()) return it->second;
        return nullptr;
    }

private:
    std::unordered_map<std::string, Event> mEvents;
};

class CAnimationSet
{
public:
    CAnimationSet() {}
	CAnimationSet(const CAnimationSet& other) = default;
    CAnimationSet(float fLength, int nFramesPerSecond, int nKeyFrameTransforms, int nSkinningBones, string pstrName);
    ~CAnimationSet();

public:
    string						    	mAnimationName;

    float						    	mLength = 0.0f;
    int						    		mFramesPerSecond = 0; //m_fTicksPerSecond
    ANIMATION_TYPE					    mType = ANIMATION_TYPE::LOOP; //Once, Loop, PingPong

    std::vector<std::shared_ptr<EventKey>>   mEventKeys;

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
    CAnimationSets(const CAnimationSets& other);
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
    CAnimationTrack(const CAnimationTrack& other);
    ~CAnimationTrack();

public:
    ANIMATION_TYPE					    mType = ANIMATION_TYPE::LOOP; //Once, Loop, PingPong
    BOOL 					    		mEnable = true;
    float 						    	mSpeed = 1.0f;
    float 							    mPosition = -ANIMATION_CALLBACK_EPSILON;
    float 						    	mWeight = 1.0f;
	float                               mTrackProgress = 0.0f; //0.0f ~ 1.0f

    std::vector<shared_ptr<EventKey>>   mEventKeys;

    int 							    mSetIndex = 0; //AnimationSet Index

public:

	void SetIndex(int nSetIndex) { mSetIndex = nSetIndex; }
    void SetEnable(bool bEnable) { mEnable = bEnable; }
    void SetSpeed(float fSpeed) { mSpeed = fSpeed; }
    void SetWeight(float fWeight) { mWeight = fWeight; }
    void SetType(ANIMATION_TYPE fType) { mType = fType; }
    void SetPosition(float fPosition) { mPosition = fPosition; }
	void SetEventEnableTrue() { for (auto& key : mEventKeys) key->mEnable = true; }

    void SetAnimationSet(std::shared_ptr<CAnimationSet>& set);

    float UpdatePosition(float fTrackPosition, float fTrackElapsedTime, float fAnimationLength);

    void HandleCallback(std::shared_ptr<CAnimationEventHandler>& registry);
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
    std::vector<std::weak_ptr<CTransform>>          mSkinningBoneTransforms{};
    std::vector<Matrix>                             finalTransforms;
    std::unordered_map<string, std::shared_ptr<CAnimationEventHandler>>         mEventHandler;

    UINT                                            mBoneTransformIdx = -1;

    void SetTrackAnimationSet(int trackIndex, int setIndex);

    void SetTrackEnabled(int trackIndex, bool enabled) { if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetEnable(enabled); }
    void SetTrackPosition(int trackIndex, float position) { if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetPosition(position); }
    void SetTrackSpeed(int trackIndex, float speed) { if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetSpeed(speed); }
    void SetTrackWeight(int trackIndex, float weight) { if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetWeight(weight); }
    void SetTrackType(int trackIndex, ANIMATION_TYPE type) { if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetType(type); }

public:
    virtual void Awake();
    virtual void Start();

    virtual void Update();
    virtual void LateUpdate();

    void BindSkinningMatrix();
    void PrepareSkinning();

public:
    bool                        mApplyRootMotion = false;
    std::weak_ptr<CTransform>   mModelRootObject;

    std::weak_ptr<CTransform>   mRootMotionObject;
    Vec3                        mFirstRootMotionPosition = Vec3(0.0f, 0.0f, 0.0f);
    
    void SetRootMotion(bool bRootMotion) { mApplyRootMotion = bRootMotion; }

    void PrintMatrix(const Matrix& mat);
};
