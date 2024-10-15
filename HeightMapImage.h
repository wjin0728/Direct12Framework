#pragma once
#include"stdafx.h"

class CHeightMapImage
{
private:
	BYTE* heightMapPixels;
	int width;
	int length;
	Vec3 scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, Vec3 xmf3Scale);
	~CHeightMapImage(void);

	float GetHeight(float fx, float fz);
	Vec3 GetHeightMapNormal(int x, int z);
	Vec3 GetScale() { return scale; }
	const BYTE* GetHeightMapPixels() { return heightMapPixels; }
	int GetHeightMapWidth() { return width; }
	int GetHeightMapLength() { return length; }
};

