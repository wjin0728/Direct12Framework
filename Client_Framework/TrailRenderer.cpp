#include "stdafx.h"
#include "TrailRenderer.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "DX12Manager.h"
#include "ResourceManager.h"
#include"RenderManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include"Timer.h"
#include"VertexBuffer.h"

CTrailRenderer::CTrailRenderer()
{
	mVertexCount = 0;
    mDuration = 0.2f;
    mTotalTime = 0.0f;
	mWidth = 0.5f; 
	mMinDstance = 0.4f;
	mMaxPoints = 1000; 
    mTrailPoints.reserve(mMaxPoints);
}

CTrailRenderer::CTrailRenderer(const CTrailRenderer& other)
{
    mDuration = other.mDuration;
    mTotalTime = other.mTotalTime;
    mWidth = other.mWidth;
    mMinDstance = other.mMinDstance;
    mMaxPoints = other.mMaxPoints;
    mTrailPoints.reserve(mMaxPoints);
    for (const auto& point : other.mTrailPoints) {
        mTrailPoints.push_back(point);
    }
    mVertexCount = 0;
}

CTrailRenderer::~CTrailRenderer()
{
}

void CTrailRenderer::Awake()
{
	CRenderer::Awake();
    mVertexBuffer = std::make_unique<CVertexBuffer>();

    INSTANCE(CDX12Manager).OpenCommandList();
    mVertexBuffer->Initialize(0, sizeof(TrailVertex), mMaxPoints * 2, nullptr, true);
    INSTANCE(CDX12Manager).CloseCommandList();

    owner->SetRenderLayer(RENDER_LAYER::Transparent);

	auto material = INSTANCE(CResourceManager).Get<CMaterial>("TrailDefault");
	auto matInstance = material->Instantiate();
	AddMaterial(matInstance);
}

void CTrailRenderer::Start()
{
	CRenderer::Start();
	int idx = m_materials[0]->GetProperty<UINT>("normalTexIdx");
}

void CTrailRenderer::Update()
{
}

void CTrailRenderer::LateUpdate()
{
	auto transform = GetTransform();
	Vec3 worldPos = transform->GetWorldPosition();

    if (mActive) {
        if (mTrailPoints.empty()) {
            mTrailPoints.push_back({ worldPos, mTotalTime, 0.9f });
            mTrailPoints.push_back({ worldPos, mTotalTime, 1.f });
        }
        else if (mTrailPoints.size() >= 2 && mTrailPoints.size() < mMaxPoints) {
            auto& origin = mTrailPoints.back();
            origin.position = worldPos;
            origin.time = mTotalTime;
            if ((origin.position - mTrailPoints[mTrailPoints.size() - 2].position).Length() >= mMinDstance) {
                mTrailPoints.back().alpha = 0.9f;
                mTrailPoints.push_back({ worldPos, mTotalTime, 1.f });
            }
        }
        else if (mTrailPoints.size() == 1)
        {
            mTrailPoints.push_back({ worldPos, mTotalTime, 1.f });
        }
    }
    if (mTotalTime >= mDuration) {
        float cutTime = mTotalTime - mDuration; 

        if (mTrailPoints.size() > 2) {
            auto& tail = mTrailPoints.front();
            auto& tailNext = mTrailPoints[1];

            if (tail.time <= cutTime && cutTime <= tailNext.time) {
                float timeDiff = tailNext.time - tail.time;
                float ratio = (cutTime - tail.time) * 2 / timeDiff;

                ratio = std::clamp(ratio, 0.0f, 1.0f);
                tail.position = Vec3::Lerp(tail.position, tailNext.position, ratio);
				tail.alpha = std::lerp(tail.alpha, tailNext.alpha, ratio);

                if ((tail.position - tailNext.position).Length() < 0.01f || ratio >= 1.0f) {
                    mTrailPoints.erase(mTrailPoints.begin());
                }
            }
            else if (tail.time < cutTime) {
                mTrailPoints.erase(mTrailPoints.begin());
            }
        }
    }
    UpdateVertices();

    mTotalTime += DELTA_TIME;
}

