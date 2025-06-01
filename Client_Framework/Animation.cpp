#include "stdafx.h"
#include "Animation.h"
#include "Mesh.h"
#include "Transform.h"
#include "SkinnedMesh.h"
#include "Timer.h"
#include "SkinnedMeshRenderer.h"
#include "ObjectPoolManager.h"
#include"AnimationEnums.h"

std::unordered_map<PLAYER_STATE, ARCHER_ANIMATION> CAnimationController::ARCHER_MAP = {
	{ PLAYER_STATE::IDLE, ARCHER_ANIMATION::COMBATIDLE },
	{ PLAYER_STATE::RUN, ARCHER_ANIMATION::RUN },
	{ PLAYER_STATE::ATTACK, ARCHER_ANIMATION::ATTACK },
	{ PLAYER_STATE::MOVE_ATTACK, ARCHER_ANIMATION::RUNATTACK },
	{ PLAYER_STATE::GETHIT, ARCHER_ANIMATION::GETHIT },
	{ PLAYER_STATE::DEATH, ARCHER_ANIMATION::DEATH },
	{ PLAYER_STATE::JUMP, ARCHER_ANIMATION::JUMP },
	{ PLAYER_STATE::SKILL, ARCHER_ANIMATION::SKILLATTACK }
};

std::unordered_map<PLAYER_STATE, FIGHTER_ANIMATION> CAnimationController::FIGHTER_MAP = {
	{ PLAYER_STATE::IDLE, FIGHTER_ANIMATION::IDLE },
	{ PLAYER_STATE::RUN, FIGHTER_ANIMATION::RUN },
	{ PLAYER_STATE::ATTACK, FIGHTER_ANIMATION::ATTACK },
	{ PLAYER_STATE::MOVE_ATTACK, FIGHTER_ANIMATION::RUNATTACK },
	{ PLAYER_STATE::GETHIT, FIGHTER_ANIMATION::GETHIT },
	{ PLAYER_STATE::DEATH, FIGHTER_ANIMATION::DEATH },
	{ PLAYER_STATE::JUMP, FIGHTER_ANIMATION::JUMP },
	{ PLAYER_STATE::SKILL, FIGHTER_ANIMATION::SKILLATTACK }
};

std::unordered_map<PLAYER_STATE, MAGE_ANIMATION> CAnimationController::MAGE_MAP = {
	{ PLAYER_STATE::IDLE, MAGE_ANIMATION::IDLE },
	{ PLAYER_STATE::RUN, MAGE_ANIMATION::RUN },
	{ PLAYER_STATE::ATTACK, MAGE_ANIMATION::ATTACK },
	{ PLAYER_STATE::MOVE_ATTACK, MAGE_ANIMATION::RUNATTACK },
	{ PLAYER_STATE::GETHIT, MAGE_ANIMATION::GETHIT },
	{ PLAYER_STATE::DEATH, MAGE_ANIMATION::DEATH },
	{ PLAYER_STATE::JUMP, MAGE_ANIMATION::JUMP },
	{ PLAYER_STATE::SKILL, MAGE_ANIMATION::SKILLATTACK }
};

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
	mCallbackKeys = other.mCallbackKeys;
	mAnimationCallbackHandler = std::make_shared<CAnimationCallbackHandler>(*other.mAnimationCallbackHandler);
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
		if (mPosition > animationLength) {
			mPosition = 0.0f;
			mType = ANIMATION_TYPE::END;
		}
		break;
	case ANIMATION_TYPE::PINGPONG:
		break;
	}

	return(mPosition);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CAnimationController::CAnimationController()
{
	mTracks.push_back(std::make_shared<CAnimationTrack>());
}

CAnimationController::CAnimationController(int trackNum, std::shared_ptr<CAnimationSets>& sets, bool applyRootMotion)
{
	mTracks.resize(trackNum);
	for (auto& track : mTracks) track = std::make_shared<CAnimationTrack>();

	mApplyRootMotion = applyRootMotion;

	mAnimationSets = sets;
}

