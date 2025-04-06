#include "stdafx.h"
#include "Animation.h"
#include "Mesh.h"
#include "Transform.h"
#include "SkinnedMesh.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationCurve::CAnimationCurve(int keyNum)
{
	mKeyTimes.resize(keyNum);
	mKeyValues.resize(keyNum);
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
	int keyNum;
	float data;
	auto animationCurve = std::make_shared<CAnimationCurve>(keyNum);

	BinaryReader::ReadDateFromFile(file, keyNum);

	for (int i = 0; i < keyNum; ++i) {
		BinaryReader::ReadDateFromFile(file, data);
		animationCurve->mKeyTimes[i] = data;
	}
	for (int i = 0; i < keyNum; ++i) {
		BinaryReader::ReadDateFromFile(file, data);
		animationCurve->mKeyValues[i] = data;
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

	int i = 0, j = 0;
	for (int i = 0;  auto & layer : mLayers) {
		for (int j = 0; auto & boneFrame : layer->mBoneFrameCaches) {
			std::shared_ptr<CTransform> transform = boneFrame->GetTransform();

			mScales[i][j] = transform->GetLocalScale();
			mRotations[i][j] = transform->GetLocalEulerAngles();
			mTranslations[i][j] = transform->GetLocalPosition();

			layer->GetSRT(layer->mAnimationCurves[j], pos, mScales[i][j], mRotations[i][j], mTranslations[i][j]);
			++j;
		}
		++i;
	}

	// 애니메이션 레이어들을 블렌딩한다.
	for (int i = 0;  auto & layer : mLayers) {
		for (int j = 0;  auto & boneFrame : layer->mBoneFrameCaches) {
			std::shared_ptr<CTransform> transform = boneFrame->GetTransform();

			switch (layer->mBlendMode) {
			case ANIMATION_BLEND_TYPE::ADDITIVE: {
				transform->GetScaleLayerBlending() += mScales[i][j];
				transform->GetRotationLayerBlending() += mRotations[i][j];
				transform->GetPositionLayerBlending() += mTranslations[i][j];
				break;
			}
			case ANIMATION_BLEND_TYPE::OVERRIDE: {
				transform->GetScaleLayerBlending() = mScales[i][j];
				transform->GetRotationLayerBlending() = mRotations[i][j];
				transform->GetPositionLayerBlending() = mTranslations[i][j];
				break;
			}
			case ANIMATION_BLEND_TYPE::OVERRIDE_PASSTHROUGH: {
				transform->GetScaleLayerBlending() += mScales[i][j] * layer->mWeight;
				transform->GetRotationLayerBlending() += mRotations[i][j] * layer->mWeight;
				transform->GetPositionLayerBlending() += mTranslations[i][j] * layer->mWeight;
				break;
			}
			}
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
}

CAnimationController::CAnimationController(std::shared_ptr<CAnimationSets>& sets, bool applyRootMotion) : CComponent(COMPONENT_TYPE::ANIMATION)
{
	mApplyRootMotion = applyRootMotion;
	mAnimationSets = sets;

	for (int i = 0; auto& skinnedMesh : mAnimationSets->mSkinnedMeshes) {
		skinnedMesh->mBoneTransformIndex = i++;
	}
}

CAnimationController::~CAnimationController()
{
}

void CAnimationController::SetAnimationCallbackHandler(std::shared_ptr<CAnimationSet>& animationSet, std::shared_ptr <CCallbackHandler>& callbackHandler)
{
	animationSet->SetCallbackHandler(callbackHandler);
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
	if (mAnimationSets->mAnimationSet.size()) mAnimationSets->mAnimationSet[mTracks[trackIndex]->mAnimationSetIndex]->SetPosition(position);
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

void CAnimationController::UpdateShaderVariables()
{
}

void CAnimationController::AdvanceTime(float elapsedTime, std::shared_ptr<CGameObject>& rootGameObject)
{
	mTime += elapsedTime;
	int nEnabledAnimationTracks = 0;

	for (auto& track : mTracks) {
		if (track->mEnabled) {
			nEnabledAnimationTracks++;
			std::shared_ptr<CAnimationSet> animationSet = mAnimationSets->mAnimationSet[track->mAnimationSetIndex];
			animationSet->Animate(elapsedTime * track->mSpeed, track->mWeight, track->mStartTime, track->mEndTime, track == mTracks.front());
		}
	}

	//*
	if (nEnabledAnimationTracks == 1) {
		for (auto& track : mTracks) {
			if (track->mEnabled) {
				std::shared_ptr<CAnimationSet> animationSet = mAnimationSets->mAnimationSet[track->mAnimationSetIndex];
				
				for (auto& layer : animationSet->mLayers) {
					for (auto& cache : layer->mBoneFrameCaches) {
						cache->GetTransform()->ApplyBlendedTransform();
					}
				}
			}
		}
	}
	else {
		for (auto& track : mTracks) {
			if (track->mEnabled) {
				std::shared_ptr<CAnimationSet> animationSet = mAnimationSets->mAnimationSet[track->mAnimationSetIndex];
				
				for (auto& layer : animationSet->mLayers) {
					for (auto& cache : layer->mBoneFrameCaches) {
						cache->GetTransform()->ApplyBlendedTransform();
					}
				}
			}
		}
	}
	//*/

	rootGameObject->UpdateTransform();

	for (auto& track : mTracks) {
		if (track->mEnabled && mAnimationSets->mAnimationSet.size())
			mAnimationSets->mAnimationSet[track->mAnimationSetIndex]->HandleCallback();
	}

	for (auto& skinnedMesh : mAnimationSets->mSkinnedMeshes) {
		const int boneNum = skinnedMesh->GetBoneNum();
		std::vector<std::shared_ptr<Matrix>> finalBones(boneNum);

		for (int i = 0; auto& bone : finalBones) {
			bone = std::make_shared<Matrix>(skinnedMesh->mBoneFrameCaches[i++]->GetTransform()->mWorldMat);
		}

		int index = skinnedMesh->GetBoneTransformIndex();
		if (index >= 0) {
			auto curFrameResource = INSTANCE(CDX12Manager).GetCurFrameResource();

			curFrameResource->GetConstantBuffer((UINT)CONSTANT_BUFFER_TYPE::BONE_TRANSFORM)->UpdateBuffer(index, finalBones.data());
		}
	}
}