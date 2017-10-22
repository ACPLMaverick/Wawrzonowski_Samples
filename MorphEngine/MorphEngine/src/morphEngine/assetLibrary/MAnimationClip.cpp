#include "MAnimationClip.h"
#include "resourceManagement/ResourceManager.h"
#include "MMesh.h"
#include "MSkeleton.h"
#include "debugging/Debug.h"

#include "FbxHelperDefines.h"

namespace morphEngine
{
	using namespace utility;
	using namespace animation;
	using namespace resourceManagement;

	namespace assetLibrary
	{
		bool MAnimationClip::LoadFromData(const utility::MFixedArray<MUint8>& data)
		{
			// read type
			MSize offset = 0;
			MUint8* dataPtr = data.GetDataPointer();
			if (!CheckType(dataPtr))
				return false;

			// read frame count
			offset += sizeof(MAssetType);
			MSize frameCount;
			memcpy(&frameCount, dataPtr + offset, sizeof(MSize));
			offset += sizeof(MSize);

			// read one frame matrices size
			MSize oneFrameMatricesSize(0);
			memcpy(&oneFrameMatricesSize, dataPtr + offset, sizeof(MSize));
			offset += sizeof(MSize);

			// read length
			memcpy(&_length, dataPtr + offset, sizeof(MFloat32));
			offset += sizeof(MFloat32);

			// read skeleton name length
			MSize skeletonNameLength(0);
			memcpy(&skeletonNameLength, dataPtr + offset, sizeof(MSize));
			offset += sizeof(MSize);

			// read name length
			MSize nameLength(0);
			memcpy(&nameLength, dataPtr + offset, sizeof(MSize));
			offset += sizeof(MSize);

			// read skeleton name
			_skeletonPath = "";
			for (MSize i = 0; i < skeletonNameLength; ++i)
			{
				MString addition(reinterpret_cast<char*>(dataPtr + offset));
				_skeletonPath += addition;
				offset += sizeof(char);
			}

			// read name
			_name = "";
			for (MSize i = 0; i < nameLength; ++i)
			{
				MString addition(reinterpret_cast<char*>(dataPtr + offset));
				_name += addition;
				offset += sizeof(char);
			}

			// read frames

			for (MSize i = 0; i < frameCount; ++i)
			{
				MArray<MMatrix> mats(oneFrameMatricesSize);
				mats.Resize(oneFrameMatricesSize);
				memcpy(mats.GetDataPointer(), dataPtr + offset, sizeof(MMatrix) * oneFrameMatricesSize);
				offset += sizeof(MMatrix) * oneFrameMatricesSize;
				MFloat32 time;
				memcpy(&time, dataPtr + offset, sizeof(MFloat32));
				offset += sizeof(MFloat32);

				AnimationFrame fr;
				_frames.Add(fr);
				(_frames[_frames.GetSize() - 1]).Initialize(mats, time, nullptr);	// anims loaded from masset are already baked with skeleton's inverse bindpose
			}

			// gimme skeleton
			MSkeleton* skel = ResourceManager::GetInstance()->GetSkeleton(GetSkeletonPath());

			// way to go

			return true;
		}

		bool MAnimationClip::LoadFromData(const utility::MArray<MUint8>& data)
		{
			return LoadFromData(reinterpret_cast<const MFixedArray<MUint8>&>(data));
		}

		bool MAnimationClip::LoadFromFile(resourceManagement::fileSystem::File & file)
		{
			utility::MString ext;
			GetExtension(file, ext);

			if (ext == "masset")
			{
				SetPath(file.GetPath());
				return LoadMAsset(file);
			}
			else if (ext == "fbx")
			{
				SetPath(file.GetPath());
				return LoadFBX(file);
			}
			else return false;
		}

		bool MAnimationClip::SaveToFile(resourceManagement::fileSystem::File & file) const
		{
			if (_frames.GetSize() == 0)
				return false;

			file.WriteAppend(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

			// write frame count
			MSize frameCount = _frames.GetSize();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&frameCount), sizeof(MSize));

			// write one frame matrices size
			MSize oneFrameLength = _frames[0].GetData().GetSize();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&oneFrameLength), sizeof(MSize));

			// write length
			file.WriteAppend(reinterpret_cast<const MUint8*>(&_length), sizeof(MFloat32));

			// write skeleton path length
			MSize skeletonPathLength = _skeletonPath.Length();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&skeletonPathLength), sizeof(MSize));

			// write name length
			MSize nameLength = _name.Length();
			file.WriteAppend(reinterpret_cast<const MUint8*>(&nameLength), sizeof(MSize));

			// write skeleton path
			file.WriteAppend(reinterpret_cast<const MUint8*>(_skeletonPath.Data()), sizeof(char) * skeletonPathLength);

			// write name
			file.WriteAppend(reinterpret_cast<const MUint8*>(_name.Data()), sizeof(char) * nameLength);

			// write frames
			for (auto i = 0; i < frameCount; ++i)
			{
				ME_ASSERT(_frames[i].GetData().GetSize() == oneFrameLength, "Error saving animation clip - some frames have different matrices count!");
				file.WriteAppend(reinterpret_cast<const MUint8*>(_frames[i].GetData().GetDataPointer()), sizeof(MMatrix) * oneFrameLength);
				MFloat32 t = _frames[i].GetTime();
				file.WriteAppend(reinterpret_cast<const MUint8*>(&t), sizeof(MFloat32));
			}

			return true;
		}

		void MAnimationClip::Shutdown()
		{
			_name = "";
			_frames.Clear();
			_length = 0.0f;
			_skeletonPath = "";
		}

		inline bool MAnimationClip::LoadFBX(resourceManagement::fileSystem::File & file)
		{
			//MFixedArray<MUint8> bytes(file.GetSize());
			//file.Read(bytes.GetDataPointer(), 0, file.GetSize());
			//FbxManager* manager = ResourceManager::GetInstance()->GetFbxManager();
			//FbxScene* scene = LoadSceneFromBuffer(manager, bytes);

			//MMesh::IndexLookupTable lut;

			//for (MInt32 i = 0; i < scene->GetNodeCount(); ++i)
			//{
			//	if (MMesh::LoadFbxSkeletonAndAnimations_Intenral(nullptr, &lut, scene, mesh, scene->GetNode(i))) break;
			//}

			//return true;
			return false;
		}

		inline bool MAnimationClip::LoadMAsset(resourceManagement::fileSystem::File & file)
		{
			MFixedArray<MUint8> data;
			data.Allocate(file.GetSize());
			file.Read(data.GetDataPointer(), 0, file.GetSize());

			return LoadFromData(data);
		}

		void MAnimationClip::Initialize()
		{
		}
	}
}