CAnimationController::CAnimationController(const CAnimationController& other) : CComponent(other)
{
	mTracks.resize(other.mTracks.size());
	for (int i = 0; i < mTracks.size(); ++i) {
		if (other.mTracks[i]) {
			mTracks[i] = std::make_shared<CAnimationTrack>(*other.mTracks[i]);
		}
	}
	mAnimationSets = std::make_shared<CAnimationSets>(*other.mAnimationSets);
	mBindPoseBoneOffsets = other.mBindPoseBoneOffsets;
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
	if(owner->GetName() == "GrassSmall")
		int a = 0; // Debugging line to check if this component is being initialized on the correct object
	auto skinnedMeshRenderer = owner->GetComponentFromHierarchy<CSkinnedMeshRenderer>();
	if (skinnedMeshRenderer)
		mBindPoseBoneOffsets = skinnedMeshRenderer->mSkinnedMesh->GetBindPoseBoneOffsets();
	
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
		for (int i = 0; auto & cache : mAnimationSets->mBoneFrameCaches) {
			auto& boneName = mAnimationSets->mBoneNames[i];
			if (boneMap.contains(boneName)) cache = boneMap[boneName];
			else {
				auto object = owner->FindChildByName(boneName);
				if (object) cache = object->GetTransform();
			}
			++i;
		}

		SetTrackAnimationSet(0, 2);
		SetTrackSpeed(0, 1.0f);
		SetTrackWeight(0, 1.0f);
	}


	mRootMotionObject.lock()->owner->SetStatic(true);
}

void CAnimationController::Update()
{
}

void CAnimationController::LateUpdate()
{
	float deltaTime = DELTA_TIME;
	mTime += deltaTime;

	if (mTracks.size()) {
		for (auto& cache : mAnimationSets->mBoneFrameCaches) { if (cache.lock()) cache.lock()->SetLocalMatZero(); }
	
		for (auto& track : mTracks) {
			if (track->mEnable) {
				auto& set = mAnimationSets->mAnimationSet[track->mSetIndex];
				float position = track->UpdatePosition(track->mPosition, deltaTime, set->mLength);
	
				for (int i = 0; auto & cache : mAnimationSets->mBoneFrameCaches) {
					if (cache.lock()) {
						Matrix transform = set->GetSRT(i, position);
						transform *= track->mWeight;
						cache.lock()->mLocalMat = transform;
					}

					++i;
				}
	
				track->HandleCallback();
			}
		}
	
		GetOwner()->UpdateWorldMatrices(nullptr);
	
		OnRootMotion(mRootMotionObject);
		OnAnimationIK(mRootMotionObject);
	}

	for (int i = 0; auto & cache : mSkinningBoneTransforms) {
		Matrix boneTransform = cache.lock()->GetWorldMat(false);
		Matrix bondOffset = mBindPoseBoneOffsets[i];
		
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

void CAnimationController::SetCallbackKeys(int trackIndex, int num)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetCallbackKeys(num);
}

void CAnimationController::SetCallbackKey(int trackIndex, int keyIndex, float keyTime, void* data)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetCallbackKey(keyIndex, keyTime, data);
}

void CAnimationController::SetAnimationCallbackHandler(int trackIndex, std::shared_ptr<CAnimationCallbackHandler> callbackHandler)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetAnimationCallbackHandler(callbackHandler);
}

void CAnimationController::SetTrackAnimationSet(int trackIndex, int setIndex)
{
	if (trackIndex < mTracks.size() && mTracks[trackIndex]->mSetIndex != setIndex) {
		mTracks[trackIndex]->SetAnimationSet(setIndex);
	}
}

void CAnimationController::SetTrackEnabled(int trackIndex, bool enabled)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetEnable(enabled);
}

void CAnimationController::SetTrackPosition(int trackIndex, float position)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetPosition(position);
}

void CAnimationController::SetTrackSpeed(int trackIndex, float speed)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetSpeed(speed);
}

void CAnimationController::SetTrackWeight(int trackIndex, float weight)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetWeight(weight);
}

void CAnimationController::SetTrackType(int trackIndex, ANIMATION_TYPE type)
{
	if (trackIndex < mTracks.size()) mTracks[trackIndex]->SetType(type);
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

void CAnimationController::PrintMatrix(const Matrix& mat)
{
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			std::cout << mat.m[row][col] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}