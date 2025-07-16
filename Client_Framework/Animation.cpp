#include "stdafx.h"
#include "Animation.h"
#include "Mesh.h"
#include "Transform.h"
#include "SkinnedMesh.h"
#include "Timer.h"
#include "SkinnedMeshRenderer.h"
#include "ObjectPoolManager.h"
#include "AnimationEnums.h"
#include "InputManager.h"
#include "ServerManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAnimationSet::CAnimationSet(float length, int framesPerSecond, int keyFrameNum, int boneNum, string name)
{
	mLength = length;
	mFramesPerSecond = framesPerSecond;
	mKeyFrames = keyFrameNum;

	mAnimationName = name;

	mKeyFrameTimes.resize(keyFrameNum);
	mKeyFrameTransforms.resize(keyFrameNum);
	for (auto& frame : mKeyFrameTransforms) frame.resize(boneNum);
}

CAnimationSet::~CAnimationSet()
{
}

Matrix CAnimationSet::GetSRT(int boneIndex, float position)
{
	Matrix transform = Matrix::Identity;
	for (int i = 0; i < (mKeyFrames - 1); i++) {
		if ((mKeyFrameTimes[i] <= position) && (position < mKeyFrameTimes[i + 1])) {
			float t = (position - mKeyFrameTimes[i]) / (mKeyFrameTimes[i + 1] - mKeyFrameTimes[i]);
			transform = Matrix::Interpolate(mKeyFrameTransforms[i][boneIndex], mKeyFrameTransforms[i + 1][boneIndex], t);
			break;
		}
	}
	if (position >= mKeyFrameTimes[mKeyFrames - 1]) transform = mKeyFrameTransforms[mKeyFrames - 1][boneIndex];

	return(transform);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSets::CAnimationSets(int setNum)
{
	mAnimationSet.resize(setNum);
	for (auto& set : mAnimationSet) set = std::make_shared<CAnimationSet>();
}

CAnimationSets::CAnimationSets(const CAnimationSets& other)
{
	mAnimationSet.resize(other.mAnimationSet.size());
	for (int i = 0; i < mAnimationSet.size(); ++i) {
		if (other.mAnimationSet[i]) {
			mAnimationSet[i] = std::make_shared<CAnimationSet>(*other.mAnimationSet[i]);
		}
	}
	mBoneNames = other.mBoneNames;
	mBoneFrameCaches.resize(other.mBoneFrameCaches.size());
}

CAnimationSets::~CAnimationSets()
{
}

CAnimationTrack::CAnimationTrack(const CAnimationTrack& other)
{
	mSetIndex = other.mSetIndex;
	mPosition = other.mPosition;
	mSpeed = other.mSpeed;
	mWeight = other.mWeight;
	mType = other.mType;
	mEnable = other.mEnable;
	mEventKeys = other.mEventKeys;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationTrack::~CAnimationTrack()
{
}

void CAnimationTrack::HandleCallback(std::shared_ptr<CAnimationEventHandler>& registry)
{
	for (auto& key : mEventKeys) {
		if (key->mTime < mPosition && key->mEnable) {
			auto event = registry->GetEvent(key->mName);
			if (event) {
				event(mPosition);
				key->mEnable = false;
			}
			break;
		}
	}
}

void CAnimationTrack::SetAnimationSet(std::shared_ptr<CAnimationSet>& set)
{
	if (set) {
		mPosition = -ANIMATION_CALLBACK_EPSILON;
		mSpeed = 1.0f;
		mWeight = 1.0f;
		mType = set->mType;
		mEnable = true;
		mTrackProgress = 0.0f;
		mEventKeys.resize(set->mEventKeys.size());
		mEventSegments.clear();

		EventSegment data;
		for (int i = 0; auto& key : mEventKeys) {
			if (set->mEventKeys[i]) key = set->mEventKeys[i];

			// 슬로우 모션
			if (key->mName == "SlowStart") {
				data.mType = ANIMATION_EVENT_TYPE::SLOW;
				data.mStart = key->mTime;
			}
			else if (key->mName == "SlowEnd") {
				data.mEnd = key->mTime;
				mEventSegments.push_back(data);
			}

			// 점프
			if (key->mName == "JumpStart") {
				data.mType = ANIMATION_EVENT_TYPE::JUMP;
				data.mStart = key->mTime;
			}
			else if (key->mName == "JumpEnd") {
				data.mEnd = key->mTime;
				mEventSegments.push_back(data);
			}

			key->mEnable = true;
			++i;
		}
	}
}

float CAnimationTrack::UpdatePosition(float trackPosition, float elapsedTime, float animationLength)
{
	if (INPUT.IsKeyDown(KEY_TYPE::F12)) mEnable = !mEnable;
	if (!mEnable) return(mPosition);

	for (auto& data : mEventSegments) {
		if (mPosition < data.mStart || mPosition > data.mEnd)
			continue;

		switch (data.mType) {
		case ANIMATION_EVENT_TYPE::SLOW: {
			mPosition = trackPosition + elapsedTime * data.mData;
			mTrackProgress = mPosition / animationLength;
			return(mPosition);
			break;
		}
		case ANIMATION_EVENT_TYPE::JUMP: {
			float duration = data.mEnd - data.mStart;
			float elapsed = mPosition - data.mStart;
			float t = elapsed / duration;
			float heightFactor = -4.0f * t * (t - 1.0f);

			data.mData * heightFactor;
		}
		}
	}

	float trackElapsedTime = elapsedTime * mSpeed;

	switch (mType) {
	case ANIMATION_TYPE::LOOP: {
		if (mPosition < 0.0f) mPosition = 0.0f;
		else {
			mPosition = trackPosition + trackElapsedTime;
			if (mPosition > animationLength) {
				mPosition = -ANIMATION_CALLBACK_EPSILON;
				SetEventEnableTrue();
				return(animationLength);
			}
		}
		//			mPosition = fmod(trackPosition, m_pfKeyFrameTimes[m_nKeyFrames-1]); // mPosition = trackPosition - int(trackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			mPosition = fmod(trackPosition, m_fLength); //if (mPosition < 0) mPosition += m_fLength;
		//			mPosition = trackPosition - int(trackPosition / m_fLength) * m_fLength;
		break;
	}
	case ANIMATION_TYPE::ONCE:
		if (mPosition < 0.0f) mPosition = 0.0f;
		mPosition = trackPosition + trackElapsedTime;
		if (mPosition > animationLength) {
			mPosition = 0.0f;
			mType = ANIMATION_TYPE::END;
		}
		break;
	case ANIMATION_TYPE::PINGPONG:
		break;
	}

	mTrackProgress = mPosition / animationLength;

	return(mPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController()
{
	mTrack = std::make_unique<CAnimationTrack>();
}

CAnimationController::CAnimationController(std::shared_ptr<CAnimationSets>& sets, bool applyRootMotion)
{
	mTrack = std::make_unique<CAnimationTrack>();
	mApplyRootMotion = applyRootMotion;
	mAnimationSets = sets;
}

CAnimationController::CAnimationController(const CAnimationController& other) : CComponent(other)
{
	mTrack = std::make_unique<CAnimationTrack>(*other.mTrack.get());
	mAnimationSets = std::make_shared<CAnimationSets>(*other.mAnimationSets);
	finalTransforms.resize(other.finalTransforms.size());
	mBoneTransformIdx = other.mBoneTransformIdx;
	mApplyRootMotion = other.mApplyRootMotion;
	mModelRootObject.reset();
	mRootMotionObject.reset();
	mFirstRootMotionPosition = other.mFirstRootMotionPosition;
	mBoneTransformIdx = -1; // Reset to default value
}

CAnimationController::~CAnimationController()
{
}

void CAnimationController::Awake()
{
	
}

void CAnimationController::Start()
{
	auto skinnedMeshRenderer = owner->GetComponentFromHierarchy<CSkinnedMeshRenderer>();
	if (!skinnedMeshRenderer) {
		// If no skinned mesh renderer is found, we cannot proceed with animation setup
		return;
	}
	
	auto& boneNames = skinnedMeshRenderer->mBoneNames;
	mSkinningBoneTransforms.resize(boneNames.size());
	finalTransforms.resize(boneNames.size());

	std::unordered_map<std::string, std::weak_ptr<CTransform>> boneMap;
	for (int i = 0; i < boneNames.size(); ++i) {
		auto bone = owner->FindChildByName(boneNames[i]);
		if (bone) {
			boneMap[boneNames[i]] = bone->GetTransform();
			mSkinningBoneTransforms[i] = bone->GetTransform();
		}
	}
	mRootMotionObject = boneMap[boneNames[0]];

	if (mBoneTransformIdx == -1) {
		mBoneTransformIdx = INSTANCE(CObjectPoolManager).GetBoneTransformIdx();
	}

	if (mAnimationSets) {
		for (int i = 0; auto& cache : mAnimationSets->mBoneFrameCaches) {
			auto& boneName = mAnimationSets->mBoneNames[i];
			if (boneMap.contains(boneName)) cache = boneMap[boneName];
			else {
				auto object = owner->FindChildByName(boneName);
				if (object) cache = object->GetTransform();
			}
			++i;
		}
		SetTrackAnimationSet(0);
	}

	mRootMotionObject.lock()->owner->SetStatic(true);
}

void CAnimationController::Update()
{
}

void CAnimationController::PrepareSkinning()
{
}

void CAnimationController::LateUpdate()
{
	float deltaTime = DELTA_TIME;
	mTime += deltaTime;

	if (mTrack) {
		for (auto& cache : mAnimationSets->mBoneFrameCaches) { if (cache.lock()) cache.lock()->SetLocalMatZero(); }
	
		auto& set = mAnimationSets->mAnimationSet[mTrack->mSetIndex];
		float position = mTrack->UpdatePosition(mTrack->mPosition, deltaTime, set->mLength);

		for (int i = 0; auto& cache : mAnimationSets->mBoneFrameCaches) {
			if (cache.lock()) {
				Matrix transform = set->GetSRT(i, position);
				transform *= mTrack->mWeight;
				cache.lock()->mLocalMat = transform;
			}
			++i;
		}

		if (mEventHandler.contains(set->mAnimationName))
			mTrack->HandleCallback(mEventHandler[set->mAnimationName]);
		GetOwner()->UpdateWorldMatrices(nullptr);
	}

	for (int i = 0; auto & cache : mSkinningBoneTransforms) {
		Matrix boneTransform = cache.lock()->GetWorldMat(false);
		finalTransforms[i] = boneTransform.Transpose();
		i++;
	}

	UINT offset = mBoneTransformIdx * ALIGNED_SIZE(sizeof(Matrix) * SKINNED_ANIMATION_BONES);
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)->UpdateBuffer(
		offset,
		finalTransforms.data(),
		sizeof(Matrix) * finalTransforms.size()
	);
}

void CAnimationController::SetTrackAnimationSet(int setIndex)
{
	if (mTrack && setIndex < mAnimationSets->mAnimationSet.size() && mTrack->mSetIndex != setIndex) {
		mTrack->SetIndex(setIndex);
		mTrack->SetAnimationSet(mAnimationSets->mAnimationSet[setIndex]);
	}
}

void CAnimationController::BindSkinningMatrix()
{
	if (mBoneTransformIdx == -1) {
		return; // If no bone transform index is set, we cannot bind the skinning matrix
	}
	UINT offset = mBoneTransformIdx * ALIGNED_SIZE(sizeof(Matrix) * SKINNED_ANIMATION_BONES);
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)->BindToShader(offset);
}

void CAnimationController::PrintMatrix(const Matrix& mat)
{
}

void CAnimationController::AddAnimationEvent(const std::string& animName, const std::string& name, CAnimationEventHandler::Event event)
{
	for (auto& set : mAnimationSets->mAnimationSet) {
		if (set->mAnimationName != animName) continue;

		std::shared_ptr<CAnimationEventHandler> handler{};
		if (mEventHandler.contains(set->mAnimationName)) {
			handler = mEventHandler[set->mAnimationName];
		}
		else {
			handler = std::make_shared<CAnimationEventHandler>();
			mEventHandler[set->mAnimationName] = handler;
		}
		for (auto& key : set->mEventKeys) {
			handler->Register(name, event);
			if (set->mAnimationName == "Attack" || set->mAnimationName == "RunAttack") {
				handler->Register("Attack", [](float time) {
					INSTANCE(ServerManager).send_cs_attack_packet();
					});
				handler->Register(name, event);
			}
		}
	}
}
