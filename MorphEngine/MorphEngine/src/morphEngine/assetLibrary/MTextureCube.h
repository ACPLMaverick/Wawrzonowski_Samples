#pragma once

#include "assetLibrary/MTexture.h"

namespace morphEngine
{
	namespace assetLibrary
	{

		class MTextureCube :
			public MTexture
		{
			friend class resourceManagement::ResourceManager;
		protected:

			utility::MFixedArray<MUint8> _rawDataPlusX;
			utility::MFixedArray<MUint8> _rawDataPlusY;
			utility::MFixedArray<MUint8> _rawDataPlusZ;
			utility::MFixedArray<MUint8> _rawDataMinusX;
			utility::MFixedArray<MUint8> _rawDataMinusY;
			utility::MFixedArray<MUint8> _rawDataMinusZ;

			renderer::device::TextureCubeResource* _res;

			// Inherited via MTexture
			virtual inline void InitializeFromRawData() override;
			virtual inline void Shutdown() override;

			virtual bool LoadMAsset(resourceManagement::fileSystem::File& file) override;
			virtual bool LoadDDS(resourceManagement::fileSystem::File& file) override;

			virtual bool SaveMAsset(resourceManagement::fileSystem::File& file) const;

			inline void SplitRawDataIntoFaces();

		public:
			inline MTextureCube() : MTexture() { _type = MAssetType::TEXTURE_CUBE; }
			inline MTextureCube(MAssetType type, const utility::MFixedArray<MUint8>& data) : MTexture(type, data) { }
			inline MTextureCube(const MTextureCube& other) : MTexture(other) { }
			virtual ~MTextureCube();

			renderer::device::TextureCubeResource* GetResource() const { return _res; }
		};

	}
}