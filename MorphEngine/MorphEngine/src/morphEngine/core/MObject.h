#pragma once

#include "core/GlobalDefines.h"
#include "reflection/Property.h"
#include "utility/MArray.h"
#include "gom/ObjectInitializer.h"

namespace morphEngine
{
	namespace core
	{
		class MObject
		{
			friend class memoryManagement::ObjectPoolAllocator;
			ME_TYPE

		protected:

			MUint64 _uniqueID = NULL_ID;
			memoryManagement::Handle<MObject> _this;

			MObject(const gom::ObjectInitializer& initializer);

		public:

			MObject(const MObject& c, bool bDeepCopy = true);
			virtual ~MObject();

			virtual MObject& operator=(const MObject& c);

			inline const memoryManagement::Handle<MObject> This() { return _this; }
			inline MUint64 GetUniqueID() { return _uniqueID; }
		};
	}
}