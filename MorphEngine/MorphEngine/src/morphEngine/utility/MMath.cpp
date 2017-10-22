#include "MMath.h"
#include <cmath>

namespace morphEngine
{
	namespace utility
	{
		const MFloat32 MMath::_tolerance = 0.00001f;
		const MFloat32 MMath::E = 2.7182818f;
		const MFloat32 MMath::PI = 3.14159265f;
		const MFloat32 MMath::PIOver2 = MMath::PI * 0.5f;
		const MFloat32 MMath::PIOver4 = MMath::PI * 0.25f;
		const MFloat32 MMath::TwoPI = MMath::PI * 2.0f;
		const MFloat32 MMath::Rad2Deg = 360.0f / MMath::TwoPI;
		const MFloat32 MMath::Deg2Rad = MMath::TwoPI / 360.0f;
	}
}