#include "MAsset.h"
#include "core/Engine.h"
#include "resourceManagement/fileSystem/FileSystem.h"

using namespace morphEngine::resourceManagement;
using namespace morphEngine::resourceManagement::fileSystem;

bool morphEngine::assetLibrary::MAsset::Reload()
{
	Shutdown();
	File& file = core::Engine::GetInstance()->GetFileSystem().OpenFile(GetPath(), FileAccessMode::READ);

	if (!file.IsOpened())
		return false;

	bool success = LoadFromFile(file);

	if (!success)
		return false;

	Initialize();

	for (MSize i = 0; i < _instances.GetSize(); ++i)
	{
		_instances[i]->Reload();
	}

	return true;
}
