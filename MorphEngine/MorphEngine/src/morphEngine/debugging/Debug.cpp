#include "../core/GlobalDefines.h"
#include "Win32Console.h"
#include "Debug.h"

#include "core/Engine.h"

#if _DEBUG

#include <DbgHelp.h>
#include <comdef.h>

#endif // _DEBUG


namespace morphEngine
{
	namespace debugging
	{
		MString Debug::_logLogType = "LOG - ";
		MString Debug::_errorLogType = "ERROR - ";
		MString Debug::_exceptionLogType = "EXCEPTION - ";
		MString Debug::_warningLogType = "WARNING - ";
		MString Debug::_defaultLogType = "Not recognized - ";
#if _TRACE_CALL_STACK
		bool Debug::_bSymInitialized = false;
#endif

		MString Debug::_logFile = "";
		Console* Debug::_console = nullptr;
		
		void Debug::Init()
		{
#if _DEBUG
			if(_console == nullptr)
			{
#ifdef PLATFORM_WINDOWS
				_console = new Win32Console;
#endif
			}

			if (_console)
			{
				_console->Open();
			}

#endif
		}

		void Debug::Log(ELogType type, const MString &msg)
		{
#if _DEBUG
			if(!_console)
			{
				return;
			}
			MString output = LogTypeToString(type) + msg;
			_logFile += output;
			_logFile += "\n";
			_console->WriteLine(output);
#endif
		}

		void Debug::Log(ELogType type, const char msg)
		{
#if _DEBUG
			if(!_console)
			{
				return;
			}
			MString output = LogTypeToString(type) + MString(&msg);
			_logFile += output;
			_logFile += "\n";
			_console->WriteLine(output);
#endif
		}

		void Debug::Shutdown()
		{
			_logLogType.Shutdown();
			_errorLogType.Shutdown();
			_exceptionLogType.Shutdown();
			_warningLogType.Shutdown();
			_defaultLogType.Shutdown();

#if _DEBUG
			if(_console != nullptr)
			{
				_console->Close();
				delete _console;
				_console = nullptr;
			}

			resourceManagement::fileSystem::FileSystem& fs = core::Engine::GetInstance()->GetFileSystem();
			if(!fs.DoesDirectoryExist("Logs"))
			{
				fs.CreateDirectory("Logs");
			}
			if(fs.DoesFileExist("Logs/previousLog.txt"))
			{
				fs.RemoveFile("Logs/previousLog.txt");
			}
			if(fs.DoesFileExist("Logs/log.txt"))
			{
				fs.RenameFile("Logs/log.txt", "previousLog.txt");
				fs.CreateFile("Logs/log.txt");
			}
			else
			{
				fs.CreateFile("Logs/log.txt");
			}
			resourceManagement::fileSystem::File& logFile = fs.OpenFile("Logs/log.txt", resourceManagement::fileSystem::FileAccessMode::READ_WRITE);
			logFile.Write(_logFile);
#endif

			_logFile.Shutdown();
		}

		void Debug::GetCallStack(std::string& result)
		{
#if _TRACE_CALL_STACK

			if (!_bSymInitialized)
			{
				SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
				SymInitialize(GetCurrentProcess(), NULL, TRUE);
				_bSymInitialized = true;
			}

			result = "";
			unsigned int i;

			unsigned short frames;
			SYMBOL_INFO   *symbol;
			IMAGEHLP_LINE64 line;
			ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
			line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
			void* _symbolStack[SYMBOL_STACK_SIZE];
			frames = CaptureStackBackTrace(0, SYMBOL_STACK_SIZE, _symbolStack, NULL);
			symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
			symbol->MaxNameLen = 255;
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			for (i = 0; i < frames; i++)
			{
				if (SymFromAddr(GetCurrentProcess(), (DWORD64)(_symbolStack[i]), 0, symbol))
				{
					DWORD dummyDisp;
					SymGetLineFromAddr(GetCurrentProcess(), (DWORD64)(_symbolStack[i]), &dummyDisp, &line);
					result += std::string(std::to_string(frames - i - 1) + std::string(" ") + std::string(symbol->Name) + std::string(" 0x") +
						std::to_string(symbol->Address) + " at line: " + std::to_string(line.LineNumber - 1) + std::string("\n"));
				}
				else
				{
					HRESULT hr = GetLastError();
					result += (_com_error(hr)).ErrorMessage();
					result += " Address: " + std::to_string(reinterpret_cast<unsigned long long>(_symbolStack[i])) + " \n";
				}
			}
			free(symbol);

#endif // _TRACE_CALL_STACK
		}
	}
}