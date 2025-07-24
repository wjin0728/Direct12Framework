#include "stdafx.h"
#include "ParticleAttach.h"
#include "GameObject.h"
#include "Transform.h"
#include"ParticleEmitter.h"
#include"ParticleManager.h"

CParticleAttach::CParticleAttach(const CParticleAttach& other)
{
	mParticleEmitterName = other.mParticleEmitterName;
	mCanEmit = other.mCanEmit;
	mLoop = other.mLoop;
	mReserve = other.mReserve;
}

CParticleAttach::~CParticleAttach()
{
	if (mParticleEmitter)
	{
		mParticleEmitter->Stop(false);
		mParticleEmitter = nullptr;
	}
}

void CParticleAttach::Awake()
{
}

void CParticleAttach::Start()
{

	if(mReserve) Play();
	
}

void CParticleAttach::Update()
{
}

void CParticleAttach::LateUpdate()
{
	auto transform = GetTransform();
	if (transform == nullptr || mParticleEmitter == nullptr)
		return;
	mParticleEmitter->mEmitterTransform = transform->GetWorldMat();
	mIsPlaying = mParticleEmitter->mIsPlaying;
	if (!mParticleEmitter->mIsPlaying) {
		mParticleEmitter = nullptr;
	}
}



void CParticleAttach::Play()
{
	if (mIsPlaying && mLoop) {
		return;
	}
	mParticleEmitter = INSTANCE(CParticleManager).PlayParticleEmitter(mParticleEmitterName, GetTransform()->GetWorldMat(), mLoop);
}

void CParticleAttach::Stop()
{
	if (mParticleEmitter->mIsPlaying) {
		mParticleEmitter->Stop();
	}
}

void CParticleAttach::Reserve(bool reserve)
{
	if (mReserve == reserve) return;
	mReserve = reserve;
	std::vector<std::shared_ptr<CParticleAttach>> children{};
	owner->GetAllComponentsFromHierarchy<CParticleAttach>(children);
	for (auto& child : children)
	{
		child->Reserve(reserve);
	}
}

void CParticleAttach::SetLoop(bool loop)
{
	mLoop = loop;
	if (mParticleEmitter)
	{
		mParticleEmitter->mIsLooping = loop;
		if (!loop)
		{
			mParticleEmitter = nullptr;
		}
	}
}
