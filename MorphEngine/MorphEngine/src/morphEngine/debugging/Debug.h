#pragma once
#include "Console.h"

namespace morphEngine
{
	namespace debugging
	{
		using namespace utility;

		enum class ELogType
		{
			LOG = 0x0000,
			WARNING = 0x0001,
			EXCEPTION = 0x0010,
			ERR = 0x0100
		};

		/// <summary>
		/// Static class.
		/// </summary>
		class Debug
		{
		protected:
			static MString _logLogType;
			static MString _errorLogType;
			static MString _exceptionLogType;
			static MString _warningLogType;
			static MString _defaultLogType;
			static MString _logFile;

			static Console* _console;

#if _TRACE_CALL_STACK
			static const int SYMBOL_STACK_SIZE = 128;
			static bool _bSymInitialized;
#endif

		protected:
			static inline MString& LogTypeToString(ELogType type)
			{
				switch(type)
				{
				case ELogType::LOG:
					return _logLogType;
				case ELogType::ERR:
					return _errorLogType;
				case ELogType::EXCEPTION:
					return _exceptionLogType;
				case ELogType::WARNING:
					return _warningLogType;
				
				default:
					return _defaultLogType;
				}
			}

		public:
			static void Init();
			static void Log(ELogType type, const MString& msg);
			static void Log(ELogType type, const char msg);
			static void Shutdown();

			static void GetCallStack(std::string& outResult);
			static inline void SetConsole(Console* console, bool deleteOldConsole = true)
			{
				if(_console != nullptr)
				{
					_console->Close();
					if(deleteOldConsole)
					{
						delete _console;
						_console = nullptr;
					}
				}
				_console = console;
				if(_console != nullptr)
				{
					_console->Open();
				}
			}
		};

	}
}