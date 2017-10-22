#include "MObject.h"
#include "reflection/Type.h"

using namespace morphEngine::gom;

namespace morphEngine
{
	namespace core
	{
		MObject::MObject(const gom::ObjectInitializer& initializer) :
			_this(initializer._this, true)
		{
			if(!initializer.IsFakeInitializer())
			{
				_uniqueID = gom::ObjectInitializer::GenerateUID();
			}
		}

		MObject::MObject(const MObject & c, bool bDeepCopy) :
			_this(true)				// THIS is copied here, but it's correct, externally set (to c) by allocator before calling copy constructor
		{
			_this = c._this;
			if (bDeepCopy)
			{
				_uniqueID = gom::ObjectInitializer::GenerateUID();
			}
			else
			{
				_uniqueID = c._uniqueID;
			}
		}

		MObject::~MObject()
		{

		}

		MObject & MObject::operator=(const MObject& c)
		{
			//In assignment operator DO NOT GENERATE ID if it's correct
			if(!(_uniqueID > NULL_ID && _uniqueID < (MUint64) - 1))
			{
				_uniqueID = ObjectInitializer::GenerateUID();
			}
			return *this;
		}
	}
}
