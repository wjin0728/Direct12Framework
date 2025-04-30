#include "Object.h"

void Object::LoadHeightMap(const std::string& fileName)
{
    UINT heightMapSize = resolution * resolution;

    std::vector<float> heightMap;
    std::ifstream file("..\\Resources\\Textures\\" + fileName + ".raw", std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    heightMap.resize(heightMapSize);
    file.read(reinterpret_cast<char*>(heightMap.data()), heightMapSize * sizeof(float));

    heightData.resize(heightMapSize);
    for (UINT y = 0; y < resolution; y++) {
        for (UINT x = 0; x < resolution; x++) {
            heightData[x + ((resolution - 1 - y) * resolution)] = heightMap[x + (y * resolution)];
        }
    }

	cout << heightData.size() << endl;
}

void Object::LoadNavMap(const std::string& fileName)
{
    UINT NavMapSize = navmap_resolution * navmap_resolution;

    std::vector<BYTE> NavMap;
    std::ifstream file("..\\Resources\\Textures\\" + fileName + ".raw", std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    NavMap.resize(NavMapSize);
    file.read(reinterpret_cast<char*>(NavMap.data()), NavMapSize * sizeof(BYTE));

    mNavMapData.resize(NavMapSize);
    for (UINT y = 0; y < navmap_resolution; y++) {
        for (UINT x = 0; x < navmap_resolution; x++) {
            mNavMapData[x + ((navmap_resolution - 1 - y) * navmap_resolution)] = NavMap[x + (y * navmap_resolution)];
        }
    }

	cout << mNavMapData.size() << endl;
}

float Object::GetHeight(float fx, float fz)
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
