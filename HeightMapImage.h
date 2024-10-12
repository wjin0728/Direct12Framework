#pragma once
#include"stdafx.h"

class CHeightMapImage
{
private:
	BYTE* heightMapPixels;
	int width;
	int length;
	XMFLOAT3 scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	float GetHeight(float fx, float fz);
	XMFLOAT3 GetHeightMapNormal(int x, int z);
	XMFLOAT3 GetScale() { return scale; }
	const BYTE* GetHeightMapPixels() { return heightMapPixels; }
	int GetHeightMapWidth() { return width; }
	int GetHeightMapLength() { return length; }
};

