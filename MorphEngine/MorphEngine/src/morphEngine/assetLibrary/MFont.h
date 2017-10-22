#pragma once
#include "assetLibrary/MTexture2D.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MFont :
			public MTexture2D
		{
		public:

			struct CharAlignment
			{
				MFloat32 TopX = 0.0f;
				MFloat32 LeftY = 0.0f;
				MFloat32 Width = 0.0f;
				MFloat32 Height = 0.0f;
				MFloat32 BaselineCorrection = 0.0f;

				CharAlignment() { }
				CharAlignment(MFloat32 tx, MFloat32 ty, MFloat32 w, MFloat32 h, MFloat32 bc) : 
					TopX(tx),
					LeftY(ty),
					Width(w),
					Height(h),
					BaselineCorrection(bc)
				{

				}
			};

		protected:

			static const MInt32 BASE_CHAR_SIZE = 128;

			// X, Y - top left corner, W - width, Z - height
			utility::MFixedArray<CharAlignment> _alignmentTab;
			utility::MString _name;
			MInt32 _charSize = BASE_CHAR_SIZE;

			inline virtual bool CheckType(MAssetType type) { return MAsset::CheckType(type); }
			inline virtual bool CheckType(MUint8* dataPtr) { return MAsset::CheckType(dataPtr); }

			virtual bool LoadMAsset(resourceManagement::fileSystem::File& file) override;
			virtual bool LoadFont(resourceManagement::fileSystem::File& file);

			virtual bool SaveMAsset(resourceManagement::fileSystem::File& file) const override;

			virtual void Shutdown() override;

		public:

			inline MFont() : MTexture2D() { _type = MAssetType::FONT; }
			inline MFont(MAssetType type, const utility::MFixedArray<MUint8>& data) : MTexture2D(type, data) { }
			inline MFont(const MFont& c) : MTexture2D(c),
				_alignmentTab(c._alignmentTab), _name(c._name), _charSize(c._charSize)
			{ 
			}
			inline virtual ~MFont() { Shutdown(); }

			inline virtual bool LoadFromFile(resourceManagement::fileSystem::File& file)
			{
				if (!MTexture2D::LoadFromFile(file))
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
					else if (
						ext == "ttf" ||
						ext == "ttc" || 
						ext == "otf" || 
						ext == "otc" || 
						ext == "pcf" || 
						ext == "fnt")
					{
						return LoadFont(file);
					}
					else
					{
						return false;
					}
				}
				else
				{
					return true;
				}
			}

			inline CharAlignment GetAlignment(char c)
			{
				if (c > _alignmentTab.GetSize())
					c = 0;
				return _alignmentTab[static_cast<MSize>(c)];
			}

			inline MSize GetCharacterSizePixels() { return BASE_CHAR_SIZE; }

		};

	}
}