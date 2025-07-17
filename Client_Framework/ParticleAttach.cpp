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
	if (mParticleEmitter == nullptr && mCanEmit)
	{
		mParticleEmitter = INSTANCE(CParticleManager).GetAvailableParticleEmitter(mParticleEmitterName);
	}
	if (mParticleEmitter) mParticleEmitter->mParticleAttach = this;

	if (mParticleEmitter && !mParticleEmitter->mIsPlaying)
		mParticleEmitter->mEmitterTransform = GetTransform()->GetWorldMat();
		INSTANCE(CParticleManager).PlayParticleEmitter(mParticleEmitter);

	auto& children = owner->GetChildren();
	for (auto& child : children)
	{
		auto particleAttach = child->GetComponent<CParticleAttach>();
		if (particleAttach)
		{
			particleAttach->Play();
		}
	}
}

void CParticleAttach::Stop()
{
}