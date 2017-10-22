#pragma once
#include "core/Singleton.h"
#include "core/GlobalDefines.h"
#include "utility/MString.h"
#include "assetLibrary/MMesh.h"
#include "assetLibrary/MSkeleton.h"
#include "assetLibrary/MAnimationClip.h"
#include "assetLibrary/MMaterial.h"
#include "assetLibrary/MShaderBytecode.h"
#include "assetLibrary/MTexture1D.h"
#include "assetLibrary/MTexture2D.h"
#include "assetLibrary/MFont.h"
#include "assetLibrary/MTextureCube.h"
#include "assetLibrary/MAudio.h"
#include "renderer/shaders/Shader.h"
#include "renderer/meshes/MeshPlane.h"
#include "renderer/meshes/MeshBox.h"
#include "utility/MDictionary.h"
#include "core/Engine.h"

namespace fbxsdk
{
	class FbxManager;
}

#include <ft2build.h>
#include FT_FREETYPE_H

namespace morphEngine
{
	namespace resourceManagement
	{
		class ResourceManager;

		//Template class for caching assets (like TextureCache, AudioClipCache etc)
		template<class T>
		class AssetCache
		{
			friend class ResourceManager;
		protected:
			utility::MDictionary<utility::MString, T*> _cache;

		public:
			inline AssetCache() { }
			virtual inline ~AssetCache() { }

		public:

			inline void AddAsset(const utility::MString& name, T* assetPtr)
			{
				_cache.Add(name, assetPtr);
			}

			//Returns asset with given path (tries to load if asset was not load yet)
			//Returns nullptr if asset cannot be loaded (file does not exist or whatever)
			inline T* GetAsset(const utility::MString& path)
			{
				if(_cache.Contains(path))
				{
					return _cache[path];
				}
				else
				{
					fileSystem::FileSystem& fs = core::Engine::GetInstance()->GetFileSystem();
					const char* p = path;
					fileSystem::File& file = fs.OpenFile(path, fileSystem::FileAccessMode::READ);
					T* newAsset = new T();
					if(!newAsset->LoadFromFile(file))
					{
						fs.CloseFile(file);
						delete newAsset;
						newAsset = 0;
						return nullptr;
					}
					fs.CloseFile(file);
					_cache.Add(path, newAsset);
					newAsset->SetPath(path);	// .
					newAsset->Initialize();
					return _cache[path];
				}
			}

			inline T* CreateAssetInstance(const utility::MString& path)
			{
				T* original(nullptr);
				if ((original = GetAsset(path)) == nullptr)
				{
					return nullptr;
				}

				return original->CreateInstance<T>();
			}

			inline void DestroyAssetInstance(T* assetPtr)
			{
				delete assetPtr;
			}

			inline bool GetPathOf(T* assetPtr, utility::MString& outPath)
			{
				return _cache.TryGetKey(assetPtr, &outPath);
			}

			//Cleans assets collection
			inline void Shutdown()
			{
				utility::MDictionary<utility::MString, T*>::MIteratorDictionary it = _cache.GetBegin();
				for(it; it.IsValid(); ++it)
				{
					delete (*it).GetValue();
				}
				_cache.Shutdown();
			}

			//Saves all assets to corresponding files
			inline void SaveAllAssets()
			{
				fileSystem::FileSystem& fs = core::Engine::GetInstance()->GetFileSystem();
				utility::MDictionary<utility::MString, T*>::MIteratorDictionary it = _cache.GetBegin();
				for (it; it.IsValid(); ++it)
				{
					if ((*it).GetValue()->GetIsInternal())
						continue;

					utility::MString path = (*it).GetKey();
					
					if (!path.EndsWithNCS(".masset"))
					{
						utility::MArray<utility::MString> splitPath;
						utility::MArray<char> toSplit;
						toSplit.Add('.');
						utility::MString::Split(path, toSplit, splitPath);
						path = splitPath[0] + ".masset";
					}

					fileSystem::File file = fs.OpenFile(path, fileSystem::FileAccessMode::WRITE);
					if (file.IsOpened())
					{
						(*it).GetValue()->SaveToFile(file);
						fs.CloseFile(file);
					}
					else
					{
						ME_WARNING(false, "File for saving is invalid. This shouldn't happen.");
					}
				}
			}
		};

