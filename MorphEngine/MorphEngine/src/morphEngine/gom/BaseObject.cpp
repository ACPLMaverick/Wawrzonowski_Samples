#include "BaseObject.h"
#include "reflection/Type.h"

namespace morphEngine
{
	namespace gom
	{
		
		BaseObject::BaseObject(const ObjectInitializer& initializer) :
			MObject(initializer),
			_name(initializer._name), _bEnabled(initializer._bEnabled), _bVisible(initializer._bVisible),
			_bInitialized(false)
		{
			_properties.Clear();
			RegisterProperties();
		}

		void BaseObject::RegisterProperties()
		{
			RegisterProperty("ID", &_uniqueID, morphEngine::reflection::PropertyAttributes::Serializable | morphEngine::reflection::PropertyAttributes::ReadOnly);
			RegisterProperty("Name", &_name, morphEngine::reflection::PropertyAttributes::Serializable | morphEngine::reflection::PropertyAttributes::HiddenInDetails);
			RegisterProperty("Enabled", &_bEnabled);
			RegisterProperty("Visible", &_bVisible);
		}

		BaseObject::BaseObject(const BaseObject& c, bool bDeepCopy) :
			MObject(c, bDeepCopy),
			_name(c._name),
			_bEnabled(c._bEnabled),
			_bVisible(c._bVisible),
			_bInitialized(false)
		{
			RegisterProperties();
		}


		BaseObject::~BaseObject()
		{
			_bEnabled = false;
			_bVisible = false;

			MSize size = _properties.GetSize();
			for(MSize i = 0; i < size; ++i)
			{
				reflection::PropertyBase* pb = _properties[i];
				if(pb != nullptr)
				{
					delete pb;
					_properties[i] = nullptr;
				}
			}
			_properties.Clear();
		}

		void BaseObject::Initialize()
		{
			_bInitialized = true;
		}

		void BaseObject::Shutdown()
		{
			if(_uniqueID != NULL_ID) ObjectInitializer::RemoveID(_uniqueID);
			_bInitialized = false;
		}

		void BaseObject::Serialize(MFixedArray<MUint8>& outData)
		{
			MArray<MFixedArray<MUint8>> serializedDatas;

			MSize wholeDataSize = 0;
			MArray<reflection::PropertyBase*>::MIteratorArray it = _properties.GetBegin();
			for(; it.IsValid(); ++it)
			{
				MFixedArray<MUint8> propertyData;
				(*it)->Serialize(propertyData);
				serializedDatas.Add(propertyData);
				wholeDataSize += propertyData.GetSize();
			}

			outData.Allocate(wholeDataSize);
			MArray<MFixedArray<MUint8>>::MIteratorArray serializedIt = serializedDatas.GetBegin();
			MSize offset = 0;
			MSize dataSize = 0;
			for(; serializedIt.IsValid(); ++serializedIt)
			{
				dataSize = (*serializedIt).GetSize();
				memcpy(outData.GetDataPointer() + offset, (*serializedIt).GetDataPointer(), dataSize);
				offset += dataSize;
			}
		}

		void BaseObject::Deserialize(MFixedArray<MUint8>& serializedData, MSize& index)
		{
			MArray<reflection::PropertyBase*>::MIteratorArray it = _properties.GetBegin(); 
			for(; it.IsValid(); ++it)
			{
				(*it)->Deserialize(serializedData, index);
			}
		}

		void BaseObject::PostDeserialize()
		{
			MArray<reflection::PropertyBase*>::MIteratorArray it = _properties.GetBegin();
			for(; it.IsValid(); ++it)
			{
				(*it)->PostDeserialize();
			}

			ObjectInitializer::AddID(_uniqueID);
		}

		bool BaseObject::operator==(const BaseObject& b) const
		{
			return this->_uniqueID == b._uniqueID;
		}

		BaseObject& BaseObject::operator=(const BaseObject & c)
		{
			MObject::operator=(c);

			_properties = c._properties;
			_name = c._name;
			_bEnabled = c._bEnabled;
			_bVisible = c._bVisible;

			return *this;
		}
	}
}