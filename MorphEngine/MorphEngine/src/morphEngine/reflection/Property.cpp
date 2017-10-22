#include "Property.h"

#include "resourceManagement\ResourceManager.h"

#include "assetLibrary\MAnimationClip.h"
#include "assetLibrary\MAudio.h"
#include "assetLibrary\MMaterial.h"
#include "assetLibrary\MMesh.h"
#include "assetLibrary\MShaderBytecode.h"
#include "assetLibrary\MSkeleton.h"
#include "assetLibrary\MTexture1D.h"
#include "assetLibrary\MTexture2D.h"
#include "assetLibrary\MTexture2DAnimated.h"
#include "assetLibrary\MTextureCube.h"

using namespace morphEngine::resourceManagement;

namespace morphEngine
{
	namespace reflection
	{
		DEFINE_MENUM(PropertyAttributes);

		assetLibrary::MAsset* PropertyBase::LoadMAssetFromResourceManager(assetLibrary::MAssetType assetType, const utility::MString& path)
		{
			switch(assetType)
			{
			case assetLibrary::MAssetType::ANIMATION_CLIP:
				break;
			case assetLibrary::MAssetType::AUDIO:
				return ResourceManager::GetInstance()->GetAudio(path);
				break;
			case assetLibrary::MAssetType::MATERIAL:
				if(path.Length() > 0)
				{
					return ResourceManager::GetInstance()->GetMaterial(path);
				}
				else
				{
					return ResourceManager::GetInstance()->GetMaterial("DefaultMaterial");
				}
				break;
			case assetLibrary::MAssetType::MESH:
				return ResourceManager::GetInstance()->GetMesh(path);
				break;
			case assetLibrary::MAssetType::SHADER:
				return ResourceManager::GetInstance()->GetShaderBytecode(path);
				break;
			case assetLibrary::MAssetType::SKELETON:
				break;
			case assetLibrary::MAssetType::TEXTURE_1D:
				return ResourceManager::GetInstance()->GetTexture1D(path);
				break;
			case assetLibrary::MAssetType::TEXTURE_2D:
				return ResourceManager::GetInstance()->GetTexture2D(path);
				break;
			case assetLibrary::MAssetType::TEXTURE_2D_ANIMATED:
				break;
			case assetLibrary::MAssetType::TEXTURE_CUBE:
				return ResourceManager::GetInstance()->GetTextureCube(path);
				break;
			}

			return nullptr;
		}

		void PropertySerializer::SerializeMAsset(Property<assetLibrary::MAsset*>* prop, MFixedArray<MUint8>& outData)
		{
			if(prop == nullptr)
			{
				return;
			}

			assetLibrary::MAsset** assetPtr = prop->InternalGet();
			if(assetPtr == nullptr)
			{
				return;
			}
			assetLibrary::MAsset* asset = *assetPtr;
			MString path = "";
			if(asset != nullptr)
			{
				path = asset->GetPath();
			}

			MSize stringLength = path.Length();
			MSize size = sizeof(assetLibrary::MAssetType) + sizeof(MSize) + stringLength;
			outData.Allocate(size);
			
			assetLibrary::MAssetType assetType = assetLibrary::MAssetType::UNKNOWN;
			if(asset != nullptr)
			{
				assetType = asset->GetType();
			}

			MSize offset = sizeof(assetLibrary::MAssetType);
			memcpy(outData.GetDataPointer(), &assetType, offset);
			memcpy(outData.GetDataPointer() + offset, &stringLength, sizeof(MSize));
			offset += sizeof(MSize);
			const char* string = *path;
			memcpy(outData.GetDataPointer() + offset, string, stringLength);
		}

		void PropertySerializer::SerializeMAssetArray(Property<MArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& outData)
		{
			if(prop == nullptr)
			{
				return;
			}

			MArray<assetLibrary::MAsset*>* assetArray = prop->InternalGet();
			if(assetArray == nullptr)
			{
				return;
			}
			MSize arraySize = assetArray->GetSize();
			
			MSize dataSize = sizeof(MSize);
			for(MSize i = 0; i < arraySize; ++i)
			{
				dataSize += sizeof(assetLibrary::MAssetType);
				dataSize += sizeof(MSize);
				if((*assetArray)[i] != nullptr)
				{
					dataSize += (*assetArray)[i]->GetPath().Length();
				}
			}

			outData.Allocate(dataSize);
			memcpy(outData.GetDataPointer(), &arraySize, sizeof(MSize));
			MSize offset = sizeof(MSize);
			for(MSize i = 0; i < arraySize; ++i)
			{
				assetLibrary::MAssetType type = assetLibrary::MAssetType::UNKNOWN;
				MSize stringLength = 0;
				const char* string = nullptr;
				if((*assetArray)[i] != nullptr)
				{
					type = (*assetArray)[i]->GetType();
					stringLength = (*assetArray)[i]->GetPath().Length();
					string = (*assetArray)[i]->GetPath();
				}

				memcpy(outData.GetDataPointer() + offset, &type, sizeof(assetLibrary::MAssetType));
				offset += sizeof(assetLibrary::MAssetType);
				memcpy(outData.GetDataPointer() + offset, &stringLength, sizeof(MSize));
				offset += sizeof(MSize);
				if(stringLength > 0)
				{
					memcpy(outData.GetDataPointer() + offset, string, stringLength);
					offset += stringLength;
				}
			}
		}

