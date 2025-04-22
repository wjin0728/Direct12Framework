#pragma once
#include"stdafx.h"
#include"Component.h"

class CLight : public CComponent
{
protected:
	CBLightsData mLightData{};
	int mLightIndex = -1;
	UINT mCbvOffset = 0;

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

	void Render(std::shared_ptr<class CCamera> camera);

public:

	void SetStrength(const Vec3& _strength) { mLightData.strength = _strength; }
	void SetColor(const Vec3& _color) { mLightData.color = _color; }
	void SetLightType(LIGHT_TYPE _type) { mLightData.type = (UINT8)_type; }
	void SetRange(float _range) { mLightData.range = _range; }
	void SetFallOffStart(float _fallOffStart) { mLightData.fallOffStart = _fallOffStart; }
	void SetFallOffEnd(float _fallOffEnd) { mLightData.fallOffEnd = _fallOffEnd; }
	void SetSpotPower(float _spotPower) { mLightData.spotPower = _spotPower; }
	void SetLightData(const CBLightsData& data) { mLightData = data; }

	CBLightsData GetLightData() const { return mLightData; }
	Vec3 GetColor() const { return mLightData.color; }
	Vec3 GetStrength() const { return mLightData.strength; }
	float GetRange() const { return mLightData.range; }
	float GetFallOffStart() const { return mLightData.fallOffStart; }
	float GetFallOffEnd() const { return mLightData.fallOffEnd; }
	float GetSpotPower() const { return mLightData.spotPower; }
	LIGHT_TYPE GetLightType() const { return (LIGHT_TYPE)mLightData.type; }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



class CDirectionalLight : public CLight
{
public:
	Vec3 direction = Vec3::Zero;
	CDirectionalLight() = default;
	CDirectionalLight(const Vec3& _color, const Vec3& _strength, const Vec3& _direction);

public:
	void SetDirection(const Vec3& _direction);

	void UpdateLightData(CBDirectionalLightInfo& data) const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



class CPointLight : public CLight
{
private:
	Vec3 position = Vec3::Zero;
	float range{};

public:
	CPointLight() = default;
	CPointLight(const Vec3& _color, const Vec3& _strength, const Vec3& _position, float _range);

public:
	void SetLocalPosition(const Vec3& _position);
	void SetRange(float _range);

	void UpdateLightData(CBPointLightInfo& data) const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



class CSpotLight : public CLight
{
private:
	Vec3 direction = Vec3::Zero;
	Vec3 position = Vec3::Zero;
	float range{};
	float fallOffStart{};
	float fallOffEnd{};
	float spotPower{};

public:
	CSpotLight() = default;
	CSpotLight(const Vec3& _color, const Vec3& _strength, const Vec3& _direction, const Vec3& _position,
		float _range, float _fallOffStart, float _fallOffEnd, float _spotPower);

public:
	void SetDirection(const Vec3& _direction);
	void SetLocalPosition(const Vec3& _position);
	void SetRange(float _range);
	void SetfallOffStart(float _fallOffStart);
	void SetfallOffEnd(float _fallOffEnd);
	void SetSpotPower(float _spotPower);

	void UpdateLightData(CBSpotLightInfo& data) const;
};