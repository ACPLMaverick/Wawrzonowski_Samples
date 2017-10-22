#include "Win32Console.h"

#include <fcntl.h>
#include <stdio.h>
#include <io.h>

namespace morphEngine
{
	namespace debugging
	{

		Win32Console::Win32Console() : _errStream(0), _inStream(0), _outStream(0)
		{

		}

		Win32Console::~Win32Console()
		{
			Close();
		}


		bool Win32Console::Open()
		{
			_opened = AllocConsole() == 1;
			freopen_s(&_inStream, "CONIN$", "r", stdin);
			freopen_s(&_outStream, "CONOUT$", "w", stdout);
			freopen_s(&_errStream, "CONOUT$", "w", stderr);

			return true;
		}

		void Win32Console::Close()
		{
			if (_opened)
			{
				FreeConsole();
			}

#ifndef _DEBUG
			if(_errStream)
			{
				fclose(_errStream);
				_errStream = 0;
			}
			
			if(_inStream)
			{
				fclose(_inStream);
				_inStream = 0;
			}

			if(_outStream)
			{
				fclose(_outStream);
				_outStream = 0;
			}
#endif

			_opened = false;
		}

		void Win32Console::Write(const MString& msg)
		{
			if(!_opened)
			{
				Open();
			}
			printf("%s", (const char*)msg);
		}

		void Win32Console::Write(const char* msg)
		{
			if(!_opened)
			{
				Open();
			}
			printf("%s", msg);
		}

		void Win32Console::WriteLine(const MString& msg)
		{
			if(!_opened)
			{
				Open();
			}
			printf("%s\n", (const char*)msg);
		}

		void Win32Console::WriteLine(const char * msg)
		{
			if (_opened)
			{
				printf("%s\n", msg);
			}
		}

		void Win32Console::WriteFormat(const char* format, ...)
		{
			if(!_opened)
			{
				Open();
			}
			va_list argptr;
			va_start(argptr, format);
			vfprintf(stderr, format, argptr);
			va_end(argptr);
		}

	}
}