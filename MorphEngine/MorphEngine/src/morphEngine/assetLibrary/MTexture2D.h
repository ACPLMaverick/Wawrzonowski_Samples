#pragma once

#include "assetLibrary/MTexture.h"
#include "renderer/device/GraphicsDevice.h"

namespace morphEngine
{
	namespace renderer
	{
		class RenderTarget2D;
	}

	namespace assetLibrary
	{

		class MTexture2D :
			public MTexture
		{
			friend class resourceManagement::ResourceManager;
		protected:

			renderer::device::Texture2DResource* _res = nullptr;

			// Inherited via MTexture
			virtual inline void InitializeFromRawData() override;
			virtual void Shutdown() override;

			virtual bool SaveMAssetInternal(resourceManagement::fileSystem::File& file, 
				const utility::MFixedArray<MUint8>& data,
				renderer::device::GraphicDataFormat savedFormat, MUint8 savedChannels, MUint8 savedMips) const;

		public:
			inline MTexture2D() : MTexture() { _type = MAssetType::TEXTURE_2D; }
			inline MTexture2D(MAssetType type, const utility::MFixedArray<MUint8>& data) : MTexture(type, data) { }
			inline MTexture2D(const MTexture2D& other) : MTexture(other) { }
			virtual ~MTexture2D();

			inline const renderer::device::Texture2DResource* GetResource() const { return _res; }

			inline void SetAsInputPS(const MUint8 slotRt) const { _res->SetPS(slotRt); }
			inline void SetAsInputVS(const MUint8 slotRt) const { _res->SetVS(slotRt); }
			inline void SetAsInputHS(const MUint8 slotRt) const { _res->SetHS(slotRt); }
			inline void SetAsInputDS(const MUint8 slotRt) const { _res->SetDS(slotRt); }
			inline void SetAsInputGS(const MUint8 slotRt) const { _res->SetGS(slotRt); }
			inline void SetAsInputCS(const MUint8 slotRt) const { _res->SetCS(slotRt); }

			void Merge(const MTexture2D* other, renderer::RenderTarget2D* output) const;
		};

	}
}