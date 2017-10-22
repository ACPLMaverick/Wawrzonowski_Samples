#include <conio.h>

#include "assetLibrary\MMesh.h"
#include "assetLibrary\MMaterial.h"
#include "assetLibrary\MTexture1D.h"
#include "assetLibrary\MTexture2D.h"
#include "assetLibrary\MTexture2DAnimated.h"
#include "assetLibrary\MTextureCube.h"
#include "assetLibrary\MAudio.h"
#include "assetLibrary\MFont.h"

#define STATIC_MESH_OPTION "-stm"
#define SKELETAL_MESH_OPTION "-skm"
#define MATERIAL_OPTION "-m"
#define TEXTURE_1D_OPTION "-t1d"
#define TEXTURE_2D_OPTION "-t"
#define TEXTURE_2D_ANIMATED_OPTION "-ta"
#define TEXTURE_CUBE_OPTION "-tc"
#define FONT_OPTION "-f"
#define AUDIO_OPTION "-a"

using namespace morphEngine::assetLibrary;

template <typename T> void LoadAsset(morphEngine::resourceManagement::fileSystem::FileSystem* fs,
	char* source, char* destination)
{
	morphEngine::resourceManagement::fileSystem::File& f = fs->OpenFileAbsolute(source, morphEngine::resourceManagement::fileSystem::FileAccessMode::READ);
	if (f.IsOpened())
	{
		T* asset = new T();
		if (asset->LoadFromFile(f))
		{
			morphEngine::resourceManagement::fileSystem::File& sf = fs->OpenFileAbsolute(destination, morphEngine::resourceManagement::fileSystem::FileAccessMode::WRITE);
			if (sf.IsOpened())
			{
				if (!asset->SaveToFile(sf))
				{
					printf("Error saving asset\n");
				}
				fs->CloseFile(sf);
			}
		}
		else
		{
			printf("Error loading asset\n");
		}
		delete asset;
		asset = 0;
	}
	else
	{
		printf("Cannot open source file\n");
	}
	fs->CloseFile(f);
}

int main(int argc, char *argv[])
{
	bool showHelp = true;
	if(argc >= 4)
	{
		showHelp = false;
		char* option = argv[1];
		char* source = argv[2];
		char* destination = argv[3];

		morphEngine::resourceManagement::fileSystem::FileSystem* fs = morphEngine::resourceManagement::fileSystem::FileSystem::GetFileSystemInstance();

		if(std::strcmp(option, STATIC_MESH_OPTION) == 0)
		{
			LoadAsset<MMesh>(fs, source, destination);
		}
		else if(std::strcmp(option, SKELETAL_MESH_OPTION) == 0)
		{
			printf("loading skeletal mesh...");
			printf("ERROR: Not implemented!");
		}
		else if(std::strcmp(option, MATERIAL_OPTION) == 0)
		{
			printf("loading material...");
			LoadAsset<MMaterial>(fs, source, destination);
		}
		else if (std::strcmp(option, TEXTURE_1D_OPTION) == 0)
		{
			printf("loading texture 2D...");
			LoadAsset<MTexture1D>(fs, source, destination);
		}
		else if(std::strcmp(option, TEXTURE_2D_OPTION) == 0)
		{
			printf("loading texture 2D...");
			LoadAsset<MTexture2D>(fs, source, destination);
		}
		else if (std::strcmp(option, TEXTURE_2D_ANIMATED_OPTION) == 0)
		{
			printf("loading texture 2D...");
			LoadAsset<MTexture2DAnimated>(fs, source, destination);
		}
		else if (std::strcmp(option, FONT_OPTION) == 0)
		{
			printf("loading texture 2D...");
			LoadAsset<MFont>(fs, source, destination);
		}
		else if(std::strcmp(option, TEXTURE_CUBE_OPTION) == 0)
		{
			printf("loading texture cube...");
			LoadAsset<MTextureCube>(fs, source, destination);
		}
		else if(std::strcmp(option, AUDIO_OPTION) == 0)
		{
			printf("loading audio clip...");
			LoadAsset<MAudio>(fs, source, destination);
		}
		else
		{
			showHelp = true;
		}

		if(!showHelp)
		{
			printf("\nProcessing completed! Press any key to exit program\n");
			_getch();
		}

		fs->Shutdown();
		delete fs;
		fs = 0;
	}

	if(showHelp)
	{
		if(!(argc >= 2 && argv[1] == "--help"))
		{
			printf("Error! Invalid arguments! See below!\n\n");
		}

		printf("USAGE:\n");
		printf("AssetProcessor.exe -opt sourcePath destinationPath\n");
		printf("sourcePath - rleative (or absolute) path to source file (.obj, .fbx, .tga, .dds, .wav etc.)\n");
		printf("destinationPath - rleative (or absolute) path to destination path (i.e. Assets\\asset1.masset or C:\\RawAssets\\asset2.masset). IMPORTANT: Notice that asset should have .masset extension!\n");
		printf("-opt may be one of the following:\n");
		printf("%s - use this to tell processor that you're willing to load static mesh\n", STATIC_MESH_OPTION);
		printf("%s - use this to tell processor that you're willing to load skeletal mesh\n", SKELETAL_MESH_OPTION);
		printf("%s - use this to tell processor that you're willing to load material\n", MATERIAL_OPTION);
		printf("%s - use this to tell processor that you're willing to load texture 1D\n", TEXTURE_1D_OPTION);
		printf("%s - use this to tell processor that you're willing to load texture 2D\n", TEXTURE_2D_OPTION);
		printf("%s - use this to tell processor that you're willing to load texture 2D Animated\n", TEXTURE_2D_ANIMATED_OPTION);
		printf("%s - use this to tell processor that you're willing to load texture cube\n", TEXTURE_CUBE_OPTION);
		printf("%s - use this to tell processor that you're willing to load audio clip\n", AUDIO_OPTION);
	}

	return 0;
}