#pragma once

#include "../core/GlobalDefines.h"

#ifdef PLATFORM_WINDOWS
#include "Console.h"

namespace morphEngine
{
	namespace debugging
	{
		class Win32Console : public Console
		{
		protected:
			FILE* _errStream;
			FILE* _outStream;
			FILE* _inStream;

		protected:
			virtual bool Open() override;
			virtual void Close() override;

		public:
			Win32Console();
			~Win32Console();

			virtual void Write(const MString& msg) override;
			virtual void Write(const char* msg) override;
			virtual void WriteLine(const MString& msg) override;
			virtual void WriteLine(const char* msg) override;
			virtual void WriteFormat(const char* format, ...) override;
		};
	}
}

#endif // PLATFORM_WINDOWS