#include "MSkeleton.h"

#include "core\Engine.h"
#include "resourceManagement\fileSystem\FileSystem.h"

namespace morphEngine
{
	using namespace animation;
	using namespace utility;

	namespace assetLibrary
	{
		MSkeleton::~MSkeleton()
		{
		}

		animation::Bone * MSkeleton::CreateBone(const utility::MString & name, utility::MMatrix& inverseBindpose, utility::MMatrix& worldTransform, animation::Bone * parent)
		{
			ME_ASSERT(_bones.GetSize() < BONES_PER_MESH, "Added too many bones to a skeleton (more than 96).");

			_bindPoseGlobalInverse.Add(inverseBindpose);
			_worldTransform.Add(worldTransform);
			Bone bone(name, _bones.GetSize(), &_bindPoseGlobalInverse[_bindPoseGlobalInverse.GetSize() - 1], &_worldTransform[_worldTransform.GetSize() - 1], parent);
			_bones.Add(bone);
			Bone* bonePtr = &_bones[_bones.GetSize() - 1];

			_bonesByName.Add(name, bonePtr);

			if (parent == nullptr)
			{
				ME_ASSERT_S(_root == nullptr);
				_root = bonePtr;
			}

			return bonePtr;
		}

		bool MSkeleton::LoadFromData(const utility::MFixedArray<MUint8>& data)
		{
			// read type
			if (!CheckType(data.GetDataPointer()))
				return false;

			// read bone count
			MSize boneCount;
			memcpy(&boneCount, data.GetDataPointer() + sizeof(MAssetType), sizeof(MSize));

			if (boneCount > BONES_PER_MESH)
			{
				ME_WARNING(true, "Loaded skeleton has more than 96 bones. This shouldn't be so.");
				return false;
			}

			MFixedArray<MSize> nameLengths(boneCount);
			MFixedArray<MString> names(boneCount);
			MFixedArray<MMatrix> matricesBindpose(boneCount);
			MFixedArray<MMatrix> matricesWorld(boneCount);
			MFixedArray<MSize> parentIndices(boneCount);
			MUint8* dataPtr = data.GetDataPointer();
			MSize offset = sizeof(MSize) + sizeof(MAssetType);

			// read name lengths
			for (MSize i = 0; i < boneCount; ++i)
			{
				nameLengths[i] = *reinterpret_cast<MSize*>(dataPtr + offset);
				offset += sizeof(MSize);
			}

			// read names
			for (MSize i = 0; i < boneCount; ++i)
			{
				MString name;
				for (MSize j = 0; j < nameLengths[i]; ++j)
				{
					char p = *reinterpret_cast<char*>(dataPtr + offset);
					MString addition(&p);
					name += p;
					offset += sizeof(char);
				}
				names[i] = name;
			}

			// read matrices bindpose
			for (MSize i = 0; i < boneCount; ++i)
			{
				matricesBindpose[i] = *reinterpret_cast<MMatrix*>(dataPtr + offset);
				offset += sizeof(MMatrix);
			}

			// read matrices world
			for (MSize i = 0; i < boneCount; ++i)
			{
				matricesWorld[i] = *reinterpret_cast<MMatrix*>(dataPtr + offset);
				offset += sizeof(MMatrix);
			}

			// read parent indices
			for (MSize i = 0; i < boneCount; ++i)
			{
				MSize p = *reinterpret_cast<MSize*>(dataPtr + offset);
				parentIndices[i] = p;
				offset += sizeof(MSize);
			}

			// create bones
			for (MSize i = 0; i < boneCount; ++i)
			{
				Bone* parent = parentIndices[i] != -1 ? &_bones[parentIndices[i]] : nullptr;
				CreateBone(names[i], matricesBindpose[i], matricesWorld[i], parent);
			}

			return true;
		}

		bool MSkeleton::LoadFromData(const utility::MArray<MUint8>& data)
		{
			return LoadFromData(reinterpret_cast<const utility::MFixedArray<MUint8>&>(data));
		}

		bool MSkeleton::LoadFromFile(resourceManagement::fileSystem::File & file)
		{
			if (!file.IsOpened())
				return false;

			MFixedArray<MUint8> data;
			data.Allocate(file.GetSize());
			file.Read(data.GetDataPointer(), 0, file.GetSize());

			return LoadFromData(data);
		}

		bool MSkeleton::SaveToFile(resourceManagement::fileSystem::File & file) const
		{
			file.WriteAppend(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

			// write bone count
			MSize boneCount = GetBoneCount();
			file.WriteAppend(reinterpret_cast<MUint8*>(&boneCount), sizeof(MSize));

			// write name lengths
			MFixedArray<MSize> nameLengths(boneCount);
			
			for (MSize i = 0; i < boneCount; ++i)
			{
				nameLengths[i] = _bones[i].GetName().Length();
			}
			file.WriteAppend(reinterpret_cast<MUint8*>(nameLengths.GetDataPointer()), sizeof(MSize) * boneCount);

			// write names
			for (MSize i = 0; i < boneCount; ++i)
			{
				Bone& bone = _bones[i];
				const MString& boneName = bone.GetName();
				const char* txt = boneName.Data();
				file.WriteAppend(reinterpret_cast<const MUint8*>(txt), sizeof(MUint8) * nameLengths[i]);
			}

			// write matrices bindpose
			for (MSize i = 0; i < boneCount; ++i)
			{
				MMatrix boneTransform = _bones[i].GetInverseBindpose();
				file.WriteAppend(reinterpret_cast<MUint8*>(&boneTransform), sizeof(MMatrix));
			}

			// write matrices world
			for (MSize i = 0; i < boneCount; ++i)
			{
				MMatrix boneTransform = _bones[i].GetWorldTransform();
				file.WriteAppend(reinterpret_cast<MUint8*>(&boneTransform), sizeof(MMatrix));
			}

			// write parent indices
			for (MSize i = 0; i < boneCount; ++i)
			{
				MSize parentIndex = _bones[i].GetParent() != nullptr ? _bones[i].GetParent()->GetFlatIndex() : -1;
				file.WriteAppend(reinterpret_cast<MUint8*>(&parentIndex), sizeof(MSize));
			}

			return true;
		}

		void MSkeleton::Initialize()
		{
		}

		void MSkeleton::Shutdown()
		{
			_worldTransform.Clear();
			_bindPoseGlobalInverse.Clear();
			_bones.Clear();
			_bonesByName.Clear();
		}

	}
}