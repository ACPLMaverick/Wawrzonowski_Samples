#pragma once
#include "assetLibrary/MMesh.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MFont;
	}

	namespace renderer
	{
		namespace meshes
		{
			class MeshText :
				public assetLibrary::MMesh
			{
			public:

				enum class Alignment
				{
					LEFT,
					CENTER,
					RIGHT
				};

			protected:

				struct LineData
				{
					MSize StartIndex;
					MFloat32 Width;

					LineData() : StartIndex(0), Width(0) { }
					LineData(MSize startIndex, MFloat32 w) : StartIndex(startIndex), Width(w) { }

					bool operator==(const LineData& o) const { return StartIndex == o.StartIndex && Width == o.Width; }
					bool operator!=(const LineData& o) const { return !operator==(o); }
				};

				utility::MString _text = "Hello Morph.";
				utility::MVector2 _unscaledSize = utility::MVector2::Zero;
				Alignment _alignment = Alignment::CENTER;
				assetLibrary::MFont* _font = nullptr;

				void CalculateVertices(bool bSameLength);

			public:
				//Default constructor
				inline MeshText() : MMesh()
				{
				}

				//Constructs MMesh from given data (i.e. file bytes array)
				inline MeshText(const utility::MFixedArray<MUint8>& data) : MMesh(data)
				{
				}

				//Copy constructor
				inline MeshText(const MeshText& other) : MMesh(other)
				{
				}

				virtual ~MeshText() 
				{
				}

				virtual void Initialize() override;

				inline void SetText(const utility::MString& text) 
				{ 
					if (text != _text)
					{
						bool bSameLength = text.Length() == _text.Length();
						_text = text;
						CalculateVertices(bSameLength);
					}
				}
				inline void SetFont(assetLibrary::MFont* font) 
				{ 
					if (font != _font)
					{
						_font = font;
						CalculateVertices(true);
					}
				}
				inline void SetAlignment(Alignment alignment)
				{
					if (alignment != _alignment)
					{
						_alignment = alignment;
						CalculateVertices(true);
					}
				}

				inline const utility::MString& GetText() const { return _text; }
				inline assetLibrary::MFont* GetFont() const { return _font; }
				inline Alignment GetAlignment() const { return _alignment; }
				inline utility::MVector2 GetTotalSize() const { return _unscaledSize; }
			};
		}
	}
}