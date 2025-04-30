#pragma once
#include "stdafx.h"

class Terrain
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

	float GetHeight(float fx, float fz);
};

