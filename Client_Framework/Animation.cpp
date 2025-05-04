#include "stdafx.h"
#include "Animation.h"
#include "Mesh.h"
#include "Transform.h"
#include "SkinnedMesh.h"
#include "Timer.h"
#include "SkinnedMeshRenderer.h"
#include "ObjectPoolManager.h"

CAnimationSet::CAnimationSet(float length, int framesPerSecond, int keyFrameNum, int boneNum, string name)
{
	mLength = length;
	mFramesPerSecond = framesPerSecond;
	mKeyFrames = keyFrameNum;

	mAnimationSetName = name;

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
	for (int i = 0; i < (mKeyFrames - 1); i++)
	{
		if ((mKeyFrameTimes[i] <= position) && (position < mKeyFrameTimes[i + 1]))
		{
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
	mAnimationSets.resize(setNum);
	for (auto& set : mAnimationSets) set = std::make_shared<CAnimationSet>();
}

CAnimationSets::~CAnimationSets()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationTrack::~CAnimationTrack()
{
}

void CAnimationTrack::SetCallbackKeys(int num)
{
	mCallbackKeys.resize(num);
}

void CAnimationTrack::SetCallbackKey(int index, float keyTime, void* data)
{
	mCallbackKeys[index].mTime = keyTime;
	mCallbackKeys[index].mCallbackData = data;
}

void CAnimationTrack::SetAnimationCallbackHandler(std::shared_ptr<CAnimationCallbackHandler> callbackHandler)
{
	mAnimationCallbackHandler = callbackHandler;
}

void CAnimationTrack::HandleCallback()
{
	if (mAnimationCallbackHandler) {
		for (auto& key : mCallbackKeys) {
			if (SimpleMath::IsEqual(key.mTime, mPosition, ANIMATION_CALLBACK_EPSILON) && key.mCallbackData) {
				mAnimationCallbackHandler->HandleCallback(key.mCallbackData, mPosition);
				break;
			}
		}
	}
}

float CAnimationTrack::UpdatePosition(float trackPosition, float elapsedTime, float animationLength)
{
	float trackElapsedTime = elapsedTime * mSpeed;
	switch (mType) {
	case ANIMATION_TYPE::LOOP: {
		if (mPosition < 0.0f) mPosition = 0.0f;
		else {
			mPosition = trackPosition + trackElapsedTime;
			if (mPosition > animationLength) {
				mPosition = -ANIMATION_CALLBACK_EPSILON;
				return(animationLength);
			}
		}
		//			mPosition = fmod(trackPosition, m_pfKeyFrameTimes[m_nKeyFrames-1]); // mPosition = trackPosition - int(trackPosition / m_pfKeyFrameTimes[m_nKeyFrames-1]) * m_pfKeyFrameTimes[m_nKeyFrames-1];
		//			mPosition = fmod(trackPosition, m_fLength); //if (mPosition < 0) mPosition += m_fLength;
		//			mPosition = trackPosition - int(trackPosition / m_fLength) * m_fLength;
		break;
	}
	case ANIMATION_TYPE::ONCE:
		mPosition = trackPosition + trackElapsedTime;
		if (mPosition > animationLength) mPosition = animationLength;
		break;
	case ANIMATION_TYPE::PINGPONG:
		break;
	}

	return(mPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController() : CComponent(COMPONENT_TYPE::ANIMATION)
{
	mTracks.push_back(std::make_shared<CAnimationTrack>());
}

CAnimationController::CAnimationController(std::shared_ptr<CAnimationSets>& sets, bool applyRootMotion) : CComponent(COMPONENT_TYPE::ANIMATION)
{
	mApplyRootMotion = applyRootMotion;
	mAnimationSets = sets;
	mTracks.push_back(std::make_shared<CAnimationTrack>());
}

CAnimationController::~CAnimationController()
{
}

void CAnimationController::SetAnimationCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr <CAnimationCallbackHandler>& callbackHandler)
{
	animationSet->SetCallbackHandler(callbackHandler);
}

void CAnimationController::Awake()
{
	auto skinnedMeshRenderer = owner->GetComponentFromHierarchy<CSkinnedMeshRenderer>();
	if (skinnedMeshRenderer) {
		mBindPoseBoneOffsets = skinnedMeshRenderer->mSkinnedMesh->GetBindPoseBoneOffsets();
	}
	auto& boneNames = skinnedMeshRenderer->mBoneNames;
	mBoneCaches.resize(boneNames.size());
	finalTransforms.resize(boneNames.size());

	std::unordered_map<std::string, std::weak_ptr<CTransform>> boneMap;
	for (int i = 0; i < boneNames.size(); ++i) {
		auto bone = owner->FindChildByName(boneNames[i]);
		if (bone) {
			boneMap[boneNames[i]] = bone->GetTransform();
			mBoneCaches[i] = bone->GetTransform();
		}
	}

	for (auto& set : mAnimationSets->mAnimationSet) {
		for (auto& layer : set->mLayers) {
			for (int i = 0; auto& cache : layer->mBoneFrameCaches) {
				auto& boneName = layer->mBoneNames[i];
				if (boneMap.contains(boneName)) cache = boneMap[boneName];
				else cache = owner->FindChildByName(boneName)->GetTransform();
				++i;
			}
		}
	}
	mRootTransform = mAnimationSets->mAnimationSet[0]->mLayers[0]->mBoneFrameCaches[0].lock()->GetTransform();
	mRootTransform.lock()->owner->SetStatic(true);

	if (mBoneTransformIdx == -1) {
		mBoneTransformIdx = INSTANCE(CObjectPoolManager).GetBoneTransformIdx();
	}

	SetTrackAnimationSet(0, 1);
	SetTrackSpeed(0, 1.0f);
	SetTrackWeight(0, 1.0f);
}

void CAnimationController::Start()
{
}

void CAnimationController::Update()
{
}

void CAnimationController::LateUpdate()
{
	float deltaTime = DELTA_TIME;
	mTime += deltaTime;
	int nEnabledAnimationTracks = 0;
	auto& animationSets = mAnimationSets->mAnimationSet;

	for (auto& track : mTracks) {
		if (track->mEnabled) {
			nEnabledAnimationTracks++;
			auto& animationSet = animationSets[track->mIndex];
			animationSet->Animate(deltaTime * track->mSpeed, track->mWeight, track->mStartTime, track->mEndTime, track == mTracks.front());
		}
	}
	mRootTransform.lock()->owner->UpdateWorldMatrices(nullptr);	

	for (auto& track : mTracks) {
		if (track->mEnabled && animationSets.size())
			animationSets[track->mIndex]->HandleCallback();
	}

	for (int i = 0; auto & cache : mBoneCaches) {
		Matrix boneTransform = cache.lock()->GetWorldMat(false);
		Matrix bondOffset = Matrix::Identity;
		bondOffset = mBindPoseBoneOffsets[i];

		finalTransforms[i] = (bondOffset * boneTransform).Transpose();
		i++;
	}

	UINT offset = mBoneTransformIdx * ALIGNED_SIZE(sizeof(Matrix) * SKINNED_ANIMATION_BONES);
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)->UpdateBuffer(
		offset,
		finalTransforms.data(),
		sizeof(Matrix) * finalTransforms.size()
	);
}

void CAnimationController::SetTrackAnimationSet(int trackIndex, int setIndex)
{
	if (trackIndex < mTracks.size() && mTracks[trackIndex]->mIndex != setIndex) {
		mTracks[trackIndex]->SetAnimationSet(setIndex);
		mTracks[trackIndex]->SetStartEndTime(mAnimationSets->mAnimationSet[setIndex]->mStartTime, mAnimationSets->mAnimationSet[setIndex]->mEndTime);
	}
}

void CAnimationController::SetTrackEnabled(int trackIndex, bool enabled)
{
	if (trackIndex < mTracks.size()) {
		mTracks[trackIndex]->SetEnable(enabled);
	}
}

void CAnimationController::SetTrackPosition(int trackIndex, float position)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetPosition(position);
	if (mAnimationSets->mAnimationSet.size()) mAnimationSets->mAnimationSet[mTracks[trackIndex]->mIndex]->SetPosition(position);
}

void CAnimationController::SetTrackSpeed(int trackIndex, float speed)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetSpeed(speed);
}

void CAnimationController::SetTrackWeight(int trackIndex, float weight)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetWeight(weight);
}

void CAnimationController::SetTrackStartEndTime(int trackIndex, float start, float end)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetStartEndTime(start, end);
}

void CAnimationController::SetAnimationType(std::shared_ptr<CAnimationSet>& animationSet, ANIMATION_TYPE type)
{
	animationSet->SetAnimationType(type);
}

void CAnimationController::AdvanceTime(float elapsedTime, std::shared_ptr<CGameObject>& rootGameObject)
{
	
}

void CAnimationController::BindSkinningMatrix()
{
	UINT offset = mBoneTransformIdx * ALIGNED_SIZE(sizeof(Matrix) * SKINNED_ANIMATION_BONES);
	CONSTANTBUFFER(CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)->BindToShader(offset);
}

void CAnimationController::PrepareSkinning()
{

}

void CAnimationController::UploadBoneOffsets()
{
}