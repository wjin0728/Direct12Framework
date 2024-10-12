#pragma once
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <stdlib.h>

#define DegreeToRadian(x)		float((x)*3.141592654f/180.0f)

#define EPSILON					1.0e-6f

using namespace DirectX;
using namespace DirectX::PackedVector;

float RandF(float fMin, float fMax);

XMVECTOR RandomUnitVectorOnSphere();

static bool IsZero(float fValue) {
	return((fabsf(fValue) < EPSILON));
}

static bool IsEqual(float fA, float fB) {
	return(::IsZero(fA - fB));
}

static float InverseSqrt(float fValue) {
	return 1.0f / sqrtf(fValue);
}

static float Flerp(float a, float b, float t) {
	return ((1.f - t) * a + t * b);
}


namespace Vector3
{
	static bool IsZero(XMFLOAT3& xmf3Vector)
	{
		if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z))
			return(true);
		return(false);
	}

	static XMFLOAT3 XMVectorToFloat3(const XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	static XMFLOAT3 ScalarProduct(const XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	static XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	static XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	static XMFLOAT3 Subtract(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	static float DotProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	static XMFLOAT3 CrossProduct(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat3(&xmf3Vector1)), XMVector3Normalize(XMLoadFloat3(&xmf3Vector2)))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMVector3Normalize(XMLoadFloat3(&xmf3Vector1)), XMVector3Normalize(XMLoadFloat3(&xmf3Vector2))));
		return(xmf3Result);
	}

	static XMFLOAT3 ClampLength(const XMFLOAT3& xmf3Vector1, float LenthMin, float LenthMax)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3ClampLength(XMLoadFloat3(&xmf3Vector1), LenthMin, LenthMax));
		return(xmf3Result);
	}

	static XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	static float Length(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}

	static float Distance(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMVectorSubtract(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		return(xmf3Result.x);
	}

	static float Angle(const XMVECTOR xmvVector1, const XMVECTOR xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return (XMVectorGetX(XMVectorACos(xmvAngle)));
	}

	static float Angle(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		return (Angle(XMVector3Normalize(XMLoadFloat3(&xmf3Vector1)), XMVector3Normalize(XMLoadFloat3(&xmf3Vector2))));
	}

	static XMFLOAT3 TransformNormal(const XMFLOAT3 xmf3Vector, const XMMATRIX xmxm4x4Transform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	static XMFLOAT3 TransformNormal(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformNormal(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}

	static XMFLOAT3 TransformCoord(const XMFLOAT3 xmf3Vector, const XMMATRIX xmxm4x4Transform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	static XMFLOAT3 TransformCoord(const XMFLOAT3& xmf3Vector, const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}

	static XMFLOAT3 VectorLerp(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2, float t)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVectorLerp(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2), t));
		return(xmf3Result);
	}
}

namespace Vector4
{
	static XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}

	static XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}

namespace Matrix4x4
{
	static XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Scale(float x, float y, float z)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixScaling(x, y, z));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Scale(const XMFLOAT3& xmf3Vector)
	{
		return(Scale(xmf3Vector.x, xmf3Vector.y, xmf3Vector.z));
	}

	static XMFLOAT4X4 Translate(float x, float y, float z)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranslation(x, y, z));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Multiply(const XMFLOAT4X4& xmmtx4x4Matrix1, const XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Multiply(const XMMATRIX& xmmtxMatrix1, const XMMATRIX& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * xmmtx4x4Matrix2);
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 RotationYawPitchRoll(float fPitch, float fYaw, float fRoll)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll)));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 RotationAxis(const XMFLOAT3& xmf3Axis, float fAngle)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixRotationAxis(XMLoadFloat3(&xmf3Axis), XMConvertToRadians(fAngle)));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Inverse(const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(nullptr, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 Transpose(const XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 PerspectiveFovLH(float fFovAngleY, float fAspectRatio, float fNearZ, float fFarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngleY), fAspectRatio, fNearZ, fFarZ));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 LookAtLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookAtPosition, const XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}

	static XMFLOAT4X4 LookToLH(const XMFLOAT3& xmf3EyePosition, const XMFLOAT3& xmf3LookTo, const XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookToLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookTo), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}

namespace Triangle
{
	static bool Intersect(const XMFLOAT3& xmf3RayPosition, const XMFLOAT3& xmf3RayDirection,
		const XMFLOAT3& v0, const XMFLOAT3& v1, const XMFLOAT3& v2, float& fHitDistance)
	{
		return(TriangleTests::Intersects(XMLoadFloat3(&xmf3RayPosition), XMLoadFloat3(&xmf3RayDirection),
			XMLoadFloat3(&v0), XMLoadFloat3(&v1), XMLoadFloat3(&v2), fHitDistance));
	}
}

namespace Plane
{
	static XMFLOAT4 Normalize(const XMFLOAT4& xmf4Plane)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMPlaneNormalize(XMLoadFloat4(&xmf4Plane)));
		return(xmf4Result);
	}
}

inline XMFLOAT3 operator+(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
	return(xmf3Result);
}

inline XMFLOAT3 operator*(float scalar, const XMFLOAT3& xmf3Vector)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * scalar);
	return(xmf3Result);
}

inline XMFLOAT3 operator*(const XMFLOAT3& xmf3Vector, float scalar)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * scalar);
	return(xmf3Result);
}

inline XMFLOAT3 operator*(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
{
	XMFLOAT3 xmf3Result = { xmf3Vector1.x * xmf3Vector2.x, xmf3Vector1.y * xmf3Vector2.y, xmf3Vector1.z * xmf3Vector2.z };
	return(xmf3Result);
}

inline XMFLOAT3 operator-(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
{
	XMFLOAT3 xmf3Result;
	XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
	return(xmf3Result);
}

