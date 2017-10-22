#include "File.h"
#include "debugging/Debug.h"
#include "utility/MMath.h"

namespace morphEngine
{
	namespace resourceManagement
	{
		namespace fileSystem
		{
			bool File::Open(const MString& path, FileAccessMode accessMode)
			{
				const char* p = path;
				_path = path;
				_accessMode = accessMode;
				MInt32 mode = std::ios::binary;
				switch(accessMode)
				{
				case FileAccessMode::READ:
					mode |= std::ios::in;
					break;
				case FileAccessMode::READ_WRITE:
					mode |= std::ios::in;
					mode |= std::ios::out;
					break;
				case FileAccessMode::WRITE:
					mode |= std::ios::out;
					break;
				}

				_fileHandle.open(path, mode);
				if(_fileHandle.is_open())
				{
					AddReference();
					_fileHandle.seekg(0, _fileHandle.end);
					_fileSize = (MSize)_fileHandle.tellg();
					_fileHandle.seekg(0, _fileHandle.beg);
				}
				return _fileHandle.is_open();
			}

			bool File::Close(bool force)
			{
				_referenceCount = MMath::Max(_referenceCount - 1, 0);
				if(_referenceCount == 0 || force)
				{
					_fileHandle.close();
					return true;
				}
				return false;
			}

			void File::CloseWithoutDereferencing()
			{
				_fileHandle.close();
			}

			void File::Read(MFixedArray<MUint8>& dataArray, MSize from, MSize byteCount)
			{
				byteCount = MMath::Max(0, MMath::Min(byteCount, _fileSize - from));
				if(byteCount > 0)
				{
					dataArray.Allocate(byteCount);
					Read(dataArray.GetDataPointer(), from, byteCount);
				}
			}

			void File::Read(MUint8 * dataArray, MSize from, MSize byteCount)
			{
				if (!IsOpened())
				{
					debugging::Debug::Log(debugging::ELogType::EXCEPTION, "Trying to read data from not opened file");
					return;
				}

				if (from >= _fileSize)
				{
					debugging::Debug::Log(debugging::ELogType::WARNING, "Trying to read data beyond its scope");
					reinterpret_cast<char*>(dataArray)[0] = '\0';
					return;
				}

				//file.seekg(start);
				//std::string s;
				//s.resize(end - start);
				//file.read(&s[0], end - start);
				
				_fileHandle.seekg(from);
				_fileHandle.read(reinterpret_cast<char*>(dataArray), byteCount);
			}

			void File::Write(const MFixedArray<MUint8>& dataToWrite, MSize start)
			{
				Write(dataToWrite.GetDataPointer(), dataToWrite.GetSize(), start);
			}

			void File::Write(const MUint8 * dataBuffer, MSize dataSize, MSize start)
			{
				if (!IsOpened())
				{
					debugging::Debug::Log(debugging::ELogType::EXCEPTION, "Trying to write data to not opened file");
					return;
				}

				if (_accessMode == FileAccessMode::READ)
				{
					debugging::Debug::Log(debugging::ELogType::EXCEPTION, "File opened in READ mode but trying to write data");
					return;
				}

				_fileHandle.seekp(start, _fileHandle.beg);
				_fileHandle.write(reinterpret_cast<const char*>(dataBuffer), dataSize);
				_fileHandle.flush();
				_fileHandle.seekg(0, _fileHandle.end);
				_fileSize = _fileHandle.tellg();
			}

			void File::Write(const MString& dataToWrite, MSize start)
			{
				Write(reinterpret_cast<const MUint8*>(static_cast<const char*>(dataToWrite)), dataToWrite.Length(), start);
			}

			void File::WriteAppend(const MUint8 * dataBuffer, MSize dataSize)
			{
				if (!IsOpened())
				{
					debugging::Debug::Log(debugging::ELogType::EXCEPTION, "Trying to write data to not opened file");
					return;
				}

				if (_accessMode == FileAccessMode::READ)
				{
					debugging::Debug::Log(debugging::ELogType::EXCEPTION, "File opened in READ mode but trying to write data");
					return;
				}
				_fileHandle.seekp(0, _fileHandle.end);
				_fileHandle.write(reinterpret_cast<const char*>(dataBuffer), dataSize);
				_fileHandle.flush();
				_fileHandle.seekg(0, _fileHandle.end);
				_fileSize = _fileHandle.tellg();
			}

			void File::WriteAppend(const MFixedArray<MUint8>& dataToWrite)
			{
				WriteAppend(dataToWrite.GetDataPointer(), dataToWrite.GetSize());
			}

			void File::WriteAppend(const MString& dataToWrite)
			{
				WriteAppend(reinterpret_cast<const MUint8*>(static_cast<const char*>(dataToWrite)), dataToWrite.Length());
			}
		}
	}
}