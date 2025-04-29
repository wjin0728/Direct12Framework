#pragma once
#include"stdafx.h"
#include"Component.h"

class CLight : public CComponent
{
protected:
	CBLightsData mLightData{};
	int mLightIndex = -1;
	UINT mCbvOffset = 0;
	
	std::shared_ptr<CCamera> mainCamera{};
	std::shared_ptr<CCamera> lightCam{};
	static std::array< std::shared_ptr<class CMesh>, 3> volumes;
public:
	CLight() : CComponent(COMPONENT_TYPE::LIGHT) {}
	CLight(const CBLightsData& data) : CComponent(COMPONENT_TYPE::LIGHT) { mLightData = data; }

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

public:
	static void SetVolumes();

	void SetLightCam(std::shared_ptr<CCamera> cam) { lightCam = cam; }
	void SetStrength(const float& _strength) { mLightData.strength = _strength; }
	void SetColor(const Vec3& _color) { mLightData.color = _color; }
	void SetLightType(LIGHT_TYPE _type) { mLightData.type = (UINT8)_type; }
	void SetRange(float _range) { mLightData.range = _range; }
	void SetSpotAngle(float _spotAngle) { mLightData.spotAngle = _spotAngle; }
	void SetInnerSpotAngle(float _innerSpotAngle) { mLightData.innerSpotAngle = _innerSpotAngle; }
	void SetLightData(const CBLightsData& data) { mLightData = data; }

	CBLightsData GetLightData() const { return mLightData; }
	Vec3 GetColor() const { return mLightData.color; }
	float GetStrength() const { return mLightData.strength; }
	Vec3 GetPosition() const;
	Vec3 GetDirection() const;
	float GetRange() const { return mLightData.range; }
	float GetSpotAngle() const { return mLightData.spotAngle; }
	float GetInnerSpotAngle() const { return mLightData.innerSpotAngle; }
	LIGHT_TYPE GetLightType() const { return (LIGHT_TYPE)mLightData.type; }

	void UpdateLightData(CBSpotLightInfo& data) const;
};
