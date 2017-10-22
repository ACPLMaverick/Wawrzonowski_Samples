#include "MTexture2D.h"
#include "renderer/RenderingManager.h"
#include "renderer/RenderTarget2D.h"
#include "renderer/shaders/ShaderMerge.h"
#include "resourceManagement/ResourceManager.h"

namespace morphEngine
{
	using namespace renderer;
	using namespace renderer::shaders;
	using namespace renderer::device;

	namespace assetLibrary
	{

		MTexture2D::~MTexture2D()
		{
			Shutdown();
		}

		void MTexture2D::Merge(const MTexture2D * other, renderer::RenderTarget2D* output) const
		{
			ShaderMerge* shader = reinterpret_cast<ShaderMerge*>(resourceManagement::ResourceManager::GetInstance()->GetShader("ShaderMerge"));
			ShaderMergeData sd;
			sd.TextureA = this;
			sd.TextureB = other;
			shader->Set();
			shader->SetLocal(sd);
			output->SetAsOutput();
			RenderingManager::GetInstance()->GetDevice()->ClearBoundVertexIndexBuffers();
			RenderingManager::GetInstance()->GetDevice()->Draw(3);
		}

		void MTexture2D::InitializeFromRawData()
		{
			_res = RenderingManager::GetInstance()->GetDevice()->CreateTexture2DInstance();
			_res->Initialize(_rawData.GetDataPointer(), _width, _height, _bpp, _dataFormat, _samplerFormat, _bCompressed, _mipmapCount);

			if (_bReadOnly)
			{
				_rawData.Deallocate();
			}
		}

		void MTexture2D::Shutdown()
		{
			MTexture::Shutdown();

			if (_res != nullptr)
			{
				_res->Shutdown();
				delete _res;
			}
		}

		bool MTexture2D::SaveMAssetInternal(resourceManagement::fileSystem::File & file, 
			const utility::MFixedArray<MUint8>& data,
			renderer::device::GraphicDataFormat savedFormat, MUint8 savedChannels, MUint8 savedMips) const
		{
			file.Write(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

			InternalHeader hdr;
			hdr._totalDataSize = data.GetSize();
			hdr._samplerFormat = _samplerFormat;
			hdr._dataFormat = savedFormat;
			hdr._width = _width;
			hdr._height = _height;
			hdr._bpp = savedChannels;
			hdr._mipmapCount = savedMips;
			hdr._bReadOnly = _bReadOnly;

			file.WriteAppend(reinterpret_cast<const MUint8*>(&hdr), sizeof(InternalHeader));

			file.WriteAppend(data.GetDataPointer(), data.GetSize());

			return true;
		}

	}
}