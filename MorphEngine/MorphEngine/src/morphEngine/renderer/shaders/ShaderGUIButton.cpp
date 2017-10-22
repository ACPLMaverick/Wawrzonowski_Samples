#include "ShaderGUIButton.h"
#include "assetLibrary/MMaterial.h"
#include "assetLibrary/MMesh.h"
#include "assetLibrary/MTexture2D.h"
#include "gom/GUITransform.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			const char* ShaderGUIButton::TYPE_NAME = "ShaderGUIButton";

			ShaderGUIButton::ShaderGUIButton()
			{
			}


			ShaderGUIButton::~ShaderGUIButton()
			{
			}

			void ShaderGUIButton::SetupMaterialInterface(assetLibrary::MMaterial & mat)
			{
				mat.AddSlotColor();
				mat.AddSlotColor();
				mat.AddSlotColor();
				mat.AddSlotColor();
				mat.AddSlotTexture2D();
				mat.AddSlotTexture2D();
				mat.AddSlotTexture2D();
				mat.AddSlotTexture2D();
				mat.AddSlotUint();
				mat.AddSlotFloat();
			}

			void ShaderGUIButton::SetLocal(const assetLibrary::MMaterial & mat, const gom::Camera & camera, const gom::Transform & transform, const assetLibrary::MMesh & mesh) const
			{
				mesh.GetVertexPositionsResource()->Set(0);
				mesh.GetVertexUVsResource()->Set(1);
				mesh.GetIndicesResouce()->Set();

				BufferDataVS* cbPtr;
				_cBufferVS->Map(reinterpret_cast<void**>(&cbPtr));
				cbPtr->_guiMatrix = reinterpret_cast<const gom::GUITransform&>(transform).GetGUIMatrix();
				_cBufferVS->Unmap();

				_cBufferVS->SetVS(0);

				BufferDataPS* buf;
				_cBufferPS->Map(reinterpret_cast<void**>(&buf));

				buf->_baseColor = mat.GetColor(0);
				buf->_baseColor1 = mat.GetColor(1);
				buf->_baseColor2 = mat.GetColor(2);
				buf->_baseColor3 = mat.GetColor(3);
				buf->_indices = mat.GetUint(0);
				buf->_lerp = mat.GetFloat(0);

				_cBufferPS->Unmap();

				_cBufferPS->SetPS(0);

				mat.GetTexture2D(0)->SetAsInputPS(0);
				mat.GetTexture2D(1)->SetAsInputPS(1);
				mat.GetTexture2D(2)->SetAsInputPS(2);
				mat.GetTexture2D(3)->SetAsInputPS(3);
			}
		}
	}
}