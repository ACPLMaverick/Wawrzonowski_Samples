#pragma once
#include "ObjectInitializer.h"
#include "memoryManagement\Handle.h"
#include "debugging\Debug.h"
#include "reflection\Type.h"

using namespace morphEngine::memoryManagement;
using namespace morphEngine::core;
using namespace morphEngine::utility;
using namespace morphEngine::reflection;

#define START_ID 1

namespace morphEngine
{
	namespace gom
	{
		MArray<MUint64> ObjectInitializer::_ids;

		void ObjectInitializer::AddID(MUint64 id)
		{
			ME_ASSERT(!_ids.Contains(id), "ID is present in IDs array");

			_ids.Add(id);
			OutputDebugStringA(*("Added " + MString::FromInt(id) + "\n"));
		}

		void ObjectInitializer::RemoveID(MUint64 id)
		{
			ME_ASSERT_S(_ids.Contains(id))
			_ids.Remove(id);
			OutputDebugStringA(*("Removed " + MString::FromInt(id) + "\n"));
		}

		MUint64 ObjectInitializer::GenerateUID()
		{
			MArray<MUint64>::MIteratorArray it = _ids.GetBegin();
			MUint64 id = START_ID;
			while(_ids.Contains(id))
			{
				++id;
			}
			_ids.Add(id);
			OutputDebugStringA(*("Generated " + MString::FromInt(id) + "\n"));
			return id;
		}
	}
}