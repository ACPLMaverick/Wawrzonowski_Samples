#pragma once

#include "core/GlobalDefines.h"
#include "resourceManagement\fileSystem\FileSystem.h"
#include "utility\MString.h"
#include "utility\MArray.h"

#include <fstream>

namespace morphEngine
{
	namespace resourceManagement
	{
		class ResourceManager;
	}

	namespace assetLibrary
	{
		//Enum indicating the type of the asset
		enum class MAssetType : MUint8
		{
			MESH,
			SKELETON,
			ANIMATION_CLIP,
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_2D_ANIMATED,
			TEXTURE_CUBE,
			AUDIO,
			SHADER,
			MATERIAL,
			FONT,
			UNKNOWN
		};

		//MAsset structure:
		//MAssetType
		//(optional) MAsset header (i.e. MMeshHeader)
		//data
		class MAsset
		{
			friend class resourceManagement::ResourceManager;
		protected:
			MAssetType _type;
			utility::MArray<MAsset*> _instances;
			utility::MString _path;
			MAsset* _original = nullptr;
			bool _bIsInternal = false;

			inline virtual bool CheckType(MAssetType type) { return _type == type; }
			inline virtual bool CheckType(MUint8* dataPtr) 
			{
				MAssetType type;
				memcpy(&type, dataPtr, sizeof(MAssetType));
				return _type == type; 
			}

			inline void GetExtension(resourceManagement::fileSystem::File& file, utility::MString& outString)
			{
				const utility::MString& filePath = file.GetPath();
				MSize lastDot = filePath.FindLast('.');
				outString = filePath.Substring(lastDot + 1, filePath.Length() - 1 - lastDot);
				outString.ToLower();
			}

			virtual void Shutdown() { }

		public:
			//Default constructor
			inline MAsset() : _type(MAssetType::UNKNOWN) { }
			//Constructs MAsset with given type and loads data from given bytes array
			inline MAsset(MAssetType type, const utility::MFixedArray<MUint8>& data) : _type(type)
			{
				LoadFromData(data);
			}
			inline MAsset(const MAsset& other) : _type(other._type), _path(other._path), _bIsInternal(other._bIsInternal)
			{
				
			}

			inline virtual ~MAsset()
			{
				if (GetIsInstance())
				{
					_original->_instances.Remove(this);
				}
				else
				{
					for (auto it = _instances.GetBegin(); it.IsValid(); ++it)
					{
						// when orignal is destroyed, all instances cease to be instances
						(*it)->_original = nullptr;
					}
				}
			}

			template <typename T>
			inline T* CreateInstance()
			{
				if (GetIsInstance())
				{
					return _original->CreateInstance<T>();
				}
				else
				{
					T* ptr = new T(reinterpret_cast<T&>(*this));
					ptr->_original = this;
					_instances.Add(ptr);
					return ptr;
				}
			}

			//Returns the type of the asset (i.e. MAssetType::MESH)
			inline MAssetType GetType() const
			{
				return _type;
			}

			inline bool GetIsInstance() const
			{
				return _original != nullptr;
			}

			inline MAsset* GetOriginal() const
			{
				return _original;
			}

			//For Property::Deserialize purposes
			inline void SetType(MAssetType type)
			{
				_type = type;
			}

			//For Property::Serialize purposes
			inline const utility::MString& GetPath() const
			{
				return _path;
			}

			//For Property::Deserialize purposes
			inline void SetPath(const utility::MString& path)
			{
				_path = utility::MString::TrimFromStart(path, '/');
				_path = utility::MString::TrimFromStart(_path, '\\');
				MInt32 index = _path.FindFirstNCS("Resources");
				if(index >= 0)
				{
					index += (MInt32)utility::MString("Resources").Length();
					_path = _path.Substring(index);
				}
				_path = utility::MString::TrimFromStart(_path, '/');
				_path = utility::MString::TrimFromStart(_path, '\\');
				_path = path;
			}

			inline void SetIsInternal(bool isInternal) { _bIsInternal = isInternal; }
			inline bool GetIsInternal() { return _bIsInternal; }

			//Loads MAsset data from given file data
			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data)
			{
				return false;
			}
			//Loads MAsset data from given file data
			virtual bool LoadFromData(const utility::MArray<MUint8>& data)
			{
				return false;
			}
			//Loads MAsset from given file
			inline virtual bool LoadFromFile(resourceManagement::fileSystem::File& file)
			{
				return false;
			}
			//Saves MAsset to given file
			inline virtual bool SaveToFile(resourceManagement::fileSystem::File& file) const
			{
				return false;
			}
			//Reloads MAsset using its path
			virtual bool Reload();
			//Initializes masset
			inline virtual void Initialize()
			{
				
			}
		};
	}
}