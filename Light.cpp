#include "stdafx.h"
#include "Light.h"

CDirectionalLight::CDirectionalLight(const Vec3& _color, const Vec3& _strength, const Vec3& _direction) : CLight(_color, _strength), direction(_direction)
{
	direction = direction.GetNormalized();
}

void CDirectionalLight::SetDirection(const Vec3& _direction)
{
	direction = _direction;
	direction = direction.GetNormalized();
}

void CDirectionalLight::UpdateLightData(CBDirectionalLightInfo& data) const
{
	data.color = color;
	data.strength = strength;
	data.direction = direction;
}

CPointLight::CPointLight(const Vec3& _color, const Vec3& _strength, const Vec3& _position, float _range)
	: CLight(_color, _strength), position(_position), range(_range)
{
}

void CPointLight::SetLocalPosition(const Vec3& _position)
{
	position = _position;
}

void CPointLight::SetRange(float _range)
{
	range = _range;
}

void CPointLight::UpdateLightData(CBPointLightInfo& data) const
{
	data.color = color;
	data.strength = strength;
	data.position = position;
	data.range = range;
}

CSpotLight::CSpotLight(const Vec3& _color, const Vec3& _strength, const Vec3& _direction, const Vec3& _position,
	float _range, float _fallOffStart, float _fallOffEnd, float _spotPower)
	: CLight(_color, _strength), direction(_direction), position(_position), range(_range) , spotPower(_spotPower)
{
	direction = direction.GetNormalized();
	fallOffStart = cos(XMConvertToRadians(_fallOffStart));
	fallOffEnd = cos(XMConvertToRadians(_fallOffEnd));
}

void CSpotLight::SetDirection(const Vec3& _direction)
{
	direction = _direction;
	direction = direction.GetNormalized();
}

void CSpotLight::SetLocalPosition(const Vec3& _position)
{
	position = _position;
}

void CSpotLight::SetRange(float _range)
{
	range = _range;
}

void CSpotLight::SetfallOffStart(float _fallOffStart)
{
	fallOffStart = cos(XMConvertToRadians(_fallOffStart));
}

void CSpotLight::SetfallOffEnd(float _fallOffEnd)
{
	fallOffEnd = cos(XMConvertToRadians(_fallOffEnd));
}

void CSpotLight::SetSpotPower(float _spotPower)
{
	spotPower = _spotPower;
}

void CSpotLight::UpdateLightData(CBSpotLightInfo& data) const
{
	data.color = color;
	data.strength = strength;
	data.direction = direction;
	data.position = position;
	data.range = range;
	data.fallOffStart = fallOffStart;
	data.fallOffEnd = fallOffEnd;
	data.spotPower = spotPower;
}
