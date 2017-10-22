#include "core/Engine.h"
#include "debugging/Console.h"
#include "tests/delegateTests.h"
#include "tests/memoryTests.h"
#include "tests/rendererTests.h"

#include <iostream>
#include <conio.h>
#include <functional>
#include <vector>

void CreateNewScene()
{
	morphEngine::gom::SceneManager::GetInstance()->OpenScene("Scenes\\testScene.msc");
	morphEngine::gom::Scene* scene = morphEngine::gom::SceneManager::GetInstance()->GetScene();

	Handle<morphEngine::gom::GameObject> go1;
	morphEngine::gom::GameObject::Create("Go1", true, true, go1);
	
	scene->SaveScene();
}

int main()
{
	// This block is to avoid heap allocation and keep memory dump still working properly.
	{
		morphEngine::core::Engine* engine = morphEngine::core::Engine::GetInstance();

		engine->Init();
		RendererTestsInit();
		{
			engine->Run();
		}
		RendererTestsShutdown();
		engine->Shutdown();

		engine->DestroyInstance();
	}

	std::cout << (_CrtDumpMemoryLeaks() != 0 ? "ERROR: Memory not cleaned up" : "LOG: Memory succesfully cleaned up") << std::endl;
	
	_getch();
	return 0;
}