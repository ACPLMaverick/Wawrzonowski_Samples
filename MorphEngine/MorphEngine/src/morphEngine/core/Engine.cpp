#include "Engine.h"
#include "debugging/GlobalDefines.h"
#include "debugging/Debug.h"
#include "MessageSystem.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"
#include "memoryManagement/HandleInfo.h"
#include "gom/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "gom/Scene.h"
#include "audio\AudioManager.h"

// testing
#include "gom/BaseObject.h"
#include <iostream>
#include <iomanip>
#include <string>

#ifdef PLATFORM_WINDOWS
#include "ITime.h"
#include "input/WindowsInputManager.h"
#endif

#include "reflection\DummyInit.h"

namespace morphEngine
{
	namespace core
	{
		Engine::Engine() : _globalTime(0), _globalFileSystem(0), _bIsGomUpdate(false), _bShouldQuit(false), _bIsEditorMode(false), _window(0)
		{
			reflection::DummyInit();
			reflection::Assembly::GetInstance()->Initialize();
		}

		Engine::~Engine()
		{
			gom::ObjectInitializer::ShutdownIDsArray();
			// destroy singleton instances
#ifdef PLATFORM_WINDOWS
			input::WindowsInputManager::DestroyInstance();
#endif
			gom::SceneManager::DestroyInstance();
			resourceManagement::ResourceManager::DestroyInstance();
			renderer::RenderingManager::DestroyInstance();
			physics::PhysicsManager::DestroyInstance();
			audio::AudioManager::DestroyInstance();
			reflection::Assembly::DestroyInstance();
			memoryManagement::HandleInfo::GetInstance()->IsShutdown = true;
			memoryManagement::MemoryManager::GetInstance()->Shutdown();
			memoryManagement::MemoryManager::DestroyInstance();
			memoryManagement::HandleInfo::DestroyInstance();
		}

		void Engine::Init()
		{
			_bIsEditorMode = false;

			debugging::Debug::Init();

			_globalTime = ITime::GetITimeInstance();
			_globalTime->Init();
			_globalFileSystem = resourceManagement::fileSystem::FileSystem::GetFileSystemInstance();
			MessageSystem::OnWindowDestroy += new Event<void>::ClassRawDelegate<Engine>(this, &Engine::Quit);
			MessageSystem::OnWindowQuit += new Event<void>::ClassRawDelegate<Engine>(this, &Engine::Quit);

			_window = renderer::Window::GetWindowInstance();
			if(_window)
			{
				_window->OpenWindow("Test window", 800, 600, false);
				_window->Show();
			}

			memoryManagement::HandleInfo::GetInstance()->IsShutdown = false;
#ifdef PLATFORM_WINDOWS
			input::WindowsInputManager::GetInstance()->Initialize();
#endif
			physics::PhysicsManager::GetInstance()->Initialize();
			audio::AudioManager::GetInstance()->Initialize();
			renderer::RenderingManager::GetInstance()->Initialize();
			resourceManagement::ResourceManager::GetInstance()->Initialize();
			gom::SceneManager::GetInstance()->Initialize();
		}

		void Engine::Shutdown()
		{
			gom::SceneManager::GetInstance()->Shutdown();
			renderer::RenderingManager::GetInstance()->Shutdown();
			resourceManagement::ResourceManager::GetInstance()->Shutdown();
			physics::PhysicsManager::GetInstance()->Shutdown();
			audio::AudioManager::GetInstance()->Shutdown();

			if(_window)
			{
				_window->ShutdownWindow();
				delete _window;
				_window = 0;
			}

			if(_globalTime)
			{
				delete _globalTime;
				_globalTime = 0;
			}

			// shutdown managers
			input::WindowsInputManager::GetInstance()->Shutdown();

			MessageSystem::Shutdown();

			debugging::Debug::Shutdown();
			reflection::Assembly::GetInstance()->Shutdown();

			if(_globalFileSystem)
			{
				_globalFileSystem->Shutdown();
				delete _globalFileSystem;
				_globalFileSystem = 0;
			}
		}

		void Engine::Run()
		{
			MFloat32 fixedDeltaTime = 1.0f / 60.0f;

			while(!_bShouldQuit)
			{
				MessageSystem::GatherMessages();
				_globalTime->Tick();
#ifdef PLATFORM_WINDOWS
				input::WindowsInputManager::GetInstance()->Update();
#endif

				_bIsGomUpdate = true;

				gom::SceneManager::GetInstance()->Update();

				_bIsGomUpdate = false;

				renderer::RenderingManager::GetInstance()->Draw();

				physics::PhysicsManager::GetInstance()->Update(fixedDeltaTime);

				physics::PhysicsManager::GetInstance()->ResolveEvents();

				audio::AudioManager::GetInstance()->Update();

				memoryManagement::MemoryManager::GetInstance()->Update();		// MemoryManager should be the last subsystem to update.
			}
		}

		void Engine::InitEditor()
		{
			_bIsEditorMode = true;

			_globalTime = ITime::GetITimeInstance();
			_globalTime->Init();
			_globalFileSystem = resourceManagement::fileSystem::FileSystem::GetFileSystemInstance();

			// no window for you, editor

			_window = renderer::Window::GetWindowInstance();

			memoryManagement::HandleInfo::GetInstance()->IsShutdown = false;
#ifdef PLATFORM_WINDOWS
			input::WindowsInputManager::GetInstance()->Initialize();
#endif
			physics::PhysicsManager::GetInstance()->Initialize();
			audio::AudioManager::GetInstance()->Initialize();
			renderer::RenderingManager::GetInstance()->Initialize();
			resourceManagement::ResourceManager::GetInstance()->Initialize();
			gom::SceneManager::GetInstance()->InitializeEditor();
		}

		void Engine::ShutdownEditor()
		{
			gom::SceneManager::GetInstance()->ShutdownEditor();
			renderer::RenderingManager::GetInstance()->Shutdown();
			resourceManagement::ResourceManager::GetInstance()->Shutdown();
			physics::PhysicsManager::GetInstance()->Shutdown();
			audio::AudioManager::GetInstance()->Shutdown();

			if (_globalTime)
			{
				delete _globalTime;
				_globalTime = 0;
			}

			// shutdown managers
			input::WindowsInputManager::GetInstance()->Shutdown();

			MessageSystem::Shutdown();

			debugging::Debug::Shutdown();
			reflection::Assembly::GetInstance()->Shutdown();

			if (_globalFileSystem)
			{
				_globalFileSystem->Shutdown();
				delete _globalFileSystem;
				_globalFileSystem = 0;
			}
		}

		void Engine::RunEditor()
		{
			//while (!_bShouldQuit)
			{
				MessageSystem::GatherMessages();
				_globalTime->Tick();
#ifdef PLATFORM_WINDOWS
				input::WindowsInputManager::GetInstance()->Update();
#endif
				gom::SceneManager::GetInstance()->UpdateEditor();

				renderer::RenderingManager::GetInstance()->DrawEditor();

				memoryManagement::MemoryManager::GetInstance()->Update();		// MemoryManager should be the last subsystem to update.
			}

		}

		ITime& Engine::GetGlobalTime()
		{
			ME_ASSERT(_globalTime != nullptr, "Global timer is null");
			return *_globalTime;
		}

		resourceManagement::fileSystem::FileSystem& Engine::GetFileSystem()
		{
			ME_ASSERT(_globalFileSystem != nullptr, "Global file system is null");
			return *_globalFileSystem;
		}
	}
}