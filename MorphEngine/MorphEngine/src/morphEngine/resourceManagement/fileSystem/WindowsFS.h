#pragma once

#include "core/GlobalDefines.h"

#ifdef PLATFORM_WINDOWS
#include "FileSystem.h"
#include "debugging/Debug.h"

namespace morphEngine
{
	namespace resourceManagement
	{
		namespace fileSystem
		{
			class WindowsFS : public FileSystem
			{
			protected:
				File& Internal_OpenFile(const MString& filePath, FileAccessMode accessMode);
				bool Internal_CreateFile(const MString& filePath);
				bool Internal_CloseFile(File& file, bool force = false);
				bool Internal_RemoveFile(const MString& filePath, bool showWarning );
				bool Internal_CopyFile(const MString& filePath, const MString& newPath, bool replaceIfExisting, bool warnIfExists);
				bool Internal_MoveFile(const MString& filePath, const MString& newPath, bool replaceIfExisting, bool warnIfExists);
				bool Internal_RenameFile(const MString& filePath, const MString& newFileName);
				bool Internal_DoesFileExist(const MString& filePath) const;
				bool Internal_CreateDirectory(const MString& directoryPath) const;
				bool Internal_RemoveDirectory(const MString& directoryPath, bool showWarning);
				bool Internal_CopyDirectory(const MString& directoryPath, const MString& newPath, bool replaceIfExisting, bool warnIfExists);
				bool Internal_MoveDirectory(const MString& directoryPath, const MString& newPath, bool replaceIfExisting, bool warnIfExists);
				bool Internal_RenameDirectory(const MString& directoryPath, const MString& newDirectoryName);
				bool Internal_DoesDirectoryExist(const MString& directoryPath) const;
				void Internal_GetAllFiles(const MString& parentPath, MArray<MString>& files, bool includeSubdirectories) const;
				void Internal_GetAllSubdirectories(const MString& parentPath, MArray<MString>& dirs, bool includeSubdirectories) const;

			public:
				inline WindowsFS() : FileSystem()
				{
					if(!Internal_DoesDirectoryExist(_parentPath))
					{
						Internal_CreateDirectory(_parentPath);
					}
				}

				inline virtual File& OpenFile(const MString& filePath, FileAccessMode accessMode)
				{
					return Internal_OpenFile(_parentPath + filePath, accessMode);
				}

				inline virtual File& OpenFileAbsolute(const MString& absoluteFilePath, FileAccessMode accessMode)
				{
					return Internal_OpenFile(absoluteFilePath, accessMode);
				}

				inline virtual bool CreateFile(const MString& filePath)
				{
					return Internal_CreateFile(_parentPath + filePath);
				}

				inline virtual bool CloseFile(File& file)
				{
					return Internal_CloseFile(file);
				}

				inline virtual bool RemoveFile(const MString& filePath, bool showWarning = false)
				{
					return Internal_RemoveFile(_parentPath + filePath, showWarning);
				}
				
				inline virtual bool CopyFile(const MString& filePath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false)
				{
					return Internal_CopyFile(_parentPath + filePath, _parentPath + newPath, replaceExisting, warnIfExists);
				}
				
				inline virtual bool MoveFile(const MString& filePath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false)
				{
					return Internal_MoveFile(_parentPath + filePath, _parentPath + newPath, replaceExisting, warnIfExists);
				}

				inline bool RenameFile(const MString& filePath, const MString& newFileName)
				{
					return Internal_RenameFile(_parentPath + filePath, newFileName);
				}

				inline virtual bool DoesFileExist(const MString& filePath) const
				{
					return Internal_DoesFileExist(_parentPath + filePath);
				}

				inline virtual bool CreateDirectory(const MString& directoryPath) const
				{
					return Internal_CreateDirectory(_parentPath + directoryPath);
				}

				inline virtual bool RemoveDirectory(const MString& directoryPath, bool showWarning = false)
				{
					return Internal_RemoveDirectory(_parentPath + directoryPath, showWarning);
				}

				inline virtual bool CopyDirectory(const MString& directoryPath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false)
				{
					MString sourcePath = _parentPath + directoryPath;
					MString destinationPath = _parentPath + newPath;
					sourcePath.Replace('/', '\\');
					destinationPath.Replace('/', '\\');
					if(!Internal_DoesDirectoryExist(sourcePath))
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Source directory (" + sourcePath + ")does not exist");
						return false;
					}
					if(!Internal_DoesDirectoryExist(destinationPath))
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Destination directory (" + destinationPath + ")does not exist");
						return false;
					}
					return Internal_CopyDirectory(sourcePath, destinationPath, replaceExisting, warnIfExists);
				}
				
				inline virtual bool MoveDirectory(const MString& directoryPath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false)
				{
					MString sourcePath = _parentPath + directoryPath;
					MString destinationPath = _parentPath + newPath;
					sourcePath.Replace('/', '\\');
					destinationPath.Replace('/', '\\');
					if(!Internal_DoesDirectoryExist(sourcePath))
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Source directory (" + sourcePath + ")does not exist");
						return false;
					}
					if(!Internal_DoesDirectoryExist(destinationPath))
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Destination directory (" + destinationPath + ")does not exist");
						return false;
					}
					return Internal_MoveDirectory(sourcePath, destinationPath, replaceExisting, warnIfExists);
				}

				inline virtual bool RenameDirectory(const MString& directoryPath, const MString& newDirectoryName)
				{
					MString sourcePath = _parentPath + directoryPath;
					sourcePath.Replace('/', '\\');

					if(!Internal_DoesDirectoryExist(sourcePath))
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Source directory (" + sourcePath + ")does not exist");
						return false;
					}

					return Internal_RenameDirectory(sourcePath, newDirectoryName);
				}

				inline virtual bool DoesDirectoryExist(const MString& directoryPath) const
				{
					return Internal_DoesDirectoryExist(_parentPath + directoryPath);
				}

				inline virtual void GetAllFiles(const MString& parentPath, MArray<MString>& files, bool includeSubdirectories = false) const
				{
					Internal_GetAllFiles(_parentPath + parentPath, files, includeSubdirectories);
				}

				//Returns from first to last dirs (i.e. given parent "Assets" the "Assets/TMP" folder will be before "Assets/TMP/Sth")
				inline virtual void GetAllSubdirectories(const MString& parentPath, MArray<MString>& dirs, bool includeSubdirectories = false) const
				{
					Internal_GetAllSubdirectories(_parentPath + parentPath, dirs, includeSubdirectories);
				}
			};
		}
	}
}
#endif // PLATFORM_WINDOWS
