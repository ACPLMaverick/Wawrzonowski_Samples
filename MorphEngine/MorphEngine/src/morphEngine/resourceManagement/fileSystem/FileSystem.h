#pragma once

#include "core/GlobalDefines.h"
#include "File.h"
#undef CreateFile
#undef CreateDirectory
#undef RemoveDirectory
#undef CopyFile
#undef MoveFile
namespace morphEngine
{
	namespace resourceManagement
	{
		namespace fileSystem
		{
			class FileSystem
			{
			protected:
				static MString _parentPath;

				MArray<File> _openedFiles;
				
			protected:
				inline bool IsFileOpened(const MString& filePath, MInt32& fileIndex) const
				{
					fileIndex = -1;
					MSize size = _openedFiles.GetSize();
					for(MSize i = 0; i < size; ++i)
					{
						if(filePath == _openedFiles[i].GetPath())
						{
							fileIndex = (MInt32)i;
							return true;
						}
					}
					return false;
				}

			public:
				void Shutdown();

				//Opens file (and creates if file does not exist)
				inline virtual File& OpenFile(const MString& filePath, FileAccessMode accessMode) = 0;
				//Opens file (without adding parent directory)
				inline virtual File& OpenFileAbsolute(const MString& absoluteFilePath, FileAccessMode accessMode) = 0;
				//Creates file at given path (if no file at given path exists)
				inline virtual bool CreateFile(const MString& filePath) = 0;
				//Closes file handle
				inline virtual bool CloseFile(File& file) = 0;
				//Removes file at given path (and closes its handle if needed)
				inline virtual bool RemoveFile(const MString& filePath, bool showWarning = false) = 0;
				//Closes given file (if it's opened) then copies file to newPath and reopens file
				inline virtual bool CopyFile(const MString& filePath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false) = 0;
				//Closes given file then moves it to newPath (does not reopen file)
				inline virtual bool MoveFile(const MString& filePath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false) = 0;
				//Renames file in given path with given name (NAME, not path)
				inline virtual bool RenameFile(const MString& filePath, const MString& newFileName) = 0;
				//Returns true if file exists
				inline virtual bool DoesFileExist(const MString& filePath) const = 0;
				//Creates directory at given path (if no directory at given path exists)
				inline virtual bool CreateDirectory(const MString& directoryPath) const = 0;
				//Removes directory at given path (if directory at given path exists)
				inline virtual bool RemoveDirectory(const MString& directoryPath, bool showWarning = false) = 0;
				//Copies directory (with all files and subdirectories) to newPath (closes every file handle under that directory)
				inline virtual bool CopyDirectory(const MString& directoryPath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false) = 0;
				//Moves directory (with all files and subdirectories) to newPath (closes every file handle under that directory)
				inline virtual bool MoveDirectory(const MString& directoryPath, const MString& newPath, bool replaceExisting = false, bool warnIfExists = false) = 0;
				//Renames directory with given newDirectoryName
				inline virtual bool RenameDirectory(const MString& directoryPath, const MString& newDirectoryName) = 0;
				//Returns true if directory exists at given path (false otherwise)
				inline virtual bool DoesDirectoryExist(const MString& directoryPath) const = 0;
				//Fills given files array with all files (and files only) below given parentPath (includeSubdirectories value indicates whether search for files in subdirectories of parentPath)
				inline virtual void GetAllFiles(const MString& parentPath, MArray<MString>& files, bool includeSubdirectories = false) const = 0;
				//Fills given dirs array with all dirs (excluding looping . and ..) below given parentPath (includeSubdirectories value indicates whether search for directories in subdirectories of parentPath)
				inline virtual void GetAllSubdirectories(const MString& parentPath, MArray<MString>& dirs, bool includeSubdirectories = false) const = 0;

				static FileSystem* GetFileSystemInstance();
			};
		}

	}
}