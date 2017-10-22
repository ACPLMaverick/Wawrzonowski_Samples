#include "WindowsFS.h"

#ifdef PLATFORM_WINDOWS

#define CHECK_FO \
if(fo == 0) \
{ \
	CreateFileOperation(); \
	if(fo == 0) \
	{ \
		return; \
	} \
}
#define CHECK_FO_BOOL \
if(fo == 0) \
{ \
	CreateFileOperation(); \
	if(fo == 0) \
	{ \
		return false; \
	} \
}
#define CREATE_WSTRING_FROM_MSTRING(name, mstring) \
wchar_t* name = new wchar_t[128]; \
mbtowc(name, mstring, 128);
#define DELETE_WSTRING(name) delete[] name;

#include <cstdio>
#include <comdef.h>

namespace morphEngine
{
	namespace resourceManagement
	{
		namespace fileSystem
		{
			MString WindowsFS::_parentPath = "Resources\\";

			File& WindowsFS::Internal_OpenFile(const MString& filePath, FileAccessMode accessMode)
			{
				MString path = MString::Replace(filePath, '/', '\\');
				MInt32 openedFileIndex;
				if(IsFileOpened(path, openedFileIndex))
				{
					File& f = _openedFiles[openedFileIndex];
					f.AddReference();
					return _openedFiles[openedFileIndex];
				}
				File f;
				if(f.Open(path, accessMode))
				{
					_openedFiles.Add(f);
				}
				else
				{
					if(Internal_CreateFile(path))
					{
						if(f.Open(path, accessMode))
						{
							_openedFiles.Add(f);
						}
						else
						{
							f.Close();
						}
					}
					else
					{
						f.Close();
					}
				}
				return _openedFiles[_openedFiles.GetSize() - 1];
			}
			
			bool WindowsFS::Internal_CreateFile(const MString& filePath)
			{
				MString filePathR = MString::Replace(filePath, '/', '\\');

				MInt32 lastSlash = filePathR.FindLast('\\');
				if(lastSlash > 0)
				{
					MString folder = filePathR.Substring(0, lastSlash);
					if(!Internal_DoesDirectoryExist(folder))
					{
						MArray<MString> folders;
						MArray<char> sep;
						sep.Add('\\');
						MString::Split(folder, sep, folders);
						MSize size = folders.GetSize();
						for(MSize i = 0; i < size; ++i)
						{
							MString path = folders[i];
							for(MInt32 j = (MInt32)i - 1; j >= 0; --j)
							{
								path = folders[j] + '\\' + path;
							}
							Internal_CreateDirectory(path);
						}
					}
					HANDLE h = CreateFileA(filePathR, GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
					if(h == INVALID_HANDLE_VALUE)
					{
						DWORD err = GetLastError();
						if(err != ERROR_FILE_EXISTS) //If err = ERROR_FILE_EXISTS then we should still return true (cause file exists and our poMInt32 was to create file)
						{
							debugging::Debug::Log(debugging::ELogType::ERR, "Failed to create file: " + filePathR + ", error code: " + MString::FromInt((MUint64)err));
							return false;
						}
					}
					else
					{
						CloseHandle(h);
					}
				}
				return true;
			}
			
			bool WindowsFS::Internal_CloseFile(File& file, bool force)
			{
				if(!file.IsOpened())
				{
					return false;
				}
				if(file.Close(force))
				{
					_openedFiles.Remove(file);
				}
				return true;
			}
			
			bool WindowsFS::Internal_RemoveFile(const MString& filePath, bool showWarning)
			{
				if(showWarning)
				{
					MInt32 result = MessageBoxA(NULL, "Do you really want to remove file " + filePath + " ?", "Warning", MB_YESNO | MB_ICONWARNING);
					if(result == IDNO)
					{
						//User canceled operation
						return false;
					}
				}
				MInt32 openedFileIndex;
				if(IsFileOpened(filePath, openedFileIndex))
				{
					debugging::Debug::Log(debugging::ELogType::WARNING, "Removing opened file (" + filePath + "). Make sure that you're not using this file");
					Internal_CloseFile(_openedFiles[openedFileIndex], true);
				}
				
				if(DeleteFile(filePath) == 0)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Removing file (" + filePath + ") failed, error code: " + MString::FromInt((MUint64)GetLastError()));
					return false;
				}
				return true;
			}

