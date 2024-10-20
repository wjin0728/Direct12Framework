#include "stdafx.h"
#include "HeightMapImage.h"

CHeightMap::CHeightMap(LPCTSTR pFileName, int nWidth, int nLength, Vec3 xmf3Scale)
{
	width = nWidth;
	length = nLength;
	scale = xmf3Scale;

	BYTE* pHeightMapPixels = new BYTE[width * length];
	memset(pHeightMapPixels, 0, width * length);

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pHeightMapPixels, (width * length), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	heightMapPixels = new BYTE[width * length];
	for (int y = 0; y < length; y++)
	{
		for (int x = 0; x < width; x++)
		{
			heightMapPixels[x + ((length - 1 - y) * width)] = pHeightMapPixels[x +
				(y * width)];
		}
	}
	if (pHeightMapPixels) delete[] pHeightMapPixels;
}

CHeightMap::~CHeightMap(void)
{
	if (heightMapPixels) delete heightMapPixels;
	heightMapPixels = nullptr;
}

float CHeightMap::GetHeight(float fx, float fz)
{
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= width) || (fz >= length)) {
		return 0.0f;
	}

	int x = (int)fx;
	int z = (int)fz;

	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)heightMapPixels[x + (z * width)];
	float fBottomRight = (float)heightMapPixels[(x + 1) + (z * width)];
	float fTopLeft = (float)heightMapPixels[x + ((z + 1) * width)];
	float fTopRight = (float)heightMapPixels[(x + 1) + ((z + 1) * width)];

	bool bRightToLeft = (z & 1);

	if (bRightToLeft)
	{
		if (fzPercent >= fxPercent)
			fBottomRight = fBottomLeft + (fTopRight - fTopLeft);
		else
			fTopLeft = fTopRight + (fBottomLeft - fBottomRight);
	}
	else
	{
		if (fzPercent < (1.0f - fxPercent))
			fTopRight = fTopLeft + (fBottomRight - fBottomLeft);
		else
			fBottomLeft = fTopLeft + (fBottomRight - fTopRight);
	}

	float fTopHeight = SimpleMath::Flerp(fTopLeft, fTopRight, fxPercent);
	float fBottomHeight = SimpleMath::Flerp(fBottomLeft, fBottomRight, fxPercent);
	float fHeight = SimpleMath::Flerp(fBottomHeight, fTopHeight, fzPercent);

	return fHeight;
}

Vec3 CHeightMap::GetHeightMapNormal(int x, int z)
{
	if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= length))
		return(Vec3(0.0f, 1.0f, 0.0f));

	int nHeightMapIndex = x + (z * width);
	int xHeightMapAdd = (x < (width - 1)) ? 1 : -1;
	int zHeightMapAdd = (z < (length - 1)) ? width : -width;

	Vec3 xmf3Edge1, xmf3Edge2;

	if(z & 1) {
		float y1 = (float)heightMapPixels[nHeightMapIndex] * scale.y;
		float y2 = (float)heightMapPixels[nHeightMapIndex + xHeightMapAdd + zHeightMapAdd] * scale.y;
		float y3 = (float)heightMapPixels[nHeightMapIndex + zHeightMapAdd] * scale.y;

		xmf3Edge1 = Vec3(0.0f, y3 - y1, scale.z);
		xmf3Edge2 = Vec3(scale.x, y2 - y1, scale.z);
	}
	else {
		float y1 = (float)heightMapPixels[nHeightMapIndex] * scale.y;
		float y2 = (float)heightMapPixels[nHeightMapIndex + xHeightMapAdd] * scale.y;
		float y3 = (float)heightMapPixels[nHeightMapIndex + zHeightMapAdd] * scale.y;

		xmf3Edge1 = Vec3(0.0f, y3 - y1, scale.z);
		xmf3Edge2 = Vec3(scale.x, y2 - y1, 0.0f);
	}
	Vec3 xmf3Normal = xmf3Edge1.Cross(xmf3Edge2).GetNormalized();

	return(xmf3Normal);
}