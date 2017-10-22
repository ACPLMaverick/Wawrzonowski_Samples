#pragma once

#include "Joint.h"

namespace morphEngine
{
	namespace physics
	{
		class FixedJoint : public Joint
		{
		public:
			FixedJoint();
			~FixedJoint();
		};
	}
}