			bool WindowsFS::Internal_CopyFile(const MString& filePath, const MString& newPath, bool replaceIfExisting, bool warnIfExists)
			{
				if(!Internal_DoesDirectoryExist(newPath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Destination folder (" + newPath + ") does not exist");
					return false;
				}

				MString path = MString::Replace(filePath, '/', '\\');
				MString newPathR = MString::Replace(newPath, '/', '\\');

				File* openedFile = nullptr;
				int openedFileIndex;
				if(IsFileOpened(path, openedFileIndex))
				{
					openedFile = &_openedFiles[openedFileIndex];
					openedFile->CloseWithoutDereferencing();
				}

				MInt32 lastSlashIndex = path.FindLast('\\');
				if(lastSlashIndex == -1)
				{
					lastSlashIndex = (MInt32)path.Length();
				}
				MString newFilePath = newPathR + (newPathR.EndsWith("\\") ? "" : "\\") + path.Substring(lastSlashIndex + 1);
				bool fileExists = Internal_DoesFileExist(newFilePath);

				if(replaceIfExisting)
				{
					if(warnIfExists && fileExists)
					{
						MInt32 result = MessageBoxA(NULL, "File " + path + " exists. Do you really want to replace it?", "Warning", MB_YESNO | MB_ICONWARNING);
						if(result == IDNO)
						{
							//User canceled operation
							return false;
						}
					}

					if(CopyFileA(path, newFilePath, false) == 0)
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Copying file (" + path + ") to new location (" + newFilePath + ") failed, error code: " + MString::FromInt((MUint64)GetLastError()));
						return false;
					}

					if(openedFile != nullptr)
					{
						openedFile->Open(path, openedFile->_accessMode);	//Reopen file
						openedFile->_referenceCount -= 1;	//We do not want to hold reference for this particular open
					}

					return true;
				}
				
				if(fileExists)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "File at " + newFilePath + " exists");
					return false;
				}

				if(CopyFileA(path, newFilePath, true) == 0)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Copying file (" + path + ") to new location (" + newFilePath + ") failed, error code: " + MString::FromInt((MUint64)GetLastError()));
					return false;
				}

				if(openedFile != nullptr)
				{
					openedFile->Open(path, openedFile->_accessMode);	//Reopen file
					openedFile->_referenceCount -= 1;	//We do not want to hold reference for this particular open
				}

