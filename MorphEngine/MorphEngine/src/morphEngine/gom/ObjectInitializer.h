#pragma once

#include "core/GlobalDefines.h"
#include "../utility/MString.h"
#include "utility\MDictionary.h"

#define NULL_ID 0

namespace morphEngine
{
	namespace memoryManagement
	{
		template <class T> class HandleInternal;
		template <class T> class Handle;
	}

	namespace core
	{
		class MObject;
	}

	namespace gom
	{
		class ObjectInitializer
		{
			friend class Scene;
			friend class SceneManager;
			friend class BaseObject;
			friend class core::MObject;

		protected:
			static utility::MArray<MUint64> _ids;

			bool _bIsFakeInitializer;

		protected:
			static void AddID(MUint64 id);
			static void RemoveID(MUint64 id);

			//This constructor will be used by Scene class to tell ObjectInitializer that the scene is loaded so no new ids should be generated since they're loaded from file
			inline ObjectInitializer(bool bFakeInitializer)
			{
				_bIsFakeInitializer = true;
			}

		public:
			memoryManagement::HandleInternal<core::MObject>* _this = nullptr;
			utility::MString _name;
			bool _bEnabled;
			bool _bVisible;

			ObjectInitializer(const utility::MString& name = "NoName", bool enabled = true, bool visible = true) :
				_bIsFakeInitializer(false),
				_name(name),
				_bEnabled(enabled),
				_bVisible(visible)
			{

			}

			inline static void ShutdownIDsArray() { _ids.Shutdown(); }
			inline bool IsFakeInitializer() const { return _bIsFakeInitializer; }

			static MUint64 GenerateUID();
		};
	}
}