#pragma once
#include "memoryManagement/MemoryManager.h"

#include "../core/ITime.h"
#include "../resourceManagement/fileSystem/FileSystem.h"
#include "Singleton.h"
#include "renderer/Window.h"

namespace morphEngine
{
	namespace core
	{
		/// <summary>
		/// Base class for the MorphEngine engine, used to create everything.
		/// </summary>
		class Engine : public Singleton<Engine>
		{
			friend class Singleton<Engine>;
		private:

#pragma region private


#pragma endregion

		protected:

#pragma region Protected

			ITime* _globalTime;
			resourceManagement::fileSystem::FileSystem* _globalFileSystem;

			//Temporary window
			renderer::Window* _window;

			bool _bIsGomUpdate;
			bool _bShouldQuit;
			bool _bIsEditorMode;

#pragma endregion

		protected:
			Engine();
			~Engine();

		public:

#pragma region functionsPublic

			void Init();
			void Shutdown();
			void Run();

			void InitEditor();
			void ShutdownEditor();
			void RunEditor();

			inline void Quit()
			{
				_bShouldQuit = true;
			}

			inline bool GetIsGomUpdate() { return _bIsGomUpdate; }
			inline bool GetIsEditorMode() { return _bIsEditorMode; }

			ITime& GetGlobalTime();
			resourceManagement::fileSystem::FileSystem& GetFileSystem();

			// temporary get window
			renderer::Window* GetWindow() { return _window; }

#pragma endregion
		};
	}
}