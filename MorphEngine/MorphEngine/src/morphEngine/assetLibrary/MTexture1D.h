#pragma once

#include "assetLibrary/MTexture.h"

namespace morphEngine
{
	namespace assetLibrary
	{

		class MTexture1D :
			public MTexture
		{
			friend class resourceManagement::ResourceManager;
		protected:

			renderer::device::Texture1DResource* _res = nullptr;

			// Inherited via MTexture
			virtual inline void InitializeFromRawData() override;
			virtual void Shutdown() override;

		public:
			inline MTexture1D() : MTexture() { _type = MAssetType::TEXTURE_1D; }
			inline MTexture1D(MAssetType type, const utility::MFixedArray<MUint8>& data) : MTexture(type, data) { }
			inline MTexture1D(const MTexture& other) : MTexture(other) { }
			virtual ~MTexture1D();
		};

	}
}