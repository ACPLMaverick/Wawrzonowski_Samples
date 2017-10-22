#pragma once

#include "GlobalDefines.h"
#include <vector>

class Shader;
class Camera;
class GBuffer;

namespace Postprocesses
{
	class Postprocess
	{
	protected:

		std::vector<Shader*> _shaders;
		bool _bEnabled = true;

	public:

		Postprocess();
		~Postprocess();

		virtual void Update() = 0;
		virtual void SetPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const;
		virtual void AfterPass(GBuffer& gBuffer, const Camera& camera, int32_t passIndex = 0) const;
		virtual inline int GetPassCount() const { return 1; }
		virtual inline Shader* GetShader(int32_t passIndex = 0) const { return const_cast<Shader*>(_shaders[passIndex]); }
		inline bool GetEnabled() const { return _bEnabled; }
		inline void SetEnabled(bool enabled) { _bEnabled = enabled; }
		inline void ToggleEnabled() { _bEnabled = !_bEnabled; }
	};
}