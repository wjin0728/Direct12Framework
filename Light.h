#pragma once
#include"stdafx.h"
#include"Component.h"

class CLight : public CComponent
{
protected:
	CBLightsData lightData{};

public:
	CLight() : CComponent(COMPONENT_TYPE::LIGHT) {}
	CLight(const CBLightsData& data) : CComponent(COMPONENT_TYPE::LIGHT) { lightData = data; }

	CLight(const CLight& other) : CComponent(other)
	{
		lightData = other.lightData;
	}

public:
	virtual std::shared_ptr<CComponent> Clone() override;

	virtual void Awake() override;
	virtual void Start() override;

	virtual void Update() override;
	virtual void LateUpdate() override;

	void Render();

public:
	void SetStrength(const Vec3& _strength) { lightData.strength = _strength; }
	void SetColor(const Vec3& _color) { lightData.color = _color; }
	void SetLightType(LIGHT_TYPE _type) { lightData.type = (UINT8)_type; }
	void SetRange(float _range) { lightData.range = _range; }
	void SetFallOffStart(float _fallOffStart) { lightData.fallOffStart = _fallOffStart; }
	void SetFallOffEnd(float _fallOffEnd) { lightData.fallOffEnd = _fallOffEnd; }
	void SetSpotPower(float _spotPower) { lightData.spotPower = _spotPower; }
	void SetPosition(const Vec3& _position) { lightData.position = _position; }
	void SetDirection(const Vec3& _direction) { lightData.direction = _direction; }
	void SetLightData(const CBLightsData& data) { lightData = data; }

	CBLightsData GetLightData() const { return lightData; }
	Vec3 GetColor() const { return lightData.color; }
	Vec3 GetStrength() const { return lightData.strength; }
	Vec3 GetPosition() const { return lightData.position; }
	Vec3 GetDirection() const { return lightData.direction; }
	float GetRange() const { return lightData.range; }
	float GetFallOffStart() const { return lightData.fallOffStart; }
	float GetFallOffEnd() const { return lightData.fallOffEnd; }
	float GetSpotPower() const { return lightData.spotPower; }
	LIGHT_TYPE GetLightType() const { return (LIGHT_TYPE)lightData.type; }

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