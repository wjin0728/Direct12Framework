#pragma once
#include"stdafx.h"
#include"Component.h"

class CLight
{
protected:
	Vec3 color = { 1.f,1.f,1.f };
	Vec3 strength = { 1.f,1.f,1.f };
	float padding{};

public:
	CLight() {}
	CLight(const Vec3& _color, const Vec3& _strength) : color(_color), strength(_strength) {}

public:
	void SetStrength(const Vec3& _strength) { strength = _strength; }
	void SetColor(const Vec3& _color) { color = _color; }

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