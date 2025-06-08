#pragma once
#include"stdafx.h"
#include"Component.h"

class CLight : public CComponent
{
protected:
	CBLightsData mLightData{};
	int mLightIndex = -1;
	int mCbvIdx = -1;
	UINT mCbvOffset{};

	UINT mDirtyFrame = FRAME_RESOURCE_COUNT + 1;
	
	std::shared_ptr<CCamera> mainCamera{};
	std::shared_ptr<CCamera> lightCam{};
	static std::array< std::shared_ptr<class CMesh>, 3> volumes;

	BoundingOrientedBox mFrustumBoundWS{};


public:
	CLight() {}
	CLight(const CBLightsData& data) { mLightData = data; }

	CLight(const CLight& other) : CComponent(other)
	{
		mLightData = other.mLightData;
	}
	virtual ~CLight();

public:
	virtual std::shared_ptr<CComponent> Clone() override;

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	void Render(std::shared_ptr<class CRenderTargetGroup> renderTarget);

	void BindLightDataToShader();

public:
	static void SetVolumes();

	void SetLightCam(std::shared_ptr<CCamera> cam) { lightCam = cam; }
	void SetStrength(const float& _strength) { mLightData.strength = _strength; }
	void SetColor(const Vec3& _color) { mLightData.color = _color; }
	void SetLightType(LIGHT_TYPE _type) { mLightData.type = (UINT8)_type; }
	void SetRange(float _range) { mLightData.range = _range; }
	void SetSpotAngle(float _spotAngle) { mLightData.spotAngle = _spotAngle; }
	void SetInnerSpotAngle(float _innerSpotAngle) { mLightData.innerSpotAngle = _innerSpotAngle; }
	void SetPosition(const Vec3& _position) { mLightData.position = _position; }
	void SetDirection(const Vec3& _direction) { mLightData.direction = _direction; }
	void SetLightData(const CBLightsData& data) { mLightData = data; }

	CBLightsData GetLightData() const { return mLightData; }
	Vec3 GetColor() const { return mLightData.color; }
	float GetStrength() const { return mLightData.strength; }
	float GetRange() const { return mLightData.range; }
	float GetSpotAngle() const { return mLightData.spotAngle; }
	float GetInnerSpotAngle() const { return mLightData.innerSpotAngle; }
	Vec3 GetWorldPosition() const { return mLightData.position; }
	Vec3 GetWorldDirection() const { return mLightData.direction; }
	LIGHT_TYPE GetLightType() const { return (LIGHT_TYPE)mLightData.type; }

private:
	void UpdateLightCamData();
	void UpdateLightViewBound();
};
