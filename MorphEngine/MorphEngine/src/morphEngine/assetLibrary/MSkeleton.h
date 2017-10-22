#pragma once

#include "MAsset.h"
#include "utility/Collections.h"
#include "utility/MMatrix.h"
#include "animation/Bone.h"
#include "animation/Socket.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MMesh;

		/*
			Holds all bone transformation matrices in an array, as well as bone hierarchy in the form of a tree.
			Each bone holds bind pose transformation matrix, which is necessary to calculate animation data.
		*/
		class MSkeleton : public MAsset
		{
			friend class MMesh;
			friend class resourceManagement::ResourceManager;
		protected:

			static const int BONES_PER_MESH = 96;

			utility::MArray<utility::MMatrix> _bindPoseGlobalInverse;
			utility::MArray<utility::MMatrix> _worldTransform;
			utility::MArray<animation::Bone> _bones;
			utility::MDictionary<utility::MString, animation::Bone*> _bonesByName;

			// marray of sockets

			animation::Bone* _root = nullptr;

			virtual void Shutdown() override;

		public:

			//Default constructor
			inline MSkeleton() : MAsset(), _bones(BONES_PER_MESH), _bindPoseGlobalInverse(BONES_PER_MESH)
			{
				_type = MAssetType::SKELETON;
			}
			//Constructs MSkeleton from given data (i.e. file bytes array)
			inline MSkeleton(const utility::MFixedArray<MUint8>& data) : MAsset(MAssetType::SKELETON, data), 
				_bones(BONES_PER_MESH), _bindPoseGlobalInverse(BONES_PER_MESH)
			{
			}
			//Copy constructor
			inline MSkeleton(const MSkeleton& c) : MAsset(c),
				_bindPoseGlobalInverse(c._bindPoseGlobalInverse),
				_bones(c._bones),
				_bonesByName(c._bonesByName),
				_root(c._root)
			{
			}

			virtual ~MSkeleton();


			animation::Bone* CreateBone(const utility::MString& name, utility::MMatrix& inverseBindpose, utility::MMatrix& worldTransform, animation::Bone* parent);

			inline animation::Bone* GetBone(MSize index) { return &_bones[index]; }
			inline animation::Bone* GetBoneByName(const utility::MString& name) { return _bonesByName.GetValue(name); }


			// Inherited via MAsset
			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data) override;

			virtual bool LoadFromData(const utility::MArray<MUint8>& data) override;

			virtual bool LoadFromFile(resourceManagement::fileSystem::File & file) override;

			virtual bool SaveToFile(resourceManagement::fileSystem::File & file) const override;

			virtual void Initialize() override;

			inline MSize GetBoneCount() const { return _bones.GetSize(); }
			inline const utility::MArray<utility::MMatrix>& GetBindposesGlobalInverse() const { return _bindPoseGlobalInverse; }
		};

	}
}