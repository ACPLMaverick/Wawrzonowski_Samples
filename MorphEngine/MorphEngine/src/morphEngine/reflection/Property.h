#pragma once

#include "..\core\GlobalDefines.h"
#include "..\reflection\Type.h"
#include "core\Delegate.h"
#include "debugging\Debug.h"
#include "utility\MVector.h"
#include "utility\MQuaternion.h"
#include "utility\MColor.h"
#include "utility\Collections.h"
#include "assetLibrary\MFont.h"
#include "utility\MEnum.h"

#define PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(type) \
template<> \
inline void Property<type>::Serialize(MFixedArray<MUint8>& outData) \
{ \
	if(_attributes.HasDefined(morphEngine::reflection::PropertyAttributes::Serializable)) \
	{ \
		PropertySerializer::SerializeMAsset(this->As<assetLibrary::MAsset*>(), outData); \
	} \
}

#define PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(type) \
template<> \
inline void Property<type>::Deserialize(MFixedArray<MUint8>& serializedData, MSize& index) \
{ \
	if(_attributes.HasDefined(morphEngine::reflection::PropertyAttributes::Serializable)) \
	{ \
		PropertySerializer::DeserializeMAsset(this->As<assetLibrary::MAsset*>(), serializedData, index); \
	} \
}

#define PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(type) \
template<> \
inline void Property<type>::PostDeserialize() \
{ \
	if(_attributes.HasDefined(morphEngine::reflection::PropertyAttributes::Serializable)) \
	{ \
		PropertySerializer::PostDeserializeMAsset(this->As<assetLibrary::MAsset*>()); \
	} \
}

namespace morphEngine
{
	namespace assetLibrary
	{
		class MAudio;
		class MMesh;
		class MMaterial;
		class MAnimationClip;
		class MShaderBytecode;
		class MSkeleton;
		class MTexture;
		class MTexture1D;
		class MTexture2D;
		class MTexture2DAnimated;
		class MTextureCube;
	}

	namespace reflection
	{
		DECLARE_MENUM(PropertyAttributes, None = 0x0000, Serializable = 0x0001, ReadOnly = 0x0010, HiddenInDetails = 0x01000);

		class PropertyBase
		{
		protected:
			static assetLibrary::MAsset* LoadMAssetFromResourceManager(assetLibrary::MAssetType assetType, const utility::MString& path);

		public:
			inline virtual ~PropertyBase() {}

			virtual void* GetVoidPtr() const = 0;

			virtual inline void Serialize(MFixedArray<MUint8>& outData) = 0;
			virtual inline void Deserialize(MFixedArray<MUint8>& serializedData, MSize& index) = 0;
			virtual inline void PostDeserialize() = 0;

			virtual inline bool IsHandleProperty() const = 0;
			virtual inline bool IsArrayProperty() const = 0;
			virtual inline bool IsFixedArrayProperty() const = 0;
			virtual inline bool IsAssetArrayProperty() const = 0;
			virtual inline bool IsAssetFixedArrayProperty() const = 0;

			virtual inline const MString& GetName() const = 0;
			virtual inline const PropertyAttributes& GetAttributes() const = 0;

			virtual inline const morphEngine::reflection::Type* GetTypeUnderlying() const = 0;
		};

		template<typename T>
		class Property : public PropertyBase
		{
			friend class PropertySerializer;

		protected:
			memoryManagement::Handle<gom::BaseObject> _owner;
			MSize _dataOffset;
			MString _name;
			PropertyAttributes _attributes;

		public:
			core::Event<void> OnPropertyValueChanged;

		protected:
			inline T* InternalGet() const
			{
				if(_dataOffset == 0 || !_owner.IsValid())
				{
					return nullptr;
				}
				MUint8* dataPtr = (MUint8*)_owner.GetPointer() + _dataOffset;
				return (T*)dataPtr;
			}

		public:
			inline Property(const MString& name, memoryManagement::Handle<gom::BaseObject> owner, T* value, PropertyAttributes& attr = PropertyAttributes::None) : _owner(true), _name(name), _attributes(attr), _dataOffset(0)
			{
				_owner = owner;
				if(owner.IsValid())
				{
					_dataOffset = (MUint8*)value - (MUint8*)owner.GetPointer();
				}
			}

			inline Property(const Property& other) : _name(other._name), _attributes(other._attributes), _dataOffset(other._dataOffset), _owner(true)
			{
				_owner = other._owner;
			}

			inline virtual ~Property()
			{
				OnPropertyValueChanged.Shutdown();
				_dataOffset = 0;
			}

