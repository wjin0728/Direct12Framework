#pragma once
#include "Renderer.h"
#include"VertexBuffer.h"
#include"ParticleEmitter.h"


struct TrailPoint
{
    Vec3 position; 
    float time;
	float alpha;
};

struct TrailVertex
{
    Vec3 position;
	Vec2 uv; 
    float time;
	Color color;
};

class CTrailRenderer :
    public CRenderer
{
private:
	float mWidth; // Width of the trail
	float mDuration;
	float mTotalTime; 
	float mMinDstance; 

	int mMaxPoints;
	int mVertexCount; 

	bool mISViewAligned = false; 

	std::vector<TrailPoint> mTrailPoints;
	CVertexBuffer* mVertexBuffer{};

	std::shared_ptr<class CTexture> mBlendMaskTexture = nullptr;
    
public:
	bool mActive = false; 
	CTrailRenderer(bool isViewAligned = false);
	CTrailRenderer(const CTrailRenderer& other);
    virtual ~CTrailRenderer();

    virtual void Awake() override;
    virtual void Start() override;

    virtual void Update() override;
    virtual void LateUpdate() override;

    virtual void Render(class CCamera* camera, int pass = 0);

    virtual std::shared_ptr<CComponent> Clone() {
        return std::make_shared<CTrailRenderer>(*this);
	}

public:
	void SetWidth(float width) { mWidth = width; }
	void SetMaxPoints(int maxPoints) { mMaxPoints = maxPoints; }
	void SetTotalTime(float time) { mTotalTime = time; }
	void SetActive(bool isActive) { mActive = isActive; }
	bool GetIsActive() const { return mIsActive; }
	int GetMaxPoints() const { return mMaxPoints; }
	void SetMinDistance(float distance) { mMinDstance = distance; }
	float GetMinDistance() const { return mMinDstance; }
	void SetDuration(float duration) { mDuration = duration; }
    float GetWidth() const { return mWidth; }
	float GetDuration() const { return mDuration; }
	void SetBlendMaskTexture(const std::string& name);
	void SetColor(const Color& color);


	void UpdateVertices();
	void ResetTrail();
};

