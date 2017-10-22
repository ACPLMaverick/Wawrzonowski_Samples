#pragma once

#include "core\GlobalDefines.h"
#include "utility\MEnum.h"

namespace morphEngine
{
	namespace physics
	{
		DECLARE_MENUM(Constraints,	FREE = 0x00000000,
									POSITION_X = 0x00000001,
									POSITION_Y = 0x00000010,
									POSITION_Z = 0x00000100,
									ROTATION_X = 0x00001000,
									ROTATION_Y = 0x00010000,
									ROTATION_Z = 0x00100000,
									POSITION_XY = POSITION_X | POSITION_Y,
									POSITION_XZ = POSITION_X | POSITION_Z,
									POSITION_YZ = POSITION_Y | POSITION_Z,
									POSITION_ALL = POSITION_X | POSITION_Y | POSITION_Z,
									ROTATION_XY = ROTATION_X | ROTATION_Y,
									ROTATION_XZ = ROTATION_X | ROTATION_Z,
									ROTATION_YZ = ROTATION_Y | ROTATION_Z,
									ROTATION_ALL = ROTATION_X | ROTATION_Y | ROTATION_Z,
									ALL = POSITION_ALL | ROTATION_ALL)
	}
}