			inline T const* Get() const
			{
				if(_dataOffset == 0 || !_owner.IsValid())
				{
					return nullptr;
				}
				MUint8* dataPtr = (MUint8*)_owner.GetPointer() + _dataOffset;
				return (T*)dataPtr;
			}

			virtual void* GetVoidPtr() const
			{
				return (void*)InternalGet();
			}

			inline void Set(const T& value)
			{
				if(!_owner.IsValid() || _dataOffset == 0)
				{
					return;
				}
				MUint8* dataPtr = (MUint8*)_owner.GetPointer() + _dataOffset;
				*(T*)dataPtr = value;
				OnPropertyValueChanged();
			}

			virtual inline const MString& GetName() const
			{
				return _name;
			}

			virtual inline const PropertyAttributes& GetAttributes() const
			{
				return _attributes;
			}

			virtual inline const morphEngine::reflection::Type* GetTypeUnderlying() const;

			virtual inline void Serialize(MFixedArray<MUint8>& outData);
			virtual inline void Deserialize(MFixedArray<MUint8>& serializedData, MSize& index);
			virtual inline void PostDeserialize();

			virtual inline bool IsHandleProperty() const;
			virtual inline bool IsArrayProperty() const;
			virtual inline bool IsFixedArrayProperty() const;
			virtual inline bool IsAssetArrayProperty() const;
			virtual inline bool IsAssetFixedArrayProperty() const;

			inline Property& operator=(const Property& other)
			{
				_name = other._name;
				_owner = other._owner;
				_dataOffset = other._dataOffset;
				_attributes = other._attributes;

				return *this;
			}

			inline bool operator==(const Property& other) const
			{
				return _name == other._name && _owner == other._owner && _dataOffset == other._dataOffset;
			}

			inline bool operator==(const Property* other) const
			{
				return other != nullptr && _name == other->_name && _owner == other->_owner && _dataOffset == other->_dataOffset;
			}

			inline bool operator!=(const Property& other) const
			{
				return _name != other._name || _dataOffset != other._dataOffset || _owner != other._owner;
			}

			inline bool operator!=(const Property* other) const
			{
				return other == nullptr || _name != other->_name || _dataOffset != other->_dataOffset || _owner != other->_dataOffset;
			}

			template<typename D>
			inline Property<D>* As() const
			{
				return (Property<D>*)this;
			}
		};

		class PropertySerializer
		{
		public:
#pragma region Serialize methods
			
			template<typename T>
			static void Serialize(Property<MQueue<T>>* prop, MFixedArray<MUint8>& outData)
			{
				static_assert(false, "MQueue cannot be serialized");
			}

			template<typename T>
			static void Serialize(Property<MPriorityQueue<T>>* prop, MFixedArray<MUint8>& outData)
			{
				static_assert(false, "MPriorityQueue cannot be serialized");
			}

			template<typename T>
			static void Serialize(Property<MStack<T>>* prop, MFixedArray<MUint8>& outData)
			{
				static_assert(false, "MStack cannot be serialized");
			}

			template<typename T>
			static void Serialize(Property<MLinkedList<T>>* prop, MFixedArray<MUint8>& outData)
			{
				static_assert(false, "MLinkedList cannot be serialized");
			}

			template<typename T>
			static void Serialize(Property<MBinaryTree<T>>* prop, MFixedArray<MUint8>& outData)
			{
				static_assert(false, "MBinaryTree cannot be serialized");
			}

			template<typename T>
			static void Serialize(Property<T>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MSize size = sizeof(T);
				T* data = prop->InternalGet();
				if(data != nullptr)
				{
					outData.Allocate(size);
					memcpy(outData.GetDataPointer(), data, size);
				}
			}

			static void SerializeMAsset(Property<assetLibrary::MAsset*>* prop, MFixedArray<MUint8>& outData);
			static void SerializeMAssetArray(Property<MArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& outData);
			static void SerializeMAssetFixedArray(Property<MFixedArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& outData);
			static void SerializeMEnum(MEnumBase* enumBase, MFixedArray<MUint8>& outData);

			template<typename T>
			static void Serialize(Property<MArray<T>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MArray<T>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize = dataArray->GetSize();
					MSize size = arraySize * sizeof(T);
					MSize offset = sizeof(MSize);
					outData.Allocate(size + offset);
					memcpy(outData.GetDataPointer(), &arraySize, offset);
					memcpy(outData.GetDataPointer() + offset, dataArray->GetDataPointer(), size);
				}
			}

