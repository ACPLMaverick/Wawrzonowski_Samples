#pragma once
#include "../utility/MString.h"

namespace morphEngine
{
	namespace debugging
	{
		using namespace utility;

		class Console
		{
			friend class Debug;

		protected:

#pragma region Protected

			bool _opened = false;

#pragma endregion
			
#pragma region Protected functions

			virtual bool Open() = 0;
			virtual void Close() = 0;

#pragma endregion

		public:

#pragma region Public functions

			virtual void Write(const MString& msg) = 0;
			virtual void Write(const char* msg) = 0;
			virtual void WriteLine(const MString& msg) = 0;
			virtual void WriteLine(const char* msg) = 0;
			virtual void WriteFormat(const char* format, ...) = 0;

#pragma endregion

		};
	}
}