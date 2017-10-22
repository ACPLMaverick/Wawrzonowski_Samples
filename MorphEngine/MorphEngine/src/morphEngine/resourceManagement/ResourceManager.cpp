#include "ResourceManager.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <fbxsdk.h>

namespace morphEngine
{
	using namespace assetLibrary;

	namespace resourceManagement
	{
		void ResourceManager::Initialize()
		{
			ilInit();
			iluInit();
			auto error = FT_Init_FreeType(&_ftLibrary);
			ME_ASSERT(!error, "Could not initialize FreeType library.");
			_fbxManager = fbxsdk::FbxManager::Create();

			// ///////////////////////////

			renderer::meshes::MeshPlane* meshPlane = new renderer::meshes::MeshPlane();
			meshPlane->Initialize();
			meshPlane->SetIsInternal(true);
			_meshCache.AddAsset("MeshPlane", meshPlane);

			renderer::meshes::MeshBox* meshBox = new renderer::meshes::MeshBox();
			meshBox->Initialize();
			meshBox->SetIsInternal(true);
			_meshCache.AddAsset("MeshBox", meshBox);

			renderer::meshes::MeshBox* meshBoxFlipped = new renderer::meshes::MeshBox();
			meshBoxFlipped->Initialize();
			meshBoxFlipped->FlipTrianglesAndNormals();
			meshBoxFlipped->SetIsInternal(true);
			_meshCache.AddAsset("MeshBoxFlipped", meshBoxFlipped);

			renderer::meshes::MeshPlane* guiMeshPlane = new renderer::meshes::MeshPlane();
			guiMeshPlane->Initialize();
			guiMeshPlane->SetIsInternal(true);
			guiMeshPlane->SetScaleAndTiling(MVector2(2.0f, 2.0f), MVector2(1.0f, 1.0f));
			_meshCache.AddAsset("GUIMeshPlane", guiMeshPlane);

			// Create placeholder assets
			MUint32 blankDiffuseData = 0xFFFFFFFF;
			MTexture2D* blankDiffuse = new MTexture2D();
			blankDiffuse->_width = 1;
			blankDiffuse->_height = 1;
			blankDiffuse->_bpp = 32;
			blankDiffuse->_samplerFormat = renderer::device::TextureSamplerFormat::ADDRESS_CLAMP_FILTER_NEAREST;
			blankDiffuse->_dataFormat = renderer::device::GraphicDataFormat::UNORM_R8G8B8A8;
			blankDiffuse->_mipmapCount = 1;
			blankDiffuse->_bCompressed = false;
			blankDiffuse->_bReadOnly = true;
			blankDiffuse->_rawData.Allocate(sizeof(MUint32));
			memcpy(blankDiffuse->_rawData.GetDataPointer(), &blankDiffuseData, sizeof(MUint32));
			blankDiffuse->Initialize();
			blankDiffuse->SetIsInternal(true);
			_tex2DCache.AddAsset("BlankDiffuse", blankDiffuse);

			MTexture1D* blankDiff1D = new MTexture1D();
			blankDiff1D->_width = 1;
			blankDiff1D->_height = 1;
			blankDiff1D->_bpp = 32;
			blankDiff1D->_samplerFormat = renderer::device::TextureSamplerFormat::ADDRESS_CLAMP_FILTER_NEAREST;
			blankDiff1D->_dataFormat = renderer::device::GraphicDataFormat::UNORM_R8G8B8A8;
			blankDiff1D->_mipmapCount = 1;
			blankDiff1D->_bCompressed = false;
			blankDiff1D->_bReadOnly = true;
			blankDiff1D->_rawData.Allocate(sizeof(MUint32));
			memcpy(blankDiff1D->_rawData.GetDataPointer(), &blankDiffuseData, sizeof(MUint32));
			blankDiff1D->Initialize();
			blankDiff1D->SetIsInternal(true);
			_tex1DCache.AddAsset("BlankDiffuse1D", blankDiff1D);

			MTextureCube* blankDiffCube = new MTextureCube();
			blankDiffCube->_width = 1;
			blankDiffCube->_height = 1;
			blankDiffCube->_bpp = 32;
			blankDiffCube->_samplerFormat = renderer::device::TextureSamplerFormat::ADDRESS_CLAMP_FILTER_NEAREST;
			blankDiffCube->_dataFormat = renderer::device::GraphicDataFormat::UNORM_R8G8B8A8;
			blankDiffCube->_mipmapCount = 1;
			blankDiffCube->_bCompressed = false;
			blankDiffCube->_bReadOnly = true;

			MUint32 cubeDataPosX = 0xFF0000FF;
			MUint32 cubeDataPosY = 0xFF00FF00;
			MUint32 cubeDataPosZ = 0xFFFF0000;
			MUint32 cubeDataNegX = 0xFFFFFF00;
			MUint32 cubeDataNegY = 0xFFFF00FF;
			MUint32 cubeDataNegZ = 0xFF00FFFF;
			blankDiffCube->_rawDataPlusX.Allocate(sizeof(MUint32));
			memcpy(blankDiffCube->_rawDataPlusX.GetDataPointer(), &cubeDataPosX, sizeof(MUint32));
			blankDiffCube->_rawDataPlusY.Allocate(sizeof(MUint32));
			memcpy(blankDiffCube->_rawDataPlusY.GetDataPointer(), &cubeDataPosY, sizeof(MUint32));
			blankDiffCube->_rawDataPlusZ.Allocate(sizeof(MUint32));
			memcpy(blankDiffCube->_rawDataPlusZ.GetDataPointer(), &cubeDataPosZ, sizeof(MUint32));
			blankDiffCube->_rawDataMinusX.Allocate(sizeof(MUint32));
			memcpy(blankDiffCube->_rawDataMinusX.GetDataPointer(), &cubeDataNegX,sizeof(MUint32));
			blankDiffCube->_rawDataMinusY.Allocate(sizeof(MUint32));
			memcpy(blankDiffCube->_rawDataMinusY.GetDataPointer(), &cubeDataNegY, sizeof(MUint32));
			blankDiffCube->_rawDataMinusZ.Allocate(sizeof(MUint32));
			memcpy(blankDiffCube->_rawDataMinusZ.GetDataPointer(), &cubeDataNegZ, sizeof(MUint32));

			blankDiffCube->Initialize();
			blankDiffCube->SetIsInternal(true);
			_texCubeCache.AddAsset("BlankDiffuseCube", blankDiffCube);

			MUint8 blankNormalData[16]{ 127, 127, 255, 255 };
			MTexture2D* blankNormal = new MTexture2D();
			blankNormal->_width = 1;
			blankNormal->_height = 1;
			blankNormal->_bpp = 32;
			blankNormal->_samplerFormat = renderer::device::TextureSamplerFormat::ADDRESS_CLAMP_FILTER_NEAREST;
			blankNormal->_dataFormat = renderer::device::GraphicDataFormat::UNORM_R8G8B8A8;
			blankNormal->_mipmapCount = 1;
			blankNormal->_bCompressed = false;
			blankNormal->_bReadOnly = true;
			blankNormal->_rawData.Allocate(sizeof(MUint32));
			memcpy(blankNormal->_rawData.GetDataPointer(), blankNormalData, sizeof(MUint32));
			blankNormal->Initialize();
			blankNormal->SetIsInternal(true);
			_tex2DCache.AddAsset("BlankNormal", blankNormal);

			renderer::shaders::ShaderObject* blankShader = reinterpret_cast<renderer::shaders::ShaderObject*>(GetShader("ShaderBlank"));
			ME_ASSERT_S(blankShader != nullptr);


			MMaterial* defaultMat = new MMaterial();
			defaultMat->SetShader(blankShader);
			defaultMat->SetIsInternal(true);
			_materialCache.AddAsset("DefaultMaterial", defaultMat);

			renderer::shaders::ShaderObject* guiShader = reinterpret_cast<renderer::shaders::ShaderObject*>(GetShader("ShaderGUI"));
			renderer::shaders::ShaderObject* buttonShader = reinterpret_cast<renderer::shaders::ShaderObject*>(GetShader("ShaderGUIButton"));

			MMaterial* guiMat = new MMaterial();
			guiMat->SetShader(guiShader);
			guiMat->SetIsInternal(true);
			guiMat->SetTexture2D(*blankDiffuse, 0);
			guiMat->SetColor(MColor::Magenta, 0);
			guiMat->SetPath("DefaultGUIMaterial");
			guiMat->SetUint(GUI_SHADER_PARAM_IMAGE, 0);
			_materialCache.AddAsset("DefaultGUIMaterial", guiMat);

			MFont* arial = GetFont(DEFAULT_FONT_PATH);

			MMaterial* fontMat = new MMaterial();
			fontMat->SetShader(guiShader);
			fontMat->SetIsInternal(true);
			fontMat->SetTexture2D(*arial, 0);
			fontMat->SetColor(MColor::White, 0);
			fontMat->SetUint(GUI_SHADER_PARAM_TEXT, 0);
			fontMat->SetPath("DefaultGUIFontMaterial");
			_materialCache.AddAsset("DefaultGUIFontMaterial", fontMat);

			MMaterial* buttonMat = new MMaterial();
			buttonMat->SetShader(buttonShader);
			buttonMat->SetIsInternal(true);
			buttonMat->SetColor(MColor::White, 0);
			buttonMat->SetColor(MColor::Red, 1);
			buttonMat->SetColor(MColor::Green, 2);
			buttonMat->SetColor(MColor::Blue, 3);
			buttonMat->SetPath("DefaultGUIButtonMaterial");
			buttonMat->SetUint(0, 0);
			_materialCache.AddAsset("DefaultGUIButtonMaterial", buttonMat);

			MMaterial* cubemapMat = new MMaterial();
			cubemapMat->SetShader(reinterpret_cast<renderer::shaders::ShaderObject*>(GetShader("ShaderSky")));
			cubemapMat->SetIsInternal(true);
			cubemapMat->SetTextureCube(*blankDiffCube, 0);
			cubemapMat->SetColor(MColor::White, 0);
			cubemapMat->SetFloat(1.0f, 0);
			cubemapMat->SetPath("SkyboxDefaultMaterial");
			cubemapMat->SetAlphaMode(MMaterial::AlphaMode::MODE_OPAQUE);
			cubemapMat->SetLitMode(MMaterial::LitMode::DEFAULT_LIT);
			_materialCache.AddAsset("SkyboxDefaultMaterial", cubemapMat);
		}

