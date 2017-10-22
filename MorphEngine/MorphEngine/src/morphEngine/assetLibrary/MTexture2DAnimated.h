#pragma once
#include "assetLibrary/MTexture2D.h"

namespace morphEngine
{
	namespace assetLibrary
	{

		class MTexture2DAnimated :
			public MTexture2D
		{
			friend class resourceManagement::ResourceManager;
		public:
			inline MTexture2DAnimated() : MTexture2D() { _type = MAssetType::TEXTURE_2D_ANIMATED; }
			inline MTexture2DAnimated(MAssetType type, const utility::MFixedArray<MUint8>& data) : MTexture2D(type, data) { }
			inline MTexture2DAnimated(const MTexture2DAnimated& other) : MTexture2D(other) { }
			virtual ~MTexture2DAnimated();
		};

	}
}