			template<typename T>
			static void Serialize(Property<MFixedArray<T>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MFixedArray<T>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize = dataArray->GetSize();
					MSize size = arraySize * sizeof(T);
					MSize offset = sizeof(MSize);
					outData.Allocate(size + offset);
					memcpy(outData.GetDataPointer(), &arraySize, offset);
					memcpy(outData.GetDataPointer() + offset, dataArray->GetDataPointer(), size);
				}
			}

			template<typename T>
			static void Serialize(Property<memoryManagement::Handle<T>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				memoryManagement::Handle<T>* handle = prop->InternalGet();
				outData.Allocate(sizeof(MUint64));
				if(handle != nullptr && handle->IsValid())
				{
					MUint64 id = NULL_ID;
					if(handle->IsStoringID())
					{
						id = handle->GetStoredID();
					}
					else
					{
						T* obj = handle->GetPointer();
						id = obj != nullptr ? obj->GetUniqueID() : NULL_ID;
					}
					memcpy(outData.GetDataPointer(), &id, sizeof(MUint64));
				}
			}

			static void Serialize(Property<MString>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MString* data = prop->InternalGet();
				if(data != nullptr)
				{
					MSize size = data->Length();
					MSize offset = sizeof(MSize);
					const char* dataPtr = *data;
					outData.Allocate(offset + size);
					memcpy(outData.GetDataPointer(), &size, offset);
					memcpy(outData.GetDataPointer() + offset, dataPtr, size);
				}
			}

