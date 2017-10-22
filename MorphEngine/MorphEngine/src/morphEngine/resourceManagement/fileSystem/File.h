#pragma once

#include "core/GlobalDefines.h"
#include "utility/MString.h"
#include "utility/MFixedArray.h"
#include "debugging/Debug.h"
#include <fstream>

namespace morphEngine
{
	namespace resourceManagement
	{
		namespace fileSystem
		{
			using namespace utility;

			//Specifies how file will be accessed
			enum class FileAccessMode
			{
				//Only read access
				READ,
				//Full access
				READ_WRITE,
				//Only write access
				WRITE
			};

#ifdef PLATFORM_WINDOWS
#define FRIEND_FS friend class WindowsFS
#else
#define FRIEND_FS
#endif

			class File
			{
			friend class FileSystem;
			FRIEND_FS;

			protected:
				MString _path;
				std::fstream _fileHandle;
				FileAccessMode _accessMode;
				MSize _fileSize;
				MInt32 _referenceCount;

				bool Open(const MString& path, FileAccessMode accessMode);
				bool Close(bool force = false);
				void CloseWithoutDereferencing();

				inline void AddReference()
				{
					++_referenceCount;
				}

			public:
				inline File() : _referenceCount(0) { }
				inline File(const File& other) : _path(other._path), _accessMode(other._accessMode), _referenceCount(0)
				{
					if(other.IsOpened())
					{
						Open(_path, _accessMode);
					}
				}

				void Read(MFixedArray<MUint8>& dataArray, MSize from = 0, MSize byteCount = UINT64_MAX);
				void Read(MUint8* dataArray, MSize from = 0, MSize byteCount = UINT64_MAX);
				void Write(const MFixedArray<MUint8>& dataToWrite, MSize start = 0);
				void Write(const MUint8* dataBuffer, MSize dataSize, MSize start = 0);
				void Write(const MString& dataToWrite, MSize start = 0);
				void WriteAppend(const MUint8* dataBuffer, MSize dataSize);
				void WriteAppend(const MFixedArray<MUint8>& dataToWrite);
				void WriteAppend(const MString& dataToWrite);

				inline bool IsOpened() const
				{
					return _fileHandle.is_open();
				}

				inline const MString& GetPath() const
				{
					return _path;
				}

				inline MSize GetSize() const
				{
					return _fileSize;
				}

				inline bool operator==(const File& other) const
				{
					return _path == other._path;
				}

				inline bool operator!=(const File& other) const
				{
					return _path != other._path;
				}
			};
		}
	}
}