				return true;
			}

			bool WindowsFS::Internal_MoveFile(const MString& filePath, const MString& newPath, bool replaceIfExisting, bool warnIfExists)
			{
				if(!Internal_DoesDirectoryExist(newPath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Destination folder (" + newPath + ") does not exist");
					return false;
				}

				MString path = MString::Replace(filePath, '/', '\\');
				MString newPathR = MString::Replace(newPath, '/', '\\');
				const char* p = path;
				p = newPathR;

				MInt32 lastSlashIndex = path.FindLast('\\');
				if(lastSlashIndex == -1)
				{
					lastSlashIndex = (MInt32)path.Length();
				}
				MString newFilePath = newPathR + (newPathR.EndsWith("\\") ? "" : "\\") + path.Substring(lastSlashIndex + 1);
				bool fileExists = Internal_DoesFileExist(newFilePath);

				File* openedFile = nullptr;
				int openedFileIndex;
				if(IsFileOpened(path, openedFileIndex))
				{
					openedFile = &_openedFiles[openedFileIndex];
					openedFile->CloseWithoutDereferencing();
				}

				if(replaceIfExisting)
				{
					if(warnIfExists && fileExists)
					{
						MInt32 result = MessageBoxA(NULL, "File " + path + " exists. Do you really want to replace it?", "Warning", MB_YESNO | MB_ICONWARNING);
						if(result == IDNO)
						{
							//User canceled operation
							return false;
						}
					}

					if(MoveFileExA(path, newFilePath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == 0)
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Moving file (" + path + ") to new location (" + newFilePath + ") failed, error code: " + MString::FromInt((MUint64)GetLastError()));
						return false;
					}

					if(openedFile != nullptr)
					{
						openedFile->Open(newFilePath, openedFile->_accessMode);	//Reopen file
						openedFile->_referenceCount -= 1;	//We do not want to hold reference for this particular open
					}

					return true;
				}

				if(fileExists)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "File at " + newFilePath + " exists");
					return false;
				}

				if(MoveFileExA(path, newFilePath, MOVEFILE_WRITE_THROUGH) == 0)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Moving file (" + path + ") to new location (" + newFilePath + ") failed, error code: " + MString::FromInt((MUint64)GetLastError()));
					return false;
				}

				if(openedFile != nullptr)
				{
					openedFile->Open(newFilePath, openedFile->_accessMode);	//Reopen file
					openedFile->_referenceCount -= 1;	//We do not want to hold reference for this particular open
				}

				return true;
			}

			bool WindowsFS::Internal_RenameFile(const MString& filePath, const MString& newFileName)
			{
				if(!Internal_DoesFileExist(filePath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Source file (" + filePath + ") does not exist");
					return false;
				}

				MString path = MString::Replace(filePath, '/', '\\');

				MInt32 lastSlashIndex = path.FindLast('\\');
				if(lastSlashIndex == -1)
				{
					lastSlashIndex = (MInt32)path.Length();
				}

				MString& newFilePath = path.Substring(0, lastSlashIndex);
				newFilePath += '\\';
				newFilePath += newFileName;

				if(Internal_DoesFileExist(newFilePath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "File with name (" + newFileName + ") already exists");
					return false;
				}

				File* openedFile = nullptr;
				int openedFileIndex;
				if(IsFileOpened(path, openedFileIndex))
				{
					openedFile = &_openedFiles[openedFileIndex];
					openedFile->CloseWithoutDereferencing();
				}

				if(MoveFileExA(filePath, newFilePath, MOVEFILE_WRITE_THROUGH) == 0)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Moving file (" + filePath + ") to new location (" + newFilePath + ") failed, error code: " + MString::FromInt((MUint64)GetLastError()));
					return false;
				}
				
				if(openedFile != nullptr)
				{
					openedFile->Open(newFilePath, openedFile->_accessMode);	//Reopen file
					openedFile->_referenceCount -= 1;	//We do not want to hold reference for this particular open
				}

				return true;
			}

			bool WindowsFS::Internal_DoesFileExist(const MString& filePath) const
			{
				WIN32_FIND_DATA findFileData;
				HANDLE handle = FindFirstFile(filePath, &findFileData);
				bool found = handle != INVALID_HANDLE_VALUE;
				if(found)
				{
					FindClose(handle);
				}
				return found;
			}
			
			bool WindowsFS::Internal_CreateDirectory(const MString& directoryPath) const
			{
				if(Internal_DoesDirectoryExist(directoryPath))
				{
					return true;
				}

				return CreateDirectoryA(directoryPath, NULL) == 1;
			}
			
			bool WindowsFS::Internal_RemoveDirectory(const MString& directoryPath, bool showWarning)
			{
				const char* tmp = directoryPath;
				if(!Internal_DoesDirectoryExist(directoryPath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Directory: " + directoryPath + " does not exist!");
					return false;
				}

				if(showWarning)
				{
					MInt32 result = MessageBoxA(NULL, "Do you really want to remove directory " + directoryPath + " with underlying content?", "Warning", MB_YESNO | MB_ICONWARNING);
					if(result == IDNO)
					{
						//User canceled operation
						return false;
					} 
				}
				MArray<MString> files;
				MArray<MString> subdirs;
				Internal_GetAllFiles(directoryPath, files, true);
				Internal_GetAllSubdirectories(directoryPath, subdirs, true);
				MSize size = files.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					tmp = files[i];
					Internal_RemoveFile(files[i], false);
				}
				size = subdirs.GetSize();
				for(MInt32 i = (MInt32)size - 1; i >= 0; --i)
				{
					tmp = subdirs[i];
					if(RemoveDirectoryA(subdirs[i]) == 0)
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Failed to delete folder: " + subdirs[i] + ", error code: " + MString::FromInt((MUint64)GetLastError()));
					}
				}

				if(RemoveDirectoryA(directoryPath) == 0)
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Failed to delete folder: " + directoryPath + ", error code: " + MString::FromInt((MUint64)GetLastError()));
					return false;
				}
				return true;
			}

			bool WindowsFS::Internal_CopyDirectory(const MString& directoryPath, const MString& newPath, bool replaceIfExisting, bool warnIfExists)
			{
				if(!Internal_DoesDirectoryExist(newPath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Destination folder (" + newPath + ") does not exist");
					return false;
				}

				MString directoryName = directoryPath;
				if(directoryPath[directoryPath.Length()] == '\\')
				{
					directoryName = directoryPath.Substring(0, directoryName.Length() - 1);
				}
				MInt32 lastSlashIndex = directoryName.FindLast('\\');
				directoryName = directoryName.Substring(lastSlashIndex + 1);
				MString newDirectoryPath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + directoryName;

				if(!Internal_DoesDirectoryExist(newDirectoryPath))
				{
					Internal_CreateDirectory(newDirectoryPath);
				}
				else
				{
					if(replaceIfExisting)
					{
						if(warnIfExists)
						{
							MInt32 result = MessageBoxA(NULL, "Directory " + directoryName + " exists at (" + newDirectoryPath + "). Do you really want to replace it (with all of its content)?", "Warning", MB_YESNO | MB_ICONWARNING);
							if(result == IDNO)
							{
								//User canceled operation
								return false;
							}
						}
					}
					else
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Folder with that name (" + directoryName + ") already exists exists at path (" + newDirectoryPath + ")");
						return false;
					}
				}

				MArray<MString> subdirs;
				Internal_GetAllSubdirectories(directoryPath, subdirs, true);
				MSize size = subdirs.GetSize();
				MSize length = directoryPath.Length();
				for(MSize i = 0; i < size; ++i)
				{
					MString relativePath = subdirs[i].Substring(length);
					if(relativePath[0] == '\\')
					{
						relativePath = relativePath.Substring(1);
					}
					MString absolutePath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + directoryName + "\\" + relativePath;
					if(!Internal_DoesDirectoryExist(absolutePath))
					{
						Internal_CreateDirectory(absolutePath);
					}
				}

				MArray<MString> files;
				Internal_GetAllFiles(directoryPath, files, true);
				size = files.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					MString relativePath = files[i].Substring(length);
					if(relativePath[0] == '\\')
					{
						relativePath = relativePath.Substring(1);
					}
					relativePath.Replace('/', '\\');
					lastSlashIndex = relativePath.FindLast('\\');
					if(lastSlashIndex >= 0)
					{
						relativePath = relativePath.Substring(0, lastSlashIndex);
					}
					else
					{
						relativePath = "";
					}
					MString absolutePath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + directoryName + "\\" + relativePath;
					Internal_CopyFile(files[i], absolutePath, replaceIfExisting, false);
				}

				return true;
			}

			bool WindowsFS::Internal_MoveDirectory(const MString& directoryPath, const MString& newPath, bool replaceIfExisting, bool warnIfExists)
			{
				if(!Internal_DoesDirectoryExist(newPath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Destination folder (" + newPath + ") does not exist");
					return false;
				}

				MString directoryName = directoryPath;
				if(directoryPath[directoryPath.Length()] == '\\')
				{
					directoryName = directoryPath.Substring(0, directoryName.Length() - 1);
				}
				MInt32 lastSlashIndex = directoryName.FindLast('\\');
				directoryName = directoryName.Substring(lastSlashIndex + 1);
				MString newDirectoryPath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + directoryName;

				if(!Internal_DoesDirectoryExist(newDirectoryPath))
				{
					Internal_CreateDirectory(newDirectoryPath);
				}
				else
				{
					if(replaceIfExisting)
					{
						if(warnIfExists)
						{
							MInt32 result = MessageBoxA(NULL, "Directory " + directoryName + " exists at (" + newDirectoryPath + "). Do you really want to replace it (with all of its content)?", "Warning", MB_YESNO | MB_ICONWARNING);
							if(result == IDNO)
							{
								//User canceled operation
								return false;
							}
						}
					}
					else
					{
						debugging::Debug::Log(debugging::ELogType::ERR, "Folder with that name (" + directoryName + ") already exists exists at path (" + newDirectoryPath + ")");
						return false;
					}
				}

				MArray<MString> subdirs;
				Internal_GetAllSubdirectories(directoryPath, subdirs, true);
				MSize size = subdirs.GetSize();
				MSize length = directoryPath.Length();
				for(MSize i = 0; i < size; ++i)
				{
					MString relativePath = subdirs[i].Substring(length);
					if(relativePath[0] == '\\')
					{
						relativePath = relativePath.Substring(1);
					}
					MString absolutePath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + directoryName + "\\" + relativePath;
					const char* tmp = relativePath;
					tmp = absolutePath;
					if(!Internal_DoesDirectoryExist(absolutePath))
					{
						Internal_CreateDirectory(absolutePath);
					}
				}

				MArray<MString> files;
				Internal_GetAllFiles(directoryPath, files, true);
				size = files.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					MString relativePath = files[i].Substring(length);
					if(relativePath[0] == '\\')
					{
						relativePath = relativePath.Substring(1);
					}
					relativePath.Replace('/', '\\');
					lastSlashIndex = relativePath.FindLast('\\');
					if(lastSlashIndex >= 0)
					{
						relativePath = relativePath.Substring(0, lastSlashIndex);
					}
					else
					{
						relativePath = "";
					}
					MString absolutePath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + directoryName + "\\" + relativePath;
					Internal_MoveFile(files[i], absolutePath, replaceIfExisting, false);
				}

				Internal_RemoveDirectory(directoryPath, false);

				return true;
			}

			bool WindowsFS::Internal_RenameDirectory(const MString& directoryPath, const MString& newDirectoryName)
			{
				MInt32 lastSlashIndex = directoryPath.FindLast('\\');
				if(lastSlashIndex == -1)
				{
					lastSlashIndex = (MInt32)directoryPath.Length();
				}
				MString newPath = directoryPath.Substring(0, lastSlashIndex);
				MString newDirectoryPath = newPath + (newPath[newPath.Length()] == '\\' ? "" : "\\") + newDirectoryName;
				if(Internal_DoesDirectoryExist(newDirectoryPath))
				{
					debugging::Debug::Log(debugging::ELogType::ERR, "Directory with name (" + newDirectoryName + ") already exists at path (" + newPath + ")");
					return false;
				}

				Internal_CreateDirectory(newDirectoryPath);
				MArray<MString> subdirs;
				Internal_GetAllSubdirectories(directoryPath, subdirs, false);
				MSize size = subdirs.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					Internal_MoveDirectory(subdirs[i], newDirectoryPath, false, false);
				}
				MArray<MString> files;
				Internal_GetAllFiles(directoryPath, files, false);
				size = files.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					Internal_MoveFile(files[i], newDirectoryPath, false, false);
				}
				return Internal_RemoveDirectory(directoryPath, false);
			}
			
			bool WindowsFS::Internal_DoesDirectoryExist(const MString& directoryPath) const
			{
				DWORD dwAttrib = GetFileAttributes(directoryPath);

				return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
					(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
			}

			void WindowsFS::Internal_GetAllFiles(const MString& parentPath, MArray<MString>& files, bool includeSubdirectories) const
			{
				if(!Internal_DoesDirectoryExist(parentPath))
				{
					return;
				}

				WIN32_FIND_DATA findData;
				HANDLE file;
				bool endsWithSlash = parentPath[parentPath.Length() - 1] == '\\';
				MString path = parentPath;
				if(!endsWithSlash)
				{
					path += '\\';
				}
				file = FindFirstFile(path + '*', &findData);

				if(file == INVALID_HANDLE_VALUE)
				{
					return;
				}

				MString tmp;
				do
				{
					const char* p = path;
					tmp = path + findData.cFileName;
					if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						if(includeSubdirectories)
						{
							bool isParentOrDot = findData.cFileName[0] == '.' &&
								(findData.cFileName[1] == '\0' ||
								(findData.cFileName[1] == '.' && findData.cFileName[2] == '\0')
								 );
							if(!isParentOrDot)
							{
								Internal_GetAllFiles(tmp, files, includeSubdirectories);
							}
						}
					}
					else
					{
						files.Add(tmp);
					}
				} while(FindNextFile(file, &findData) != 0);

				FindClose(file);
			}

			void WindowsFS::Internal_GetAllSubdirectories(const MString& parentPath, MArray<MString>& dirs, bool includeSubdirectories) const
			{
				if(!Internal_DoesDirectoryExist(parentPath))
				{
					return;
				}

				WIN32_FIND_DATA findData;
				HANDLE file;
				bool endsWithSlash = parentPath[parentPath.Length() - 1] == '\\';
				MString path = parentPath;
				if(!endsWithSlash)
				{
					path += '\\';
				}
				file = FindFirstFile(path + '*', &findData);

				if(file == INVALID_HANDLE_VALUE)
				{
					return;
				}

				MString tmp;
				do
				{
					const char* p = path;
					tmp = path + findData.cFileName;
					if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					{
						bool isParentOrDot = findData.cFileName[0] == '.' &&
							(findData.cFileName[1] == '\0' ||
							(findData.cFileName[1] == '.' && findData.cFileName[2] == '\0')
							 );
						if(!isParentOrDot)
						{
							dirs.Add(tmp);
							if(includeSubdirectories)
							{
								Internal_GetAllSubdirectories(tmp, dirs, includeSubdirectories);
							}
						}
					}
				} while(FindNextFile(file, &findData) != 0);

				FindClose(file);
			}
		}
	}
}
#endif