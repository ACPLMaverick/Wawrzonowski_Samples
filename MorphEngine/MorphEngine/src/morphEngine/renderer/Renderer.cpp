#include "Renderer.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace resourceManagement;

	namespace renderer
	{
		Renderer::Renderer(const gom::ObjectInitializer& initializer) :
			OctTreeElement(initializer)
		{
		}

		assetLibrary::MMaterial * Renderer::GetDefaultMaterial()
		{
			return ResourceManager::GetInstance()->GetMaterial("DefaultMaterial");
		}

		Renderer::~Renderer()
		{
		}

		void Renderer::Initialize()
		{
			OctTreeElement::Initialize();
			_material = GetDefaultMaterial();
		}

		void Renderer::Shutdown()
		{
			OctTreeElement::Shutdown();
		}

	}
}