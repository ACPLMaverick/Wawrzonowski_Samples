#pragma once

#include "core/GlobalDefines.h"
#include "utility/MString.h"
#include "utility/MMatrix.h"
#include "utility/MVector.h"
#include "renderer/device/GraphicsDevice.h"

#ifdef PLATFORM_WINDOWS

#define SHADER_PATH_PREFIX "Shaders/"
#define SHADER_PATH_MIDDLEFIX "/DX11/"
#define SHADER_PATH_SUFFIX_VS "_VS.cso"
#define SHADER_PATH_SUFFIX_PS "_PS.cso"
#define SHADER_PATH_SUFFIX_GS "_GS.cso"
#define SHADER_PATH_SUFFIX_HS "_HS.cso"
#define SHADER_PATH_SUFFIX_DS "_DS.cso"
#define SHADER_PATH_SUFFIX_CS "_CS.cso"
#define SHADER_PATH_SUFFIX_DEFERRED "_D"

#endif

namespace morphEngine
{
	namespace gom
	{
		class Transform;
		class Camera;
	}

	namespace assetLibrary
	{
		class MMaterial;
		class MMesh;
	}

	namespace resourceManagement
	{
		class ResourceManager;
	}

	namespace renderer
	{
		namespace shaders
		{
			struct ShaderGlobalData
			{
				const gom::Camera* Camera;
			};

			struct ShaderLocalData
			{
			};

			/// <summary>
			/// This class stores and represents whole pipeline and all data for a given shader.
			/// It is responsible for loading shader bytecodes, acquiring resources from GPU, creating and setting constant buffers and resources on GPU
			/// and creating an interface for a material it is assigned to.
			/// This is an abstract class and has to be derived from for each supported shader.
			/// Global data means data set ONCE PER FRAME, such as SHADER RESOURCES, camera position or fog parameters.
			/// Local data is data per object or per one draw call (of many using the same shader). You cannot set local data before setting global data for given shader.
			/// </summary>
			class Shader
			{
				friend class resourceManagement::ResourceManager;

			public:

				static const char* TYPE_NAME;

			protected:

#pragma region Structs Protected

				__declspec(align(16))
					struct BufferDataGlobal
				{
					utility::MVector3 CameraPosition;
				};

#pragma endregion

#pragma region Functions Protected

				void SetupGlobalCBuffer(const ShaderGlobalData& globalData, device::ConstantBufferResource* cb, MUint32 slot) const;

#pragma endregion

			public:

#pragma region Functions Public

				Shader();
				virtual ~Shader();

				virtual void Initialize() = 0;
				virtual void Shutdown() = 0;

				// Simply sets shader resources, without setting any additional data.
				virtual void Set(MUint8 passIndex = 0) const = 0;
				// Sets shader and all its resources from custom data. Most versatile function, though least performant.
				virtual void Set(const ShaderGlobalData& globalData, const ShaderLocalData& localData, MUint8 passIndex = 0) const = 0;
				virtual void SetGlobal(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const = 0;
				// If shader does not support deferred rendering, this call is the same as SetGlobal.
				virtual void SetGlobalDeferred(const ShaderGlobalData& globalData, MUint8 passIndex = 0) const { SetGlobal(globalData, passIndex); }

				virtual void UnsetGlobal() const = 0;

#pragma region Static

				// This function is temporary, until a better solution is found.
				static Shader* CreateShaderByName(const utility::MString& name);

				static void BuildShaderPath(const char* typeName, const char* suffix, utility::MString& outPath, bool deferred = false);

#pragma endregion

#pragma endregion
			};
		}
	}
}