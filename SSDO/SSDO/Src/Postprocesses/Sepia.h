#pragma once
#include "Postprocess.h"

namespace Postprocesses
{
	class Sepia :
		public Postprocess
	{
	protected:

	public:
		Sepia();
		~Sepia();

		virtual void Update() override;
	};
}