#include "stdafx.h"
#include "Light.h"

CDirectionalLight::CDirectionalLight(const XMFLOAT4& _color, const XMFLOAT3& _strength, const XMFLOAT3& _direction) : CLight(_color, _strength), direction(_direction)
{
	direction = Vector3::Normalize(direction);
}

void CDirectionalLight::SetDirection(const XMFLOAT3& _direction)
{
	direction = _direction;
	direction = Vector3::Normalize(direction);
}

void CDirectionalLight::UpdateLightData(CBDirectionalLightInfo& data) const
{
	data.color = color;
	data.strength = strength;
	data.direction = direction;
}

CPointLight::CPointLight(const XMFLOAT4& _color, const XMFLOAT3& _strength, const XMFLOAT3& _position, float _range)
	: CLight(_color, _strength), position(_position), range(_range)
{
}

void CPointLight::Setposition(const XMFLOAT3& _position)
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

CSpotLight::CSpotLight(const XMFLOAT4& _color, const XMFLOAT3& _strength, const XMFLOAT3& _direction, const XMFLOAT3& _position,
	float _range, float _fallOffStart, float _fallOffEnd, float _spotPower)
	: CLight(_color, _strength), direction(_direction), position(_position), range(_range) , spotPower(_spotPower)
{
	direction = Vector3::Normalize(direction);
	fallOffStart = cos(DegreeToRadian(_fallOffStart));
	fallOffEnd = cos(DegreeToRadian(_fallOffEnd));
}

void CSpotLight::SetDirection(const XMFLOAT3& _direction)
{
	direction = _direction;
	direction = Vector3::Normalize(direction);
}

void CSpotLight::Setposition(const XMFLOAT3& _position)
{
	position = _position;
}

void CSpotLight::SetRange(float _range)
{
	range = _range;
}

void CSpotLight::SetfallOffStart(float _fallOffStart)
{
	fallOffStart = cos(DegreeToRadian(_fallOffStart));
}

void CSpotLight::SetfallOffEnd(float _fallOffEnd)
{
	fallOffEnd = cos(DegreeToRadian(_fallOffEnd));
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
