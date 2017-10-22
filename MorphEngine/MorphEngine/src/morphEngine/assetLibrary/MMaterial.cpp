#include "MMaterial.h"
#include "renderer/shaders/ShaderObject.h"
#include "MMesh.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace renderer::shaders;
	using namespace resourceManagement;
	using namespace utility;
	using namespace animation;

	namespace assetLibrary
	{
		void MMaterial::SetPass(const gom::Camera& camera, MUint8 passIndex) const
		{
			ShaderGlobalData gData;
			gData.Camera = &camera;
			UpdateSetGlobalStatistics();
			_shader->SetGlobal(gData, passIndex);
		}

		void MMaterial::SetPassDeferred(const gom::Camera & camera, MUint8 passIndex) const
		{
			ShaderGlobalData gData;
			gData.Camera = &camera;
			UpdateSetGlobalStatistics();
			_shader->SetGlobalDeferred(gData, passIndex);
		}

		void MMaterial::SetPassSkinned(const gom::Camera & camera, MUint8 passIndex) const
		{
			ShaderGlobalData gData;
			gData.Camera = &camera;
			UpdateSetGlobalStatistics();
			_shader->SetGlobalSkinned(gData, passIndex);
		}

		void MMaterial::SetPassSkinnedDeferred(const gom::Camera & camera, MUint8 passIndex) const
		{
			ShaderGlobalData gData;
			gData.Camera = &camera;
			UpdateSetGlobalStatistics();
			_shader->SetGlobalSkinnedDeferred(gData, passIndex);
		}

		void MMaterial::UnsetPass(MUint8 passIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			_shader->UnsetGlobal();
		}

		void MMaterial::SetData(MUint8 passIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			UpdateSetMaterialStatistics();
			_shader->SetMaterialWise(*this);
		}

		void MMaterial::SetDataDeferred(MUint8 passIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			UpdateSetMaterialStatistics();
			_shader->SetMaterialWiseDeferred(*this);
		}

		void MMaterial::SetDataSkinned(MUint8 passIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			UpdateSetMaterialStatistics();
			_shader->SetMaterialWiseSkinned(*this);
		}

		void MMaterial::SetDataSkinnedDeferred(MUint8 passIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			UpdateSetMaterialStatistics();
			_shader->SetMaterialWiseSkinnedDeferred(*this);
		}

		void MMaterial::DrawMesh(const gom::Camera & camera, const gom::Transform & transform, const MMesh & mesh, MSize submeshIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			_shader->SetLocal(*this, camera, transform, mesh);
			MSize indexCount = mesh.GetIndexCount(submeshIndex);
			UpdateDrawCallStatistics(mesh.GetVertexCount(submeshIndex), indexCount);
			renderer::RenderingManager::GetInstance()->GetDevice()->Draw(static_cast<MUint32>(indexCount), static_cast<MUint32>(mesh.GetStartIndex(submeshIndex)), static_cast<MUint32>(mesh.GetStartVertex()));
		}

		void MMaterial::DrawMeshSkinned(const gom::Camera & camera, const gom::Transform & transform, const MMesh & mesh, const AnimationFrame& frame, MSize submeshIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			_shader->SetLocalSkinned(*this, camera, transform, mesh, frame);
			MSize indexCount = mesh.GetIndexCount(submeshIndex);
			UpdateDrawCallStatistics(mesh.GetVertexCount(submeshIndex), indexCount);
			renderer::RenderingManager::GetInstance()->GetDevice()->Draw(static_cast<MUint32>(indexCount), static_cast<MUint32>(mesh.GetStartIndex(submeshIndex)), static_cast<MUint32>(mesh.GetStartVertex()));
		}

		void MMaterial::DrawMeshDeferred(const gom::Camera & camera, const gom::Transform & transform, const MMesh & mesh, MSize submeshIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			_shader->SetLocalDeferred(*this, camera, transform, mesh);
			MSize indexCount = mesh.GetIndexCount(submeshIndex);
			UpdateDrawCallStatistics(mesh.GetVertexCount(submeshIndex), indexCount);
			renderer::RenderingManager::GetInstance()->GetDevice()->Draw(static_cast<MUint32>(indexCount), static_cast<MUint32>(mesh.GetStartIndex(submeshIndex)), static_cast<MUint32>(mesh.GetStartVertex()));
		}

		void MMaterial::DrawMeshSkinnedDeferred(const gom::Camera & camera, const gom::Transform & transform, const MMesh & mesh, const AnimationFrame& frame, MSize submeshIndex) const
		{
			ME_ASSERT(_shader != nullptr, "Material: shader is null.");
			_shader->SetLocalSkinnedDeferred(*this, camera, transform, mesh, frame);
			MSize indexCount = mesh.GetIndexCount(submeshIndex);
			UpdateDrawCallStatistics(mesh.GetVertexCount(submeshIndex), indexCount);
			renderer::RenderingManager::GetInstance()->GetDevice()->Draw(static_cast<MUint32>(indexCount), static_cast<MUint32>(mesh.GetStartIndex(submeshIndex)), static_cast<MUint32>(mesh.GetStartVertex()));
		}

		void MMaterial::SetShader(renderer::shaders::ShaderObject * shd)
		{
			_shader = shd;
			ClearInterface();
			_shader->SetupMaterialInterface(*this);
		}

		bool MMaterial::LoadFromData(const utility::MFixedArray<MUint8>& data)
		{
			if (!CheckType(data.GetDataPointer()))
				return false;

			MSize offset = sizeof(MAssetType);
			if (offset > data.GetSize()) return false;

			InternalHeader* header = reinterpret_cast<InternalHeader*>(data.GetDataPointer() + offset);
			
			offset += sizeof(InternalHeader);

			MString shaderName("");
			for (MSize i = 0; i < header->ShaderNameLength; ++i)
			{
				char addition = *(data.GetDataPointer() + offset);
				shaderName += addition;
				offset += sizeof(char);
			}

			ShaderObject* shader = reinterpret_cast<ShaderObject*>(ResourceManager::GetInstance()->GetShader(shaderName));
			if (shader != nullptr)
			{
				SetShader(shader);
			}
			else
			{
				ME_WARNING(false, "Could not load shader declared in material.");
				return false;
			}

			// loading data

			for (MSize i = 0; i < header->SizeMatrix; ++i)
			{
				_paramsMatrix[i] = *(reinterpret_cast<MMatrix*>(data.GetDataPointer() + offset));

				offset += sizeof(MMatrix);
			}

			for (MSize i = 0; i < header->SizeColor; ++i)
			{
				MColor col = *(reinterpret_cast<MColor*>(data.GetDataPointer() + offset));
				_paramsColor[i] = col;

				offset += sizeof(MColor);
			}

			for (MSize i = 0; i < header->SizeVector3; ++i)
			{
				_paramsVector3[i] = *(reinterpret_cast<MVector3*>(data.GetDataPointer() + offset));

				offset += sizeof(MVector3);
			}

			for (MSize i = 0; i < header->SizeVector2; ++i)
			{
				_paramsVector2[i] = *(reinterpret_cast<MVector2*>(data.GetDataPointer() + offset));

				offset += sizeof(MVector2);
			}

			for (MSize i = 0; i < header->SizeUint; ++i)
			{
				_paramsUint[i] = *(reinterpret_cast<MUint64*>(data.GetDataPointer() + offset));

				offset += sizeof(MUint64);
			}

			for (MSize i = 0; i < header->SizeFloat; ++i)
			{
				_paramsFloat[i] = *(reinterpret_cast<MFloat32*>(data.GetDataPointer() + offset));

				offset += sizeof(MFloat32);
			}

			////

			auto readString = [](MUint8* dataPtrBase, MString& outString, MSize& inoutOffset)
			{
				MSize length;
				memcpy(&length, dataPtrBase + inoutOffset, sizeof(MSize));
				inoutOffset += sizeof(MSize);

				outString = "";
				for (MSize i = 0; i < length; ++i)
				{
					char addition = *(dataPtrBase + inoutOffset);
					outString += addition;
					inoutOffset += sizeof(char);
				}
			};


			for (MSize i = 0; i < header->SizeTexture1D; ++i)
			{
				MString path;
				readString(data.GetDataPointer(), path, offset);
				_paramsTexture1D[i] = ResourceManager::GetInstance()->GetTexture1D(path);
			}

			for (MSize i = 0; i < header->SizeTexture2D; ++i)
			{
				MString path;
				readString(data.GetDataPointer(), path, offset);
				debugging::Debug::Log(debugging::ELogType::LOG, path);
				_paramsTexture2D[i] = ResourceManager::GetInstance()->GetTexture2D(path);
			}

			for (MSize i = 0; i < header->SizeTextureCube; ++i)
			{
				MString path;
				readString(data.GetDataPointer(), path, offset);
				_paramsTextureCube[i] = ResourceManager::GetInstance()->GetTextureCube(path);
			}

			ME_ASSERT_S(offset <= data.GetSize());

			return true;
		}
		bool MMaterial::LoadFromData(const utility::MArray<MUint8>& data)
		{
			MFixedArray<MUint8> tempData(data.GetSize());
			memcpy(tempData.GetDataPointer(), data.GetDataPointer(), data.GetSize());
			return LoadFromData(tempData);
		}
		bool MMaterial::LoadFromFile(resourceManagement::fileSystem::File & file)
		{
			SetPath(file.GetPath());
			MFixedArray<MUint8> tempData(file.GetSize());
			file.Read(tempData.GetDataPointer(), 0, file.GetSize());
			return LoadFromData(tempData);
		}

		bool MMaterial::SaveToFile(resourceManagement::fileSystem::File & file) const
		{
			file.WriteAppend(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

			MString shaderName;
			bool res = ResourceManager::GetInstance()->GetPathOfShader(_shader, shaderName);
			ME_ASSERT_S(res);

			InternalHeader ih;
			ih.SizeMatrix = _paramsMatrix.GetSize();
			ih.SizeColor = _paramsColor.GetSize();
			ih.SizeVector3 = _paramsVector3.GetSize();
			ih.SizeVector2 = _paramsVector2.GetSize();
			ih.SizeUint = _paramsUint.GetSize();
			ih.SizeFloat = _paramsFloat.GetSize();
			ih.SizeTexture1D = _paramsTexture1D.GetSize();
			ih.SizeTexture2D = _paramsTexture2D.GetSize();
			ih.SizeTextureCube = _paramsTextureCube.GetSize();
			ih.ShaderNameLength = shaderName.Length();

			file.WriteAppend(reinterpret_cast<const MUint8*>(&ih), sizeof(InternalHeader));

			file.WriteAppend(reinterpret_cast<const MUint8*>(shaderName.Data()), sizeof(char) * ih.ShaderNameLength);

			if(_paramsMatrix.GetSize() != 0) file.WriteAppend(reinterpret_cast<const MUint8*>(_paramsMatrix.GetDataPointer()), sizeof(MMatrix) * _paramsMatrix.GetSize());
			if (_paramsColor.GetSize() != 0) file.WriteAppend(reinterpret_cast<const MUint8*>(_paramsColor.GetDataPointer()), sizeof(MColor) * _paramsColor.GetSize());
			if (_paramsVector3.GetSize() != 0) file.WriteAppend(reinterpret_cast<const MUint8*>(_paramsVector3.GetDataPointer()), sizeof(MVector3) * _paramsVector3.GetSize());
			if (_paramsVector2.GetSize() != 0) file.WriteAppend(reinterpret_cast<const MUint8*>(_paramsVector2.GetDataPointer()), sizeof(MVector2) * _paramsVector2.GetSize());
			if (_paramsUint.GetSize() != 0) file.WriteAppend(reinterpret_cast<const MUint8*>(_paramsUint.GetDataPointer()), sizeof(MUint64) * _paramsUint.GetSize());
			if (_paramsFloat.GetSize() != 0) file.WriteAppend(reinterpret_cast<const MUint8*>(_paramsFloat.GetDataPointer()), sizeof(MFloat32) * _paramsFloat.GetSize());

			auto writeString = [](resourceManagement::fileSystem::File & file, const MString& string)
			{
				MSize length = string.Length();
				file.WriteAppend(reinterpret_cast<MUint8*>(&length), sizeof(MSize));
				file.WriteAppend(reinterpret_cast<const MUint8*>(string.Data()), sizeof(char) * length);
			};

			for (MSize i = 0; i < _paramsTexture1D.GetSize(); ++i)
			{
				MString path;
				bool res = ResourceManager::GetInstance()->GetPathOfTexture1D(_paramsTexture1D[i], path);
				ME_ASSERT_S(res);
				writeString(file, path);
			}

			for (MSize i = 0; i < _paramsTexture2D.GetSize(); ++i)
			{
				MString path;
				bool res = ResourceManager::GetInstance()->GetPathOfTexture2D(_paramsTexture2D[i], path);
				ME_ASSERT_S(res);
				writeString(file, path);
			}

			for (MSize i = 0; i < _paramsTextureCube.GetSize(); ++i)
			{
				MString path;
				bool res = ResourceManager::GetInstance()->GetPathOfTextureCube(_paramsTextureCube[i], path);
				ME_ASSERT_S(res);
				writeString(file, path);
			}

			return true;
		}

		inline void MMaterial::ClearInterface()
		{
			_paramsMatrix.Clear();
			_paramsColor.Clear();
			_paramsVector3.Clear();
			_paramsVector2.Clear();
			_paramsUint.Clear();
			_paramsFloat.Clear();
			_paramsTexture1D.Clear();
			_paramsTexture2D.Clear();
			_paramsTextureCube.Clear();
		}

		inline void MMaterial::UpdateSetGlobalStatistics() const
		{
#if _DEBUG
			renderer::RenderingManager::GetInstance()->AddSetGlobalCall();
#endif
		}

		inline void MMaterial::UpdateSetMaterialStatistics() const
		{
#if _DEBUG
			renderer::RenderingManager::GetInstance()->AddSetMaterialCall();
#endif
		}

		inline void MMaterial::UpdateDrawCallStatistics(MSize vertices, MSize indices) const
		{
#if _DEBUG
			renderer::RenderingManager::GetInstance()->AddDrawCall(vertices, indices);
#endif
		}

		MTexture1D * MMaterial::GetDefaultTexture1D()
		{
			return ResourceManager::GetInstance()->GetTexture1D("BlankDiffuse1D");
		}

		MTexture2D * MMaterial::GetDefaultTexture2D()
		{
			return ResourceManager::GetInstance()->GetTexture2D("BlankDiffuse");
		}

		MTextureCube * MMaterial::GetDefaultTextureCube()
		{
			return ResourceManager::GetInstance()->GetTextureCube("BlankDiffuseCube");
		}

		renderer::shaders::ShaderObject * MMaterial::GetDefaultShader()
		{
			return reinterpret_cast<renderer::shaders::ShaderObject*>(ResourceManager::GetInstance()->GetShader("ShaderBlank"));
		}

		void MMaterial::Shutdown()
		{
			ClearInterface();
			_shader = GetDefaultShader();
		}

	}
}