		//Singleton class for managing AssetCaches
		//Stores references to all required AssetCaches
		//Allows to get asset by path
		class ResourceManager : public core::Singleton<ResourceManager>
		{
			friend class core::Singleton<ResourceManager>;
			friend class assetLibrary::MMesh;
		protected:
			AssetCache<assetLibrary::MMesh> _meshCache;
			AssetCache<assetLibrary::MSkeleton> _skeletonCache;
			AssetCache<assetLibrary::MAnimationClip> _animationClipCache;
			AssetCache<assetLibrary::MMaterial> _materialCache;
			AssetCache<assetLibrary::MShaderBytecode> _shaderBytecodeCache;
			AssetCache<assetLibrary::MTexture1D> _tex1DCache;
			AssetCache<assetLibrary::MTexture2D> _tex2DCache;
			AssetCache<assetLibrary::MFont> _fontCache;
			AssetCache<assetLibrary::MTextureCube> _texCubeCache;
			AssetCache<assetLibrary::MAudio> _audioCache;

			utility::MDictionary<utility::MString, renderer::shaders::Shader*> _shaders;

			fbxsdk::FbxManager* _fbxManager = nullptr;
			FT_Library _ftLibrary;

		protected:
			inline ResourceManager() { }

		public:

#pragma region Const

			const utility::MString DEFAULT_FONT_PATH = "Fonts/arial.masset";
			const MUint64 GUI_SHADER_PARAM_IMAGE = 0xFFFFFFFF00000000;
			const MUint64 GUI_SHADER_PARAM_TEXT = 0xFFFFFFFF00000004;

#pragma endregion


			inline fbxsdk::FbxManager* GetFbxManager() { return _fbxManager; }
			inline FT_Library GetFreeTypeLibrary() { return _ftLibrary; }

			//Returns MMesh at given path (or nullptr if path is invalid)
			inline assetLibrary::MMesh* GetMesh(const utility::MString& path)
			{
				return _meshCache.GetAsset(path);
			}

			inline assetLibrary::MMesh* CreateMeshInstance(const utility::MString& path)
			{
				return _meshCache.CreateAssetInstance(path);
			}