			template<typename T>
			static void Serialize(Property<MArray<memoryManagement::Handle<T>>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MArray<memoryManagement::Handle<T>>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize = dataArray->GetSize();
					MSize offset = sizeof(MSize);
					outData.Allocate(offset + arraySize * sizeof(MUint64));
					memcpy(outData.GetDataPointer(), &arraySize, offset);
					for(MSize i = 0; i < arraySize; ++i)
					{
						memoryManagement::Handle<T> handle((*dataArray)[i]);
						if(handle.IsValid())
						{
							MUint64 id = NULL_ID;
							if(handle.IsStoringID())
							{
								id = handle.GetStoredID();
							}
							else
							{
								T* obj = handle.GetPointer();
								id = obj != nullptr ? obj->GetUniqueID() : NULL_ID;
							}
							memcpy(outData.GetDataPointer() + offset, &id, sizeof(MUint64));
						}
						else
						{
							MUint64 nullID = NULL_ID;
							memcpy(outData.GetDataPointer() + offset, &nullID, sizeof(MUint64));
						}
						offset += sizeof(MUint64);
					}
				}
			}

			static void Serialize(Property<MArray<MString>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MArray<MString>* data = prop->InternalGet();
				if(data != nullptr)
				{
					MSize arraySize = data->GetSize();
					MSize dataSize = 0;
					const MSize msizeSize = sizeof(MSize);
					for(MSize i = 0; i < arraySize; ++i)
					{
						dataSize += msizeSize;	//to ensure that outData will have enough memory to store MStrings lengths
						dataSize += (*data)[i].Length();
					}
					outData.Allocate(msizeSize + dataSize);
					memcpy(outData.GetDataPointer(), &arraySize, msizeSize);
					MSize stringLength;
					MSize currentOffset = msizeSize;
					for(MSize i = 0; i < arraySize; ++i)
					{
						MString& dataI = (*data)[i];
						stringLength = dataI.Length();
						const char* text = dataI;
						memcpy(outData.GetDataPointer() + currentOffset, &stringLength, msizeSize);
						memcpy(outData.GetDataPointer() + currentOffset + msizeSize, text, stringLength);
						currentOffset += msizeSize + stringLength;
					}
				}
			}

			template<typename T>
			static void Serialize(Property<MFixedArray<memoryManagement::Handle<T>>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MFixedArray<memoryManagement::Handle<T>>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize = dataArray->GetSize();
					MSize offset = sizeof(MSize);
					outData.Allocate(offset + arraySize * sizeof(MUint64));
					memcpy(outData.GetDataPointer(), &arraySize, offset);
					for(MSize i = 0; i < arraySize; ++i)
					{
						Handle<T> handle = (*dataArray)[i];
						if(handle.IsValid())
						{
							MUint64 id = NULL_ID;
							if(handle.IsStoringID())
							{
								id = handle.GetStoredID();
							}
							else
							{
								T* obj = handle.GetPointer();
								id = obj != nullptr ? obj->GetUniqueID() : NULL_ID;
							}
							memcpy(outData.GetDataPointer() + offset, &id, sizeof(MUint64));
						}
						else
						{
							MUint64 nullID = NULL_ID;
							memcpy(outData.GetDataPointer() + offset, &nullID, sizeof(MUint64));
						}
						offset += sizeof(MUint64);
					}
				}
			}

			static void Serialize(Property<MFixedArray<MString>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MFixedArray<MString>* data = prop->InternalGet();
				if(data != nullptr)
				{
					MSize arraySize = data->GetSize();
					MSize dataSize = 0;
					const MSize msizeSize = sizeof(MSize);
					for(MSize i = 0; i < arraySize; ++i)
					{
						dataSize += msizeSize;	//to ensure that outData will have enough memory to store MStrings lengths
						dataSize += (*data)[i].Length();
					}
					outData.Allocate(msizeSize + dataSize);
					memcpy(outData.GetDataPointer(), &arraySize, msizeSize);
					MSize stringLength;
					MSize currentOffset = msizeSize;
					for(MSize i = 0; i < arraySize; ++i)
					{
						MString& dataI = (*data)[i];
						stringLength = dataI.Length();
						const char* text = dataI;
						memcpy(outData.GetDataPointer() + currentOffset, &stringLength, msizeSize);
						memcpy(outData.GetDataPointer() + currentOffset + msizeSize, text, stringLength);
						currentOffset += msizeSize + stringLength;
					}
				}
			}

			template<typename K, typename V>
			static void Serialize(Property<MDictionary<K, V>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MDictionary<K, V>* dictionary = prop->InternalGet();
				if(dictionary != nullptr)
				{
					const MSize kSize = sizeof(K);
					const MSize vSize = sizeof(V);
					MSize dictionarySize = dictionary->GetSize();
					outData.Allocate(sizeof(MSize) + dictionarySize * (kSize + vSize));
					MSize currentOffset = sizeof(MSize);
					memcpy(outData.GetDataPointer(), &dictionarySize, currentOffset);
					MDictionary<K, V>::MIteratorDictionary it = dictionary->GetBegin();
					for(; it.IsValid(); ++it)
					{
						K& key = (*it).GetKey();
						V& value = (*it).GetValue();
						memcpy(outData.GetDataPointer() + currentOffset, &key, kSize);
						currentOffset += kSize;
						memcpy(outData.GetDataPointer() + currentOffset, &value, vSize);
						currentOffset += vSize;
					}
				}
			}

			template<typename V>
			static void Serialize(Property<MDictionary<MString, V>>* prop, MFixedArray<MUint8>& outData)
			{
				if(prop == nullptr)
				{
					return;
				}

				MDictionary<MString, V>* dictionary = prop->InternalGet();
				if(dictionary != nullptr)
				{
					const MSize msizeSize = sizeof(MSize);
					const MSize vSize = sizeof(V);
					MSize dictionarySize = dictionary->GetSize();
					MSize dataSize = msizeSize;
					MDictionary<MString, V>::MIteratorDictionary it = dictionary->GetBegin();
					for(; it.IsValid(); ++it)
					{
						dataSize += msizeSize;
						dataSize += (*it).GetKey().Length();
						dataSize += vSize;
					}
					outData.Allocate(dataSize);
					memcpy(outData.GetDataPointer(), &dictionarySize, msizeSize);
					MSize currentOffset = msizeSize;
					MDictionary<MString, V>::MIteratorDictionary it2 = dictionary->GetBegin();
					MSize stringLength = 0;
					if(it2.IsValid())
					{
						for(; it2.IsValid(); ++it2)
						{
							MString& key = (*it2).GetKey();
							stringLength = key.Length();
							memcpy(outData.GetDataPointer() + currentOffset, &stringLength, msizeSize);
							currentOffset += msizeSize;
							const char* text = key;
							memcpy(outData.GetDataPointer() + currentOffset, text, stringLength);
							currentOffset += stringLength;
							V& value = (*it2).GetValue();
							memcpy(outData.GetDataPointer() + currentOffset, &value, vSize);
							currentOffset += vSize;
						}
					}
				}
			}

#pragma endregion

