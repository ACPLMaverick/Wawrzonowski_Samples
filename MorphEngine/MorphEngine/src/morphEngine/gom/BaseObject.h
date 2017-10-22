#pragma once

#include "core/MObject.h"

namespace morphEngine
{
	namespace gom
	{
		class BaseObject : public core::MObject
		{
			ME_TYPE

		protected:

#pragma region Protected variables

			utility::MArray<reflection::PropertyBase*> _properties;
			
			MString _name;
			bool _bEnabled;
			bool _bVisible;
			bool _bInitialized;

#pragma endregion

#pragma region Protected functions

			BaseObject(const ObjectInitializer& initializer);

			template<typename T>
			inline void RegisterProperty(const MString& name, T* value, reflection::PropertyAttributes attributes = reflection::PropertyAttributes::Serializable)
			{
				_properties.Add(new reflection::Property<T>(name, _this, value, attributes));
			}

			// Beware. This function's implementation defines which component of this object is a property and needs to be serialized.
			// This function must be manually called in BOTH default constructor and copy constructor of EACH deriving classes.
			// Because it is called from constructors, you MAY NOT call base functions from your implementation of this function.
			// More info on what will happen if you do not comply:
			// https://www.securecoding.cert.org/confluence/display/cplusplus/OOP50-CPP.+Do+not+invoke+virtual+functions+from+constructors+or+destructors
			virtual void RegisterProperties();

#pragma endregion

		public:
#pragma region FunctionsPublic

			BaseObject(const BaseObject& c, bool bDeepCopy = true);
			virtual ~BaseObject();

			// Called post-constructor and post-initialize-properties
			virtual void Initialize();
			// Called pre-deallocation
			virtual void Shutdown();

			virtual void Serialize(MFixedArray<MUint8>& outData);
			virtual void Deserialize(MFixedArray<MUint8>& serializedData, MSize& index);
			virtual void PostDeserialize();

			bool operator==(const BaseObject& b) const;
			virtual BaseObject& operator=(const BaseObject& a);

			inline void SetName(const MString& newName) { _name = newName; }
			inline void SetEnabled(bool newEnabled) { _bEnabled = newEnabled; }
			inline void SetVisible(bool newVisible) { _bVisible = newVisible; }

			inline const MString& GetName() { return _name; }
			virtual inline bool GetEnabled() { return _bEnabled; }
			virtual inline bool GetVisible() { return _bVisible; }
			inline bool GetInitialized() { return _bInitialized; }

			inline const utility::MArray<reflection::PropertyBase*>& GetProperties()
			{
				return _properties;
			}

			template<typename T>
			reflection::Property<T>* GetProperty(const MString& name)
			{
				MSize size = _properties.GetSize();
				for(MSize i = 0; i < size; ++i)
				{
					if(_properties[i] != nullptr)
					{
						reflection::Property<T>* p = dynamic_cast<reflection::Property<T>*>(_properties[i]);
						if(p != nullptr && p->GetName() == name)
						{
							return p;
						}
					}
				}

				return nullptr;
			}

#pragma endregion
		};
	}
}