			inline void DestroyMeshInstance(assetLibrary::MMesh* asset)
			{
				_meshCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfMesh(assetLibrary::MMesh* mesh, utility::MString& outPath)
			{
				return _meshCache.GetPathOf(mesh, outPath);
			}

			inline assetLibrary::MSkeleton* GetSkeleton(const utility::MString& path)
			{
				return _skeletonCache.GetAsset(path);
			}

			inline assetLibrary::MSkeleton* CreateSkeletonInstance(const utility::MString& path)
			{
				return _skeletonCache.CreateAssetInstance(path);
			}

			inline void DestroySkeletonInstance(assetLibrary::MSkeleton* asset)
			{
				_skeletonCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfSkeleton(assetLibrary::MSkeleton* skeleton, utility::MString& outPath)
			{
				return _skeletonCache.GetPathOf(skeleton, outPath);
			}

			inline assetLibrary::MAnimationClip* GetAnimationClip(const utility::MString& path)
			{
				return _animationClipCache.GetAsset(path);
			}

			inline assetLibrary::MAnimationClip* CreateAnimationClipInstance(const utility::MString& path)
			{
				return _animationClipCache.CreateAssetInstance(path);
			}

			inline void DestroyAnimationClipInstance(assetLibrary::MAnimationClip* asset)
			{
				_animationClipCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfAnimationClip(assetLibrary::MAnimationClip* clip, utility::MString& outPath)
			{
				return _animationClipCache.GetPathOf(clip, outPath);
			}

			inline assetLibrary::MMaterial* GetMaterial(const utility::MString& path)
			{
				return _materialCache.GetAsset(path);
			}

			inline assetLibrary::MMaterial* CreateMaterialInstance(const utility::MString& path)
			{
				return _materialCache.CreateAssetInstance(path);
			}

			inline void DestroyMaterialInstance(assetLibrary::MMaterial* asset)
			{
				_materialCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfMaterial(assetLibrary::MMaterial* mat, utility::MString& outPath)
			{
				return _materialCache.GetPathOf(mat, outPath);
			}

			inline assetLibrary::MShaderBytecode* GetShaderBytecode(const utility::MString& path)
			{
				return _shaderBytecodeCache.GetAsset(path);
			}

			inline assetLibrary::MShaderBytecode* CreateShaderBytecodeInstance(const utility::MString& path)
			{
				return _shaderBytecodeCache.CreateAssetInstance(path);
			}

			inline void DestroyShaderBytecodeInstance(assetLibrary::MShaderBytecode* asset)
			{
				_shaderBytecodeCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfShaderBytecode(assetLibrary::MShaderBytecode* sb, utility::MString& outPath)
			{
				return _shaderBytecodeCache.GetPathOf(sb, outPath);
			}

			inline assetLibrary::MAudio* GetAudio(const utility::MString& path)
			{
				return _audioCache.GetAsset(path);
			}

			inline assetLibrary::MAudio* CreateAudioInstance(const utility::MString& path)
			{
				return _audioCache.CreateAssetInstance(path);
			}

			inline void DestroyAudioInstance(assetLibrary::MAudio* asset)
			{
				_audioCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfAudio(assetLibrary::MAudio* audio, utility::MString& outPath)
			{
				return _audioCache.GetPathOf(audio, outPath);
			}

			// Shaders are received by type name (i.e. ShaderLegacy), not by path.
			inline renderer::shaders::Shader* GetShader(const utility::MString& name)
			{
				renderer::shaders::Shader* outShader;
				if (_shaders.TryGetValue(name, &outShader))
				{
					return outShader;
				}
				else
				{
					outShader = renderer::shaders::Shader::CreateShaderByName(name);
					outShader->Initialize();
					_shaders.Add(name, outShader);
					return outShader;
				}
			}

			template <typename T> inline T* GetShader(const utility::MString& name)
			{
				return reinterpret_cast<T*>(GetShader(name));
			}

			inline bool GetPathOfShader(renderer::shaders::Shader* shader, utility::MString& outPath)
			{
				return _shaders.TryGetKey(shader, &outPath);
			}

			inline assetLibrary::MTexture1D* GetTexture1D(const utility::MString& path)
			{
				return _tex1DCache.GetAsset(path);
			}

			inline assetLibrary::MTexture1D* CreateTexture1DInstance(const utility::MString& path)
			{
				return _tex1DCache.CreateAssetInstance(path);
			}

			inline void DestroyTexture1DInstance(assetLibrary::MTexture1D* asset)
			{
				_tex1DCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfTexture1D(assetLibrary::MTexture1D* tex, utility::MString& outPath)
			{
				return _tex1DCache.GetPathOf(tex, outPath);
			}

			inline assetLibrary::MTexture2D* GetTexture2D(const utility::MString& path)
			{
				return _tex2DCache.GetAsset(path);
			}

			inline assetLibrary::MTexture2D* CreateTexture2DInstance(const utility::MString& path)
			{
				return _tex2DCache.CreateAssetInstance(path);
			}

			inline void DestroyTexture2DInstance(assetLibrary::MTexture2D* asset)
			{
				_tex2DCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfTexture2D(assetLibrary::MTexture2D* tex, utility::MString& outPath)
			{
				return _tex2DCache.GetPathOf(tex, outPath);
			}

			inline assetLibrary::MFont* GetFont(const utility::MString& path)
			{
				return _fontCache.GetAsset(path);
			}

			inline assetLibrary::MFont* CreateFontInstance(const utility::MString& path)
			{
				return _fontCache.CreateAssetInstance(path);
			}

			inline void DestroyFontInstance(assetLibrary::MFont* asset)
			{
				_fontCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfFont(assetLibrary::MFont* tex, utility::MString& outPath)
			{
				return _fontCache.GetPathOf(tex, outPath);
			}

			inline assetLibrary::MTextureCube* GetTextureCube(const utility::MString& path)
			{
				return _texCubeCache.GetAsset(path);
			}

			inline assetLibrary::MTextureCube* CreateTextureCubeInstance(const utility::MString& path)
			{
				return _texCubeCache.CreateAssetInstance(path);
			}

			inline void DestroyTextureCubeInstance(assetLibrary::MTextureCube* asset)
			{
				_texCubeCache.DestroyAssetInstance(asset);
			}

			inline bool GetPathOfTextureCube(assetLibrary::MTextureCube* tex, utility::MString& outPath)
			{
				return _texCubeCache.GetPathOf(tex, outPath);
			}

			// Initializes caches with system-built assets.
			// Also initializes any necessary asset libraries.
			void Initialize();

			//Shutdowns all AssetCaches
			//Also shuts down any used asset libraries.
			void Shutdown();

			// Saves all existing assets
			void SaveAllAssets();
		};
	}
}