		void PropertySerializer::SerializeMAssetFixedArray(Property<MFixedArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& outData)
		{
			if(prop == nullptr)
			{
				return;
			}

			MFixedArray<assetLibrary::MAsset*>* assetArray = prop->InternalGet();
			if(assetArray == nullptr)
			{
				return;
			}
			MSize arraySize = assetArray->GetSize();

			MSize dataSize = sizeof(MSize);
			for(MSize i = 0; i < arraySize; ++i)
			{
				dataSize += sizeof(assetLibrary::MAssetType);
				dataSize += sizeof(MSize);
				if((*assetArray)[i] != nullptr)
				{
					dataSize += (*assetArray)[i]->GetPath().Length();
				}
			}

			outData.Allocate(dataSize);
			memcpy(outData.GetDataPointer(), &arraySize, sizeof(MSize));
			MSize offset = sizeof(MSize);
			for(MSize i = 0; i < arraySize; ++i)
			{
				assetLibrary::MAssetType type = assetLibrary::MAssetType::UNKNOWN;
				MSize stringLength = 0;
				const char* string = nullptr;
				if((*assetArray)[i] != nullptr)
				{
					type = (*assetArray)[i]->GetType();
					stringLength = (*assetArray)[i]->GetPath().Length();
					string = (*assetArray)[i]->GetPath();
				}

				memcpy(outData.GetDataPointer() + offset, &type, sizeof(assetLibrary::MAssetType));
				offset += sizeof(assetLibrary::MAssetType);
				memcpy(outData.GetDataPointer() + offset, &stringLength, sizeof(MSize));
				offset += sizeof(MSize);
				if(stringLength > 0)
				{
					memcpy(outData.GetDataPointer() + offset, string, stringLength);
					offset += stringLength;
				}
			}
		}

		void PropertySerializer::SerializeMEnum(MEnumBase* enumBase, MFixedArray<MUint8>& outData)
		{
			if(enumBase == nullptr)
			{
				return;
			}

			outData.Allocate(enumBase->GetSizeOf());
			memcpy(outData.GetDataPointer(), enumBase->GetValuePtr(), enumBase->GetSizeOf());
		}

