#pragma once
#include"stdafx.h"

class CLight
{
protected:
	XMFLOAT4 color;
	XMFLOAT3 strength;
	float padding;

public:
	CLight() = default;
	CLight(const XMFLOAT4& _color, const XMFLOAT3& _strength) : color(_color), strength(_strength) {}

public:
	void SetStrength(const XMFLOAT3& _strength) { strength = _strength; }
	void SetColor(const XMFLOAT4& _color) { color = _color; }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



class CDirectionalLight : public CLight
{
private:
	XMFLOAT3 direction;

public:
	CDirectionalLight() = default;
	CDirectionalLight(const XMFLOAT4& _color, const XMFLOAT3& _strength, const XMFLOAT3& _direction);

public:
	void SetDirection(const XMFLOAT3& _direction);

	void UpdateLightData(CBDirectionalLightInfo& data) const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



class CPointLight : public CLight
{
private:
	XMFLOAT3 position;
	float range;

public:
	CPointLight() = default;
	CPointLight(const XMFLOAT4& _color, const XMFLOAT3& _strength, const XMFLOAT3& _position, float _range);

public:
	void Setposition(const XMFLOAT3& _position);
	void SetRange(float _range);

	void UpdateLightData(CBPointLightInfo& data) const;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////



class CSpotLight : public CLight
{
private:
	XMFLOAT3 direction;
	XMFLOAT3 position;
	float range;
	float fallOffStart;
	float fallOffEnd;
	float spotPower;

public:
	CSpotLight() = default;
	CSpotLight(const XMFLOAT4& _color, const XMFLOAT3& _strength, const XMFLOAT3& _direction, const XMFLOAT3& _position,
		float _range, float _fallOffStart, float _fallOffEnd, float _spotPower);

public:
	void SetDirection(const XMFLOAT3& _direction);
	void Setposition(const XMFLOAT3& _position);
	void SetRange(float _range);
	void SetfallOffStart(float _fallOffStart);
	void SetfallOffEnd(float _fallOffEnd);
	void SetSpotPower(float _spotPower);

	void UpdateLightData(CBSpotLightInfo& data) const;
};