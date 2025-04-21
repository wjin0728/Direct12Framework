#pragma once
#include "stdafx.h"

class Object
{
	UINT resolution{};
	Vec3 scale = { 1.f, 1.f, 1.f };
	UINT navmap_resolution{};
	Vec3 Offset = { 0.f, 0.f, 0.f };

public:
	std::vector<float> heightData;
	std::vector<BYTE> mNavMapData{};

	void SetScale(float x, float y, float z) { scale = Vec3(x, y, z); };
	Vec3 GetScale() { return scale; };

	void SetResolution(UINT res) { resolution = res; };
	UINT GetResolution() { return resolution; };

	void SetNavMapResolution(UINT res) { navmap_resolution = res; };
	UINT GetNavMapResolution() { return navmap_resolution; };

	void SetOffset(float x, float y, float z) { Offset = Vec3(x, y, z); };
	Vec3 GetOffset() { return Offset; };

	void LoadHeightMap(const std::string& fileName);
	void LoadNavMap(const std::string& fileName);

	float GetHeight(float fx, float fz)
	{
		float localX = fx - Offset.x;
		float localZ = fz - Offset.z;

		if (localX < 0.0f || localZ < 0.0f || localX >= scale.x || localZ >= scale.z) {
			return 0.0f;
		}

		float xIndex = localX / (scale.x / (resolution));
		float zIndex = localZ / (scale.z / (resolution));
		zIndex = resolution - zIndex;


		int x = static_cast<int>(xIndex);
		int z = static_cast<int>(zIndex);

		float fxPercent = xIndex - x;
		float fzPercent = zIndex - z;

		float fBottomLeft = heightData[x + (z * resolution)] * scale.y;
		float fBottomRight = heightData[(x + 1) + (z * resolution)] * scale.y;
		float fTopLeft = heightData[x + ((z + 1) * resolution)] * scale.y;
		float fTopRight = heightData[(x + 1) + ((z + 1) * resolution)] * scale.y;

		if (fzPercent >= fxPercent) {
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		}
		else {
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
		}

		float fTopHeight = SimpleMath::Flerp(fTopLeft, fTopRight, fxPercent);
		float fBottomHeight = SimpleMath::Flerp(fBottomLeft, fBottomRight, fxPercent);
		float fHeight = SimpleMath::Flerp(fBottomHeight, fTopHeight, fzPercent);

		return fHeight;
	}
};

