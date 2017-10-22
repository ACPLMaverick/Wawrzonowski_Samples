#pragma once

#include "assetLibrary/MAsset.h"
#include "renderer/device/GraphicsDevice.h"

namespace morphEngine
{
	namespace assetLibrary
	{

		/// <summary>
		/// Abstract class.
		/// </summary>
		class MTexture : public MAsset
		{
			friend class resourceManagement::ResourceManager;
		protected:

#pragma region Struct

			struct InternalHeader
			{
				renderer::device::GraphicDataFormat _dataFormat;
				renderer::device::TextureSamplerFormat _samplerFormat;
				MSize _totalDataSize;
				MUint16 _width = 0;
				MUint16 _height = 0;
				MUint8 _bpp = 0;
				MUint8 _mipmapCount = 0;
				bool _bReadOnly;
			};

#pragma endregion

#pragma region Protected

			utility::MFixedArray<MUint8> _rawData;

			renderer::device::GraphicDataFormat _dataFormat = renderer::device::GraphicDataFormat::UNORM_R8G8B8A8;
			renderer::device::TextureSamplerFormat _samplerFormat = renderer::device::TextureSamplerFormat::ADDRESS_WRAP_FILTER_LINEAR;
			MUint16 _width = 0;
			MUint16 _height = 0;
			MUint8 _bpp = 0;
			MUint8 _mipmapCount = 0;
			bool _bCompressed = false;
			bool _bReadOnly = false;
			//bool _bIsNormalMap = false;

#pragma endregion


#pragma region Functions Protected

			inline virtual bool CheckType(MAssetType type) override 
			{ 
				return type == MAssetType::TEXTURE_2D || 
					type == MAssetType::TEXTURE_1D || 
					type == MAssetType::TEXTURE_2D_ANIMATED || 
					type == MAssetType::TEXTURE_CUBE;
			}
			inline virtual bool CheckType(MUint8* dataPtr) override
			{
				MAssetType type;
				memcpy(&type, dataPtr, sizeof(MAssetType));
				return CheckType(type);
			}

			inline MUint16 FixedArrayToUInt16(const utility::MFixedArray<MUint8>& array);
			inline void CompressRawDataToDDS(const utility::MFixedArray<MUint8>& rawData, utility::MFixedArray<MUint8>& outDdsData,
				renderer::device::GraphicDataFormat& outFormat, MUint8& outChannels, MUint8& outMipmapCount) const;
			//inline void LoadDDSToRawData(const MFixedArray<MUint8>& ddsData, MFixedArray<MUint8>& outRawData) const;

			// SaveLoadFunctions - they make sure _rawData field is always filled with uniform texture data.
			// Particular texture implementations create different GPU resources from that data.

			virtual bool LoadMAsset(resourceManagement::fileSystem::File& file);
			virtual bool LoadTGA(resourceManagement::fileSystem::File& file);
			virtual bool LoadPNG(resourceManagement::fileSystem::File& file);
			virtual bool LoadDDS(resourceManagement::fileSystem::File& file);

			virtual bool SaveMAsset(resourceManagement::fileSystem::File& file) const;
			virtual bool SaveMAssetInternal(resourceManagement::fileSystem::File& file, 
				const utility::MFixedArray<MUint8>& data,
				renderer::device::GraphicDataFormat savedFormat, MUint8 savedChannels, MUint8 savedMips) const;

			// Texture internal interface

			virtual void InitializeFromRawData() = 0;
			virtual void Shutdown() override;

			//virtual void ConvertToNormalMap();
			//virtual void ConvertFromNormalMap();

#pragma endregion

		public:

#pragma region Functions Public

			//Default constructor
			inline MTexture() : MAsset() { }

			//Constructs MAsset with given type and loads data from given bytes array
			inline MTexture(MAssetType type, const utility::MFixedArray<MUint8>& data) : MAsset(type, data)
			{
			}

