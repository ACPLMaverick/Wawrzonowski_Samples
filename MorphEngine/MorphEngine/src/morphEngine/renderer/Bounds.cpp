#include "Bounds.h"
#include "resourceManagement/ResourceManager.h"
#include "shaders/ShaderBounds.h"

namespace morphEngine
{
	namespace renderer
	{
		void Bounds::SetShader() const
		{
			_shader->Set();
		}

		void Bounds::UnsetShader() const
		{
			_shader->UnsetGlobal();
		}

		void Bounds::LoadShader()
		{
			_shader =
				reinterpret_cast<shaders::ShaderBounds*>(resourceManagement::ResourceManager::GetInstance()->GetShader("ShaderBounds"));
		}
	}
}