#pragma region Deserialization

			template<typename T>
			static void Deserialize(Property<T>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				T* data = prop->InternalGet();
				if(data != nullptr)
				{
					MSize size = sizeof(T);
					memcpy(data, serializedData.GetDataPointer() + index, size);
					index += size;
				}
			}

			static void DeserializeMAsset(Property<assetLibrary::MAsset*>* prop, MFixedArray<MUint8>& serializedData, MSize& index);
			static void DeserializeMAssetArray(Property<MArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& serializedData, MSize& index);
			static void DeserializeMAssetFixedArray(Property<MFixedArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& serializedData, MSize& index);
			static void DeserializeMEnum(MEnumBase* enumBase, MFixedArray<MUint8>& serializedData, MSize& index);

			template<typename T>
			static void Deserialize(Property<MArray<T>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				
				MArray<T>* dataArray = prop->InternalGet();
				if(dataArray)
				{
					MSize arraySize;
					MSize msizeSize = sizeof(MSize);
					memcpy(&arraySize, dataPointer, msizeSize);
					
					MSize dataSize = sizeof(T) * arraySize;
					dataArray->Resize(arraySize);
					memcpy(dataArray->GetDataPointer(), dataPointer + msizeSize, dataSize);

					index += msizeSize + dataSize;
				}
			}

			template<typename T>
			static void Deserialize(Property<MFixedArray<T>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				
				MFixedArray<T>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize;
					MSize msizeSize = sizeof(MSize);
					memcpy(&arraySize, dataPointer, msizeSize);
					
					MSize dataSize = sizeof(T) * arraySize;
					dataArray->Allocate(arraySize);
					memcpy(dataArray->GetDataPointer(), dataPointer + msizeSize, dataSize);

					index += msizeSize + dataSize;
				}
			}

			template<typename T>
			static void Deserialize(Property<memoryManagement::Handle<T>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;

				memoryManagement::Handle<T>* reference = prop->InternalGet();
				if(reference != nullptr)
				{
					MUint64 id;
					memcpy(&id, dataPointer, sizeof(MUint64));
					reference->StoreID(id);

					index += sizeof(MUint64);
				}
			}

			static void Deserialize(Property<MString>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				MString* data = prop->InternalGet();
				if(data != nullptr)
				{
					const MUint8* dataPointer = serializedData.GetDataPointer() + index;
					MSize stringLength;
					memcpy(&stringLength, dataPointer, sizeof(MSize));

					MArray<char> chars;
					chars.Resize(stringLength);
					memcpy(chars.GetDataPointer(), dataPointer + sizeof(MSize), stringLength);
					(*data) = chars;

					index += sizeof(MSize) + stringLength;
				}
			}

			template<typename T>
			static void Deserialize(Property<MArray<memoryManagement::Handle<T>>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;

				

				MArray<memoryManagement::Handle<T>>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize;
					MSize currentOffset = sizeof(MSize);
					memcpy(&arraySize, dataPointer, currentOffset);

					dataArray->Clear();
					for(MSize i = 0; i < arraySize; ++i)
					{
						MUint64 id;
						memcpy(&id, dataPointer + currentOffset, sizeof(MUint64));
						currentOffset += sizeof(MUint64);

						memoryManagement::Handle<T> handle(id);
						dataArray->Add(handle);
					}

					index += currentOffset;
				}
			}
			
			static void Deserialize(Property<MArray<MString>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				

				MArray<MString>* data = prop->InternalGet();
				if(data != nullptr)
				{
					MSize msizeSize = sizeof(MSize);
					MSize arraySize;
					memcpy(&arraySize, dataPointer, msizeSize);
					
					data->Clear();
					MSize currentOffset = msizeSize;
					MArray<char> chars;
					MSize stringLength;
					for(MSize i = 0; i < arraySize; ++i)
					{
						memcpy(&stringLength, dataPointer + currentOffset, msizeSize);
						currentOffset += msizeSize;
						chars.Resize(stringLength);
						
						memcpy(chars.GetDataPointer(), dataPointer + currentOffset, stringLength);
						currentOffset += stringLength;
						data->Add(chars);
					}

					index += currentOffset;
				}
			}
			
			template<typename T>
			static void Deserialize(Property<MFixedArray<memoryManagement::Handle<T>>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				MFixedArray<memoryManagement::Handle<T>>* dataArray = prop->InternalGet();
				if(dataArray != nullptr)
				{
					MSize arraySize;
					MSize currentOffset = sizeof(MSize);
					memcpy(&arraySize, dataPointer, currentOffset);

					dataArray->Deallocate();
					dataArray->Allocate(arraySize);
					for(MSize i = 0; i < arraySize; ++i)
					{
						MUint64 id;
						memcpy(&id, dataPointer + currentOffset, sizeof(MUint64));
						currentOffset += sizeof(MUint64);

						memoryManagement::Handle<T> handle(id);
						(*dataArray)[i] = handle;
					}

					index += currentOffset;
				}
			}

			static void Deserialize(Property<MFixedArray<MString>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				MFixedArray<MString>* data = prop->InternalGet();
				if(data != nullptr)
				{
					const MSize msizeSize = sizeof(MSize);
					MSize arraySize;
					memcpy(&arraySize, dataPointer, msizeSize);
					
					data->Deallocate();
					data->Allocate(arraySize);
					MSize currentOffset = msizeSize;
					MSize stringLength;
					MArray<char> charArray;
					for(MSize i = 0; i < arraySize; ++i)
					{
						memcpy(&stringLength, dataPointer + currentOffset, msizeSize);
						charArray.Resize(stringLength);
						currentOffset += msizeSize;
						memcpy(charArray.GetDataPointer(), dataPointer + currentOffset, stringLength);
						(*data)[i] = charArray;
						currentOffset += stringLength;
					}

					index += currentOffset;
				}
			}

			template<typename K, typename V>
			static void Deserialize(Property<MDictionary<K, V>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				MDictionary<K, V>* dictionary = prop->InternalGet();
				if(dictionary != nullptr)
				{
					dictionary->Clear();
					const MSize kSize = sizeof(K);
					const MSize vSize = sizeof(V);
					MSize dictionarySize;

					MSize currentOffset = sizeof(MSize);
					memcpy(&dictionarySize, dataPointer, currentOffset);

					K key;
					V value;
					for(MSize i = 0; i < dictionarySize; ++i)
					{
						memcpy(&key, dataPointer + currentOffset, kSize);
						currentOffset += kSize;
						memcpy(&value, dataPointer + currentOffset, vSize);
						currentOffset += vSize;
						dictionary->Add(key, value);
					}

					index += currentOffset;
				}
			}
			
			template<typename V>
			static void Deserialize(Property<MDictionary<MString, V>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
			{
				if(prop == nullptr)
				{
					return;
				}

				const MUint8* dataPointer = serializedData.GetDataPointer() + index;
				MDictionary<MString, V>* data = prop->InternalGet();
				if(data != nullptr)
				{
					data->Clear();
					const MSize msizeSize = sizeof(MSize);
					const MSize vSize = sizeof(V);
					MSize dictionarySize = 0;
					memcpy(&dictionarySize, dataPointer, msizeSize);

					MSize currentOffset = msizeSize;
					MSize stringLength;
					MString key;
					V value;
					MArray<char> chars;
					for(MSize i = 0; i < dictionarySize; ++i)
					{
						memcpy(&stringLength, dataPointer + currentOffset, msizeSize);
						chars.Resize(stringLength);
						currentOffset += msizeSize;
						
						memcpy(chars.GetDataPointer(), dataPointer + currentOffset, stringLength);
						key = chars;
						currentOffset += stringLength;
						
						memcpy(&value, dataPointer + currentOffset, vSize);
						currentOffset += vSize;
						data->Add(key, value);
					}

					index += currentOffset;
				}
			}

#pragma endregion

#pragma region Post deserialization

			template<typename T>
			static void PostDeserialize(Property<T>* prop)
			{
				//Do nothing, since this method is made only for handle initialization after deserialization
			}

			static void PostDeserializeMAsset(Property<assetLibrary::MAsset*>* prop);
			static void PostDeserializeMAssetArray(Property<MArray<assetLibrary::MAsset*>>* prop);
			static void PostDeserializeMAssetFixedArray(Property<MFixedArray<assetLibrary::MAsset*>>* prop);

			template<typename T>
			static void PostDeserialize(Property<memoryManagement::Handle<T>>* prop)
			{
				if(prop == nullptr)
				{
					return;
				}

				memoryManagement::Handle<T>* reference = prop->InternalGet();
				if(reference != nullptr)
				{
					bool succeeded = reference->LoadObjectFromStoredID();
#if _DEBUG
					MString msg = "DEBUG ONLY WARNING: stored id does not match any of serialized objects, object name: ";
					if(prop->_owner.IsValid())
					{
						msg += prop->_owner->GetName();
					}
					msg += ", property name: ";
					msg += prop->GetName();
					ME_WARNING(succeeded, *msg);
#endif
				}
			}
			
			template<typename T>
			static void PostDeserialize(Property<MArray<memoryManagement::Handle<T>>>* prop)
			{
				if(prop == nullptr)
				{
					return;
				}

				MArray<memoryManagement::Handle<T>>* referencesArray = prop->InternalGet();
				if(referencesArray != nullptr)
				{
					MArray<memoryManagement::Handle<T>>::MIteratorArray it = referencesArray->GetBegin();
					for(; it.IsValid(); ++it)
					{
						bool succeeded = (*it).LoadObjectFromStoredID();
#if _DEBUG
						MString msg = "DEBUG ONLY WARNING: stored id does not match any of serialized objects, object name: ";
						if(prop->_owner.IsValid())
						{
							msg += prop->_owner->GetName();
						}
						msg += ", property name: ";
						msg += prop->GetName();
						ME_WARNING(succeeded, *msg);
#endif
					}
				}
			}

			template<typename T>
			static void PostDeserialize(Property<MFixedArray<memoryManagement::Handle<T>>>* prop)
			{
				if(prop == nullptr)
				{
					return;
				}

				MFixedArray<memoryManagement::Handle<T>>* referencesArray = prop->InternalGet();
				if(referencesArray)
				{
					MFixedArray<memoryManagement::Handle<T>>::MIteratorArray it = referencesArray->GetBegin();
					for(; it.IsValid(); ++it)
					{
						bool succeeded = (*it).LoadObjectFromStoredID();
#if _DEBUG
						MString msg = "DEBUG ONLY WARNING: stored id does not match any of serialized objects, object name: ";
						if(prop->_owner.IsValid())
						{
							msg += prop->_owner->GetName();
						}
						msg += ", property name: ";
						msg += prop->GetName();
						ME_WARNING(succeeded, *msg);
#endif
					}
				}
			}

#pragma endregion
		};

		class PropertyChecker
		{
		public:
			template<typename T>
			static inline bool IsHandleProperty(const Property<T>* property)
			{
				return false;
			}

			template<typename T>
			static inline bool IsHandleProperty(const Property<memoryManagement::Handle<T>>* property)
			{
				return true;
			}

			template<typename T>
			static inline bool IsArrayProperty(const Property<T>* property)
			{
				return false;
			}

			template<typename T>
			static inline bool IsArrayProperty(const Property<MArray<T>>* property)
			{
				return true;
			}

			template<typename T>
			static inline bool IsFixedArrayProperty(const Property<T>* property)
			{
				return false;
			}

			template<typename T>
			static inline bool IsFixedArrayProperty(const Property<MFixedArray<T>>* property)
			{
				return true;
			}
			
			template<typename T>
			static inline bool IsAssetArrayProperty(const Property<T>* property)
			{
				return false;
			}

			template<typename T>
			static inline bool IsAssetFixedArrayProperty(const Property<T>* property)
			{
				return false;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MAudio*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MAnimationClip*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MFont*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MMaterial*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MMesh*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MShaderBytecode*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MSkeleton*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MTexture1D*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MTexture2D*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MTexture2DAnimated*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetArrayProperty(const Property<MArray<assetLibrary::MTextureCube*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MAudio*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MAnimationClip*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MFont*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MMaterial*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MMesh*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MShaderBytecode*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MSkeleton*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MTexture1D*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MTexture2D*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MTexture2DAnimated*>>* property)
			{
				return true;
			}

			template<>
			static inline bool IsAssetFixedArrayProperty(const Property<MFixedArray<assetLibrary::MTextureCube*>>* property)
			{
				return true;
			}

			template<typename T>
			static inline const morphEngine::reflection::Type* GetPropertyTypeUnderlying(const Property<T>* property)
			{
				return nullptr;
			}

			template<typename T>
			static inline const morphEngine::reflection::Type* GetPropertyTypeUnderlying(const Property<memoryManagement::Handle<T>>* property)
			{
				return T::GetType();
			}

			template<typename T>
			static inline const morphEngine::reflection::Type* GetPropertyTypeUnderlying(const Property<MArray<memoryManagement::Handle<T>>>* property)
			{
				return T::GetType();
			}

			template<typename T>
			static inline const morphEngine::reflection::Type* GetPropertyTypeUnderlying(const Property<MFixedArray<memoryManagement::Handle<T>>>* property)
			{
				return T::GetType();
			}
		};

		template<typename T>
		inline void Property<T>::Serialize(MFixedArray<MUint8>& outData)
		{
			if(_attributes.HasDefined(morphEngine::reflection::PropertyAttributes::Serializable))
			{
				bool isAssetArray = IsAssetArrayProperty();
				if(isAssetArray)
				{
					Property<MArray<assetLibrary::MAsset*>>* assetArray = reinterpret_cast<Property<MArray<assetLibrary::MAsset*>>*>(this);
					if(assetArray != nullptr)
					{
						PropertySerializer::SerializeMAssetArray(assetArray, outData);
						return;
					}
				}
				bool isAssetFixedArray = IsAssetFixedArrayProperty();
				if(isAssetFixedArray)
				{
					Property<MFixedArray<assetLibrary::MAsset*>>* assetArray = reinterpret_cast<Property<MFixedArray<assetLibrary::MAsset*>>*>(this);
					if(assetArray != nullptr)
					{
						PropertySerializer::SerializeMAssetFixedArray(assetArray, outData);
						return;
					}
				}

				MEnumBase* enumCheck = (MEnumBase*)(InternalGet());
				if(enumCheck != nullptr && enumCheck->DUMMY == VALID_DUMMY)
				{
					PropertySerializer::SerializeMEnum(enumCheck, outData);
					return;
				}

				PropertySerializer::Serialize(this, outData);
			}
		}

		template<typename T>
		inline void Property<T>::Deserialize(MFixedArray<MUint8>& serializedData, MSize& index)
		{
			if(_attributes.HasDefined(morphEngine::reflection::PropertyAttributes::Serializable))
			{
				bool isArray = PropertyChecker::IsAssetArrayProperty(this);
				if(isArray)
				{
					Property<MArray<assetLibrary::MAsset*>>* assetArray = reinterpret_cast<Property<MArray<assetLibrary::MAsset*>>*>(this);
					if(assetArray != nullptr)
					{
						PropertySerializer::DeserializeMAssetArray(assetArray, serializedData, index);
						return;
					}
				}
				bool isFixedArray = PropertyChecker::IsAssetFixedArrayProperty(this);
				if(isFixedArray)
				{
					Property<MFixedArray<assetLibrary::MAsset*>>* assetArray = reinterpret_cast<Property<MFixedArray<assetLibrary::MAsset*>>*>(this);
					if(assetArray != nullptr)
					{
						PropertySerializer::DeserializeMAssetFixedArray(assetArray, serializedData, index);
						return;
					}
				}
				MEnumBase* enumCheck = (MEnumBase*)(InternalGet());
				if(enumCheck != nullptr && enumCheck->DUMMY == VALID_DUMMY)
				{
					PropertySerializer::DeserializeMEnum(enumCheck, serializedData, index);
					return;
				}

				PropertySerializer::Deserialize(this, serializedData, index);
			}
		}

		template<typename T>
		inline void Property<T>::PostDeserialize()
		{
			if(_attributes.HasDefined(morphEngine::reflection::PropertyAttributes::Serializable))
			{
				bool isArray = PropertyChecker::IsAssetArrayProperty(this);
				if(isArray)
				{
					Property<MArray<assetLibrary::MAsset*>>* assetArray = reinterpret_cast<Property<MArray<assetLibrary::MAsset*>>*>(this);
					if(assetArray != nullptr)
					{
						PropertySerializer::PostDeserializeMAssetArray(assetArray);
						return;
					}
				}
				bool isFixedArray = PropertyChecker::IsAssetFixedArrayProperty(this);
				if(isFixedArray)
				{
					Property<MFixedArray<assetLibrary::MAsset*>>* assetArray = reinterpret_cast<Property<MFixedArray<assetLibrary::MAsset*>>*>(this);
					if(assetArray != nullptr)
					{
						PropertySerializer::PostDeserializeMAssetFixedArray(assetArray);
						return;
					}
				}

				PropertySerializer::PostDeserialize(this);
			}
		}
		
		template<typename T>
		inline bool Property<T>::IsHandleProperty() const
		{
			return PropertyChecker::IsHandleProperty(this);
		}

		template<typename T>
		inline bool Property<T>::IsArrayProperty() const
		{
			return PropertyChecker::IsArrayProperty(this);
		}

		template<typename T>
		inline bool Property<T>::IsFixedArrayProperty() const
		{
			return PropertyChecker::IsFixedArrayProperty(this);
		}

		template<typename T>
		inline bool Property<T>::IsAssetArrayProperty() const
		{
			return PropertyChecker::IsAssetArrayProperty(this);
		}

		template<typename T>
		inline bool Property<T>::IsAssetFixedArrayProperty() const
		{
			return PropertyChecker::IsAssetFixedArrayProperty(this);
		}

		template<typename T>
		inline const morphEngine::reflection::Type* Property<T>::GetTypeUnderlying() const
		{
			return PropertyChecker::GetPropertyTypeUnderlying(this);
		}

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MAudio*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MAudio*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MAudio*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MAnimationClip*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MAnimationClip*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MAnimationClip*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MFont*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MFont*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MFont*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MMaterial*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MMaterial*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MMaterial*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MMesh*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MMesh*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MMesh*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MShaderBytecode*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MShaderBytecode*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MShaderBytecode*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MSkeleton*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MSkeleton*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MSkeleton*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture1D*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture1D*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture1D*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture2D*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture2D*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture2D*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture2DAnimated*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture2DAnimated*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTexture2DAnimated*);

		PROPERTY_SERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTextureCube*);
		PROPERTY_DESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTextureCube*);
		PROPERTY_POSTDESERIALIZE_SPECIALIZATION_MASSET(assetLibrary::MTextureCube*);
	}
}