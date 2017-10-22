#include "MShaderBytecode.h"
#include "renderer/RenderingManager.h"

#include "core\Engine.h"
#include "resourceManagement\fileSystem\FileSystem.h"

using namespace morphEngine::renderer;
using namespace morphEngine::renderer::device;
using namespace morphEngine::utility;
using namespace morphEngine::resourceManagement::fileSystem;

namespace morphEngine
{
	namespace assetLibrary
	{
		bool MShaderBytecode::LoadFromData(const MFixedArray<MUint8>& data)
		{
			_shaderBytecode = data;
			return true;
		}
		bool MShaderBytecode::LoadFromData(const MArray<MUint8>& data)
		{
			_shaderBytecode = data;
			return true;
		}
		bool MShaderBytecode::LoadFromFile(File & file)
		{
			SetPath(file.GetPath());
			MSize size = file.GetSize();
			_shaderBytecode.Allocate(size);
			file.Read(_shaderBytecode);
			return true;
		}
		bool MShaderBytecode::SaveToFile(File & file) const
		{
			return false;
		}

		void MShaderBytecode::Shutdown()
		{
			_shaderBytecode.Deallocate();
		}
	}
}