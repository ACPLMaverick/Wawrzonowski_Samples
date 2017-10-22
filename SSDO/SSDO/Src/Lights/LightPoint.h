#pragma once
#include "GlobalDefines.h"

class Camera;

namespace Lights
{
	__declspec(align(16))
	class LightPoint
	{
	protected:

		XMFLOAT4A _color;
		XMFLOAT3A _position;
		float _range;
		bool _enabled;

	public:

		LightPoint(const XMFLOAT4A& color = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f),
			const XMFLOAT3A& position = XMFLOAT3A(0.0f, 0.0f, 0.0f), float range = 10.0f);
		~LightPoint();

		inline const XMFLOAT4A& GetColor() const { return _color; }
		inline void SetColor(const XMFLOAT4A& col) { _color = col; }
		inline bool GetEnabled() const { return _enabled; }
		inline void SetEnabled(bool enabled) { _enabled = enabled; }
		inline void ToggleEnabled() { _enabled = !_enabled; }
		inline const XMFLOAT3A& GetPosition() const { return _position; }
		inline void SetPosition(const XMFLOAT3A& pos) { _position = pos; }

		inline float GetRange() const { return _range; }
		inline void SetRange(float rng) { _range = rng; }

		inline LightPoint& operator=(const LightPoint& other)
		{
			_color = other._color;
			_position = other._position;
			_range = other._range;
			return *this;
		}

		void ApplyToShader(const LightPoint& other, const Camera& camera);
	};
}