		void ResourceManager::Shutdown()
		{
			_meshCache.Shutdown();
			_skeletonCache.Shutdown();
			_animationClipCache.Shutdown();
			_shaderBytecodeCache.Shutdown();
			_tex1DCache.Shutdown();
			_tex2DCache.Shutdown();
			_fontCache.Shutdown();
			_texCubeCache.Shutdown();
			_materialCache.Shutdown();
			_audioCache.Shutdown();

			for (auto it = _shaders.GetBegin(); it.IsValid(); ++it)
			{
				(*it).GetValue()->Shutdown();
				delete (*it).GetValue();
			}

			_shaders.Clear();
			_shaders.Shutdown();

			_fbxManager->Destroy();
			_fbxManager = nullptr;

			auto error = FT_Done_FreeType(_ftLibrary);
			ME_ASSERT(!error, "Could not initialize FreeType library.");

			ilShutDown();
		}

		void ResourceManager::SaveAllAssets()
		{
			_meshCache.SaveAllAssets();
			_skeletonCache.SaveAllAssets();
			_animationClipCache.SaveAllAssets();
			_materialCache.SaveAllAssets();
			//_shaderBytecodeCache.SaveAllAssets();		// no need
			_tex1DCache.SaveAllAssets();
			_tex2DCache.SaveAllAssets();
			_texCubeCache.SaveAllAssets();
			_audioCache.SaveAllAssets();
		}

	}
}