		void PropertySerializer::DeserializeMAsset(Property<assetLibrary::MAsset*>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
		{
			if(prop == nullptr)
			{
				return;
			}

			assetLibrary::MAsset** assetPtr = prop->InternalGet();
			if(assetPtr == nullptr)
			{
				return;
			}

			const MUint8* dataPointer = serializedData.GetDataPointer() + index;
			assetLibrary::MAssetType assetType;
			MSize offset = sizeof(assetLibrary::MAssetType);
			memcpy(&assetType, dataPointer, offset);
			MSize stringLength;
			memcpy(&stringLength, dataPointer + offset, sizeof(MSize));
			offset += sizeof(MSize);
			MArray<char> stringArray;
			stringArray.Resize(stringLength);
			memcpy(stringArray.GetDataPointer(), dataPointer + offset, stringLength);
			offset += stringLength;

			if(stringLength > 0)
			{
				assetLibrary::MAsset* asset = *assetPtr;
				if(asset == nullptr)
				{
					(*assetPtr) = new assetLibrary::MAsset();
				}
				(*assetPtr)->SetType(assetType);
				(*assetPtr)->SetPath(stringArray);
			}

			index += offset;
		}

		void PropertySerializer::DeserializeMAssetArray(Property<MArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
		{
			if(prop == nullptr)
			{
				return;
			}

			MArray<assetLibrary::MAsset*>* assetArray = prop->InternalGet();
			if(assetArray == nullptr)
			{
				return;
			}
			
			MUint8 const* dataPtr = serializedData.GetDataPointer() + index;
			MSize arraySize;
			memcpy(&arraySize, dataPtr, sizeof(MSize));
			MSize offset = sizeof(MSize);
			assetArray->Resize(arraySize);
			for(MSize i = 0; i < arraySize; ++i)
			{
				assetLibrary::MAssetType type;
				MSize stringLength;
				MArray<char> stringArray;

				memcpy(&type, dataPtr + offset, sizeof(assetLibrary::MAssetType));
				offset += sizeof(assetLibrary::MAssetType);
				memcpy(&stringLength, dataPtr + offset, sizeof(MSize));
				offset += sizeof(MSize);
				stringArray.Resize(stringLength);
				memcpy(stringArray.GetDataPointer(), dataPtr + offset, stringLength);
				offset += stringLength;

				assetLibrary::MAsset* asset = (*assetArray)[i];
				if(asset == nullptr)
				{
					(*assetArray)[i] = new assetLibrary::MAsset();
				}
				(*assetArray)[i]->SetType(type);
				if(stringArray.GetSize() > 0)
				{
					(*assetArray)[i]->SetPath(stringArray);
				}
				else
				{
					(*assetArray)[i]->SetPath("");
				}
			}

			index += offset;
		}

		void PropertySerializer::DeserializeMAssetFixedArray(Property<MFixedArray<assetLibrary::MAsset*>>* prop, MFixedArray<MUint8>& serializedData, MSize& index)
		{
			if(prop == nullptr)
			{
				return;
			}

			MFixedArray<assetLibrary::MAsset*>* assetArray = prop->InternalGet();
			if(assetArray == nullptr)
			{
				return;
			}

			MUint8 const* dataPtr = serializedData.GetDataPointer() + index;
			MSize arraySize;
			memcpy(&arraySize, dataPtr, sizeof(MSize));
			MSize offset = sizeof(MSize);
			assetArray->Deallocate();
			assetArray->Allocate(arraySize);
			for(MSize i = 0; i < arraySize; ++i)
			{
				assetLibrary::MAssetType type;
				MSize stringLength;
				MArray<char> stringArray;

				memcpy(&type, dataPtr + offset, sizeof(assetLibrary::MAssetType));
				offset += sizeof(assetLibrary::MAssetType);
				memcpy(&stringLength, dataPtr + offset, sizeof(MSize));
				offset += sizeof(MSize);
				stringArray.Resize(stringLength);
				memcpy(stringArray.GetDataPointer(), dataPtr + offset, stringLength);
				offset += stringLength;

				assetLibrary::MAsset* asset = (*assetArray)[i];
				if(asset == nullptr)
				{
					(*assetArray)[i] = new assetLibrary::MAsset();
				}
				(*assetArray)[i]->SetType(type);
				if(stringArray.GetSize() > 0)
				{
					(*assetArray)[i]->SetPath(stringArray);
				}
				else
				{
					(*assetArray)[i]->SetPath("");
				}
			}

			index += offset;
		}

		void PropertySerializer::DeserializeMEnum(MEnumBase* enumBase, MFixedArray<MUint8>& serializedData, MSize& index)
		{
			if(enumBase == nullptr)
			{
				return;
			}

			MUint8* dataPtr = serializedData.GetDataPointer() + index;
			memcpy(enumBase->GetValuePtr(), dataPtr, enumBase->GetSizeOf());
			index += enumBase->GetSizeOf();
		}

		void PropertySerializer::PostDeserializeMAsset(Property<assetLibrary::MAsset*>* prop)
		{
			if(prop == nullptr)
			{
				return;
			}

			assetLibrary::MAsset** assetPtr = prop->InternalGet();
			if(assetPtr == nullptr)
			{
				return;
			}

			assetLibrary::MAsset* asset = *assetPtr;
			if(asset == nullptr)
			{
				return;
			}

			assetLibrary::MAssetType assetType = asset->GetType();
			MString path = asset->GetPath();

			delete asset;
			asset = nullptr;

			assetLibrary::MAsset* loadedAsset = PropertyBase::LoadMAssetFromResourceManager(assetType, path);

			if(loadedAsset != nullptr)
			{
				prop->Set(loadedAsset);
			}
		}

		void PropertySerializer::PostDeserializeMAssetArray(Property<MArray<assetLibrary::MAsset*>>* prop)
		{
			if(prop == nullptr)
			{
				return;
			}

			MArray<assetLibrary::MAsset*>* assetArray = prop->InternalGet();
			if(assetArray == nullptr)
			{
				return;
			}

			MSize arraySize = assetArray->GetSize();
			for(MSize i = 0; i < arraySize; ++i)
			{
				if((*assetArray)[i] == nullptr)
				{
					continue;
				}

				assetLibrary::MAssetType assetType = (*assetArray)[i]->GetType();
				MString path = (*assetArray)[i]->GetPath();

				delete (*assetArray)[i];
				(*assetArray)[i] = nullptr;

				assetLibrary::MAsset* loadedAsset = PropertyBase::LoadMAssetFromResourceManager(assetType, path);

				if(loadedAsset != nullptr)
				{
					(*assetArray)[i] = loadedAsset;
				}
			}
		}

		void PropertySerializer::PostDeserializeMAssetFixedArray(Property<MFixedArray<assetLibrary::MAsset*>>* prop)
		{
			if(prop == nullptr)
			{
				return;
			}

			MFixedArray<assetLibrary::MAsset*>* assetArray = prop->InternalGet();
			if(assetArray == nullptr)
			{
				return;
			}

			MSize arraySize = assetArray->GetSize();
			for(MSize i = 0; i < arraySize; ++i)
			{
				if((*assetArray)[i] == nullptr)
				{
					continue;
				}

				assetLibrary::MAssetType assetType = (*assetArray)[i]->GetType();
				MString path = (*assetArray)[i]->GetPath();

				delete (*assetArray)[i];
				(*assetArray)[i] = nullptr;

				assetLibrary::MAsset* loadedAsset = PropertyBase::LoadMAssetFromResourceManager(assetType, path);

				if(loadedAsset != nullptr)
				{
					(*assetArray)[i] = loadedAsset;
				}
			}
		}
	}
}