void CTrailRenderer::Render(class CCamera* camera, int pass) 
{
    if (mTrailPoints.size() < 3)
        return;
    if (!mVertexBuffer || mVertexCount == 0) return;
	mVertexBuffer->SetVertexBuffer();
	m_materials[0]->BindShader(PASS_TYPE::FORWARD);
	m_materials[0]->BindDataToShader();

    CMDLIST->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    CMDLIST->DrawInstanced(mVertexCount, 1, 0, 0);
}

void CTrailRenderer::SetBlendMaskTexture(const std::string& name)
{
    auto material = m_materials[0];
    if (material) {
        auto texture = INSTANCE(CResourceManager).Get<CTexture>(name);
        if (texture) {
			UINT blendTexIdx = texture->GetSrvIndex();
            material->SetProperty("normalTexIdx", blendTexIdx);
        }
	}
}

void CTrailRenderer::UpdateVertices()
{
    if (mTrailPoints.size() < 3)
        return;
	auto camera = INSTANCE(CRenderManager).GetMainCamera();
    if(!camera) return;

    std::vector<TrailPoint> smoothTrailPoints{};
	const int segmentPerPair = 4; 

    if (mTrailPoints.size() >= 4) { 
        for (int i = 1; i < mTrailPoints.size() - 2; ++i) {
            for (int s = 0; s < segmentPerPair; ++s) {
                float t = (float)s / segmentPerPair;

                Vec3 pos = Vec3::CatmullRom(
                    mTrailPoints[i - 1].position,
                    mTrailPoints[i].position,
                    mTrailPoints[i + 1].position,
                    mTrailPoints[i + 2].position,
                    t
                );
                float time = std::lerp(mTrailPoints[i].time, mTrailPoints[i + 1].time, t);
                float alpha = std::lerp(mTrailPoints[i].alpha, mTrailPoints[i + 1].alpha, t);

                smoothTrailPoints.push_back({ pos, time, alpha });
            }
        }
        smoothTrailPoints.push_back(mTrailPoints[mTrailPoints.size() - 2]);
        smoothTrailPoints.push_back(mTrailPoints[mTrailPoints.size() - 1]);
    }
    else smoothTrailPoints = mTrailPoints;

    TrailVertex* vertices = reinterpret_cast<TrailVertex*>(mVertexBuffer->mappedData);
	if (!vertices) return;
    if(smoothTrailPoints.size() < 2) return;
	auto transform = GetTransform();
    mVertexCount = 0;

    float uvStep = 1.0f / (smoothTrailPoints.size() - 1);
    Vec3 right = transform->GetWorldLook();
    for (size_t i = 0; i < smoothTrailPoints.size(); ++i)
    {
        Vec3 direction;
        if (i == 0) {
            direction = (smoothTrailPoints[i+1].position - smoothTrailPoints[i].position).GetNormalized();
        }
        else if (i == (smoothTrailPoints.size() - 1))
            direction = -transform->GetWorldRight();
        else {
            Vec3 dir1 = (smoothTrailPoints[i].position - smoothTrailPoints[i - 1].position).GetNormalized();
            Vec3 dir2 = (smoothTrailPoints[i + 1].position - smoothTrailPoints[i].position).GetNormalized();
            direction = (dir1 + dir2).GetNormalized();
        }

        Vec3 up = direction.Cross(right).GetNormalized();
		float t = i * uvStep;
		Vec3 offset = up * mWidth * lerp(0.2f, 1.f, t); 
		float alpha = smoothTrailPoints[i].alpha * std::lerp(0.0f, 1.f, t); 
		alpha = std::pow(alpha, 0.8f); 
        float uvY = i * uvStep;

        vertices[mVertexCount++] = { smoothTrailPoints[i].position + offset, Vec2(1, uvY), smoothTrailPoints[i].time, Color(1.f,1.f,1.f,alpha) };
        vertices[mVertexCount++] = { smoothTrailPoints[i].position - offset, Vec2(0, uvY), smoothTrailPoints[i].time, Color(1.f,1.f,1.f,alpha) };
    }

	mVertexBuffer->UpdateVertexBuffer(nullptr, mVertexCount);
}

void CTrailRenderer::ResetTrail()
{
	mTrailPoints.clear();
	mTotalTime = 0.0f;
	mVertexCount = 0;

}