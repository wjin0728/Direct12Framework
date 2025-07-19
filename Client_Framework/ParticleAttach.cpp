#include "stdafx.h"
#include "ParticleAttach.h"
#include "GameObject.h"
#include "Transform.h"
#include"ParticleEmitter.h"
#include"ParticleManager.h"

CParticleAttach::CParticleAttach(const CParticleAttach& other)
{
	mParticleEmitterName = other.mParticleEmitterName;
	mParticleEmitter = nullptr;
	mCanEmit = other.mCanEmit;
}

CParticleAttach::~CParticleAttach()
{
	if (mParticleEmitter)
	{
		mParticleEmitter->mParticleAttach = nullptr;
		mParticleEmitter = nullptr;
	}
}

void CParticleAttach::Awake()
{
	if (mParticleEmitter == nullptr && mCanEmit)
	{
		mParticleEmitter = INSTANCE(CParticleManager).GetAvailableParticleEmitter(mParticleEmitterName);
	}
	if(mParticleEmitter) mParticleEmitter->mParticleAttach = this;
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
}

void CParticleAttach::InitializeParticleEmitter()
{
	if (mParticleEmitter == nullptr && mCanEmit)
	{
		mParticleEmitter = INSTANCE(CParticleManager).GetAvailableParticleEmitter(mParticleEmitterName);
	}
	if (mParticleEmitter) mParticleEmitter->mParticleAttach = this;
}

void CParticleAttach::Play()
{
	InitializeParticleEmitter();

	if (mParticleEmitter && !mParticleEmitter->mIsPlaying)
		mParticleEmitter->mEmitterTransform = GetTransform()->GetWorldMat();
		INSTANCE(CParticleManager).PlayParticleEmitter(mParticleEmitter);
}

void CParticleAttach::Stop()
{
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
