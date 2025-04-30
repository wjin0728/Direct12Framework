#include "stdafx.h"
#include "Animation.h"
#include "Mesh.h"
#include "Transform.h"
#include "SkinnedMesh.h"
#include "Timer.h"
#include "SkinnedMeshRenderer.h"
#include "ObjectPoolManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationCurve::CAnimationCurve(int keyNum)
{
	mKeyTimes.reserve(keyNum);
	mKeyValues.reserve(keyNum);
}

CAnimationCurve::~CAnimationCurve()
{
}

float CAnimationCurve::GetInterpolatedValue(float position)
{
	for (int k = 0; k + 1 < mKeyTimes.size(); ++k) {
		if ((mKeyTimes[k] <= position) && (position < mKeyTimes[k + 1])) {
			float t = (position - mKeyTimes[k]) / (mKeyTimes[k + 1] - mKeyTimes[k]);
			return(mKeyValues[k] * (1.0f - t) + mKeyValues[k + 1] * t);
		}
	}

	return mKeyValues.back();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationLayer::CAnimationLayer()
{
}

CAnimationLayer::~CAnimationLayer()
{
}

void CAnimationLayer::LoadKeyValues(int boneFrame, int curve, std::ifstream& file)
{
	int keyNum{};
	float data;
	BinaryReader::ReadDateFromFile(file, keyNum);
	auto animationCurve = std::make_shared<CAnimationCurve>(keyNum);


	for (int i = 0; i < keyNum; ++i) {
		BinaryReader::ReadDateFromFile(file, data);
		animationCurve->mKeyTimes.push_back(data);
	}
	for (int i = 0; i < keyNum; ++i) {
		BinaryReader::ReadDateFromFile(file, data);
		animationCurve->mKeyValues.push_back(data);
	}

	mAnimationCurves[boneFrame][curve] = animationCurve;
}

void CAnimationLayer::GetSRT(const std::array<std::shared_ptr<CAnimationCurve>, 9>& curves, float position, Vec3& scale, Vec3& rotation, Vec3& translation)
{
	if (curves[0]) translation.x = curves[0]->GetInterpolatedValue(position);
	if (curves[1]) translation.y = curves[1]->GetInterpolatedValue(position);
	if (curves[2]) translation.z = curves[2]->GetInterpolatedValue(position);
	if (curves[3]) rotation.x = curves[3]->GetInterpolatedValue(position);
	if (curves[4]) rotation.y = curves[4]->GetInterpolatedValue(position);
	if (curves[5]) rotation.z = curves[5]->GetInterpolatedValue(position);
	if (curves[6]) scale.x = curves[6]->GetInterpolatedValue(position);
	if (curves[7]) scale.y = curves[7]->GetInterpolatedValue(position);
	if (curves[8]) scale.z = curves[8]->GetInterpolatedValue(position);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSet::CAnimationSet(float startTime, float endTime, const std::string& name)
{
	mStartTime = startTime;
	mEndTime = endTime;
	mLength = endTime - startTime;
	mName = name;
}

CAnimationSet::~CAnimationSet()
{
}

void CAnimationSet::HandleCallback()
{
	if (mCallbackHandler) {
		for (auto& callbackKey : mCallbackKeys) {
			if (SimpleMath::IsEqual(callbackKey.mTime, mPosition)) {
				if (callbackKey.mData)
					mCallbackHandler->HandleCallback(callbackKey.mData, mPosition);
				break;
			}
		}
	}
}

float CAnimationSet::UpdatePosition(float position, float start, float end)
{
	switch (mType) {
	case ANIMATION_TYPE::LOOP: {
		mPosition += position;
		if (mPosition < start) mPosition = start;
		if (mPosition > end) mPosition = start;
		break;
	}
	case ANIMATION_TYPE::ONCE: {
		mPosition += position;
		if (mPosition < start) mPosition = start;
		if (mPosition > end) mPosition = end;
		break;
	}
	case ANIMATION_TYPE::PINGPONG:
		break;
	}

	return(mPosition);
}

void CAnimationSet::Animate(float position, float weight, float start, float end, bool override)
{
	float pos = UpdatePosition(position, start, end);

	for (int i = 0;  auto & layer : mLayers) {
		for (int j = 0; auto & boneFrame : layer->mBoneFrameCaches) {
			auto transform = boneFrame.lock();

			mScales[i][j] = transform->GetLocalScale();
			mRotations[i][j] = transform->GetLocalEulerAngles();
			mTranslations[i][j] = transform->GetLocalPosition();

			layer->GetSRT(layer->mAnimationCurves[j], pos, mScales[i][j], mRotations[i][j], mTranslations[i][j]);
			transform->BlendingTransform(layer->mBlendMode, mScales[i][j], mRotations[i][j], mTranslations[i][j], layer->mWeight);
			transform->ApplyBlendedTransform();

			++j;
		}
		++i;
	}
}

void CAnimationSet::SetCallbackHandler(std::shared_ptr<CCallbackHandler>& handler)
{
	mCallbackHandler = handler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationSets::CAnimationSets(int setsNum)
{
	mAnimationSet.resize(setsNum);
}

CAnimationSets::~CAnimationSets()
{
}

void CAnimationSets::SetCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr<CCallbackHandler>& callbackHandler)
{
	animationSet->SetCallbackHandler(callbackHandler);
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

void CAnimationController::SetAnimationCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr <CCallbackHandler>& callbackHandler)
{
	animationSet->SetCallbackHandler(callbackHandler);
}

void CAnimationController::Awake()
{
	
}

void CAnimationController::Start()
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
			for (int i = 0; auto & cache : layer->mBoneFrameCaches) {
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

	SetTrackAnimationSet(0, 0);
	SetTrackPosition(0, 0.55f);
	SetTrackSpeed(0, 0.5f);
	SetTrackWeight(0, 1.0f);
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
	if (trackIndex < mTracks.size()) {
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