			inline MTexture(const MTexture& other) : 
				MAsset(other),
				_rawData(other._rawData),
				_width(other._width),
				_height(other._height),
				_bpp(other._bpp),
				_samplerFormat(other._samplerFormat),
				_dataFormat(other._dataFormat),
				_mipmapCount(other._mipmapCount),
				_bCompressed(other._bCompressed),
				_bReadOnly(other._bReadOnly)
			{
				Shutdown();
				InitializeFromRawData();
			}

			virtual inline ~MTexture()
			{
				// do not call shutdown here because it was called by deriving destructor
			}

			virtual inline MTexture& operator=(const MTexture& other)
			{
				_rawData = other._rawData;
				_width = other._width;
				_height = other._height;
				_bpp = other._bpp;
				_samplerFormat = other._samplerFormat;
				_dataFormat = other._dataFormat;
				_mipmapCount = other._mipmapCount;
				_bCompressed = other._bCompressed;
				_bReadOnly = other._bReadOnly;

				Shutdown();
				InitializeFromRawData();

				return *this;
			}

			//Loads MTexture from given file. Only loading from MASSET file loads settings as well, otherwise they will be set as default.
			inline virtual bool LoadFromFile(resourceManagement::fileSystem::File& file)
			{
				if (!file.IsOpened())
				{
					return false;
				}

				SetPath(file.GetPath());

				utility::MString ext;
				GetExtension(file, ext);

				if (ext == "masset")
				{
					return LoadMAsset(file);
				}
				else if (ext == "tga")
				{
					return LoadTGA(file);
				}
				else if (ext == "dds")
				{
					return LoadDDS(file);
				}
				else if (ext == "png")
				{
					return LoadPNG(file);
				}
				else
				{
					return false;
				}
			}

			//Saves MAsset to given file
			inline virtual bool SaveToFile(resourceManagement::fileSystem::File& file) const
			{
				return SaveMAsset(file);
			}

			//Loads from data with DEFAULT settings (format, sampler, etc.)
			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data);
			//Loads from data with DEFAULT settings (format, sampler, etc.)
			virtual bool LoadFromData(const utility::MArray<MUint8>& data);

			// You might want to change settings before calling this function.
			inline virtual void Initialize()
			{
				InitializeFromRawData();
			}

#pragma region Accessors

			inline MUint16 GetWidth() const { return _width; }
			inline MUint16 GetHeight() const { return _height; }
			inline renderer::device::GraphicDataFormat GetDataFormat() const { return _dataFormat; }
			inline renderer::device::TextureSamplerFormat GetSamplerFormat() const { return _samplerFormat; }
			// Zero means texture has mipmaps generated by graphics device.
			inline MUint8 GetMipmapCount() const { return _mipmapCount; }
			inline bool IsCompressed() const { return _bCompressed; }
			//inline bool IsNormalMap() const { return _bIsNormalMap; }
			inline bool IsMipmapped() const { return _mipmapCount != 1; }
			inline bool IsReadOnly() const { return _bReadOnly; }

			inline void SetDataFormat(renderer::device::GraphicDataFormat format) { _dataFormat = format; }
			inline void SetSamplerFormat(renderer::device::TextureSamplerFormat format) { _samplerFormat = format; }
			inline void SetMipapCount(MUint8 mps) { _mipmapCount = mps; }
			inline void SetCompressed(bool compressed) { _bCompressed = compressed; }
			inline void SetMipmapped(bool mipmapped)
			{
				if (!mipmapped && IsMipmapped())
				{
					_mipmapCount = 1;
				}
				else if (mipmapped && !IsMipmapped())
				{
					_mipmapCount = 0;
				}
			}
			// Setting read only takes effect only after texture was saved to masset and loaded again.
			inline void SetReadOnly(bool ro)
			{
				_bReadOnly = ro;
			}
			//inline void SetNormalMap(bool isNormalMap) 
			//{ 
			//	if (isNormalMap != _bIsNormalMap)
			//	{
			//		isNormalMap ? ConvertToNormalMap() : ConvertFromNormalMap();
			//	}
			//	_bIsNormalMap = isNormalMap; 
			//}

#pragma endregion

#pragma endregion

		};

	}
}