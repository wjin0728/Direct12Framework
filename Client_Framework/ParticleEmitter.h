#pragma once
#include"stdafx.h"

class CParticleEmitter
{
private:
    EmitterProperties  EmitProperties;
    Color MinStartColor;
    Color MaxStartColor;
    Color MinEndColor;
    Color MaxEndColor;
    float emitRate;
    Vec2 lifeMinMax;
    Vec2 massMinMax;
    Vec4 size;
    Vec3 spread;
    std::wstring TexturePath;
    float totalActiveLifetime;
    Vec4 velocity;
};

