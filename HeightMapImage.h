#pragma once
#include"stdafx.h"

class CHeightMap
{
private:
	BYTE* heightMapPixels;
	int width;
	int length;
	Vec3 scale;

public:
	CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, Vec3 xmf3Scale);
	~CHeightMap(void);

	float GetHeight(float fx, float fz);
	Vec3 GetHeightMapNormal(int x, int z);
	Vec3 GetScale() { return scale; }
	const BYTE* GetHeightMapPixels() { return heightMapPixels; }
	int GetHeightMapWidth() { return width; }
	int GetHeightMapLength() { return length; }
};

