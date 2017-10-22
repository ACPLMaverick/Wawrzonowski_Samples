#include "FileSystem.h"
#include "WindowsFS.h"

namespace morphEngine
{
	namespace resourceManagement
	{
		namespace fileSystem
		{
			void FileSystem::Shutdown()
			{
				MSize size = _openedFiles.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					ME_ASSERT(_openedFiles[i].Close(true), "Can't succesfully close file" + _openedFiles[i].GetPath() + "!");
				}
				_openedFiles.Clear();

				_parentPath.Shutdown();
			}

			FileSystem* FileSystem::GetFileSystemInstance()
			{
#ifdef PLATFORM_WINDOWS
				WindowsFS* fileSystem = new WindowsFS();
#endif
				return fileSystem;
			}
		}
	}
}