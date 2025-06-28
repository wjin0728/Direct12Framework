#pragma once
#include "stdafx.h"

enum class ANIMATION_TYPE : UINT
{
	ONCE,
	LOOP,
	PINGPONG,
	END,

	end
};

enum class ANIMATION_BLEND_TYPE : UINT
{
	ADDITIVE,
	OVERRIDE,
	OVERRIDE_PASSTHROUGH,

	end
};