#include "GUIText.h"
#include "resourceManagement/ResourceManager.h"
#include "gom/GUITransform.h"

#define TM(meshPtr) reinterpret_cast<MeshText*>(meshPtr)

namespace morphEngine
{
	using namespace resourceManagement;
	using namespace utility;
	using namespace gom;
	using namespace memoryManagement;

	namespace renderer
	{

		using namespace meshes;

		GUIText::GUIText(const gom::ObjectInitializer& initializer) : 
			GUIImage(initializer)
		{
		}

		GUIText::GUIText(const GUIText & copy, bool bDeepCopy) :
			GUIImage(copy, bDeepCopy)
		{
			if (bDeepCopy)
			{
				AssignDefaultMesh();
			}
		}

		GUIText::~GUIText()
		{
		}

		void GUIText::Initialize()
		{
			GUIImage::Initialize();
			FitSizeToText();
		}

		void GUIText::Shutdown()
		{
			// destroy mesh
			delete _drawMesh;

			GUIImage::Shutdown();
		}

		void GUIText::SetMaterial(assetLibrary::MMaterial * material)
		{
			if (material->GetSlotCountUint() == 0)
			{
				ME_WARNING_RETURN_STATEMENT(false, "Assigining wrong material to GUIText.");
			}
			_drawMaterial = material;
			_drawMaterial->SetUint(ResourceManager::GetInstance()->GUI_SHADER_PARAM_TEXT, 0);
		}

		void GUIText::SetText(const utility::MString & text)
		{
			TM(_drawMesh)->SetText(text);
			FitSizeToText();
		}

		const utility::MString & GUIText::GetText()
		{
			return TM(_drawMesh)->GetText();
		}

		void GUIText::SetFont(assetLibrary::MFont * font)
		{
			TM(_drawMesh)->SetFont(font);
			FitSizeToText();
		}

		assetLibrary::MFont * GUIText::GetFont()
		{
			return TM(_drawMesh)->GetFont();
		}

		void GUIText::SetAlignment(renderer::meshes::MeshText::Alignment alignment)
		{
			TM(_drawMesh)->SetAlignment(alignment);
		}

		renderer::meshes::MeshText::Alignment GUIText::GetAlignment() const
		{
			return TM(_drawMesh)->GetAlignment();
		}

		void GUIText::AssignDefaultMesh()
		{
			// create own text mesh which will be non-changing through whole life of this object.
			if (_drawMesh == nullptr)
			{
				_drawMesh = new MeshText();
				TM(_drawMesh)->Initialize();
			}
		}

		void GUIText::AssignDefaultMaterial()
		{
			_drawMaterial = ResourceManager::GetInstance()->CreateMaterialInstance("DefaultGUIFontMaterial");
		}

		inline void GUIText::FitSizeToText()
		{
			// calculate new size from set text
			MVector2 textSize = TM(_drawMesh)->GetTotalSize();
			Handle<GUITransform> guiT = GetGUITransform();

			// favorize height
			ME_ASSERT_S(textSize.Y != 0.0f);
			textSize = MVector2(guiT->GetSize().Y, guiT->GetSize().Y) * MVector2(textSize.X / textSize.Y, 1.0f);
			guiT->SetSize(textSize);
		}
	}
}