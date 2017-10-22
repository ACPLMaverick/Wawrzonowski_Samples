#pragma once
#include "MAsset.h"
#include "utility/Collections.h"
#include "utility/MMatrix.h"
#include "animation/AnimationFrame.h"
#include "animation/Timeline.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MMesh;

		class MAnimationClip : public MAsset
		{
			friend class MMesh;
			friend class resourceManagement::ResourceManager;
		protected:

#pragma region Protected

			utility::MString _name;

			utility::MArray<animation::AnimationFrame> _frames;

			// length in seconds
			MFloat32 _length;

			// id of skeleton
			utility::MString _skeletonPath;

#pragma endregion

#pragma region Functions Protected

			virtual void Shutdown() override;

			inline bool LoadFBX(resourceManagement::fileSystem::File & file);
			inline bool LoadMAsset(resourceManagement::fileSystem::File & file);

#pragma endregion

#pragma region Functions public

		public:
			//Default constructor
			inline MAnimationClip() : MAsset() { _type = MAssetType::ANIMATION_CLIP; }
			//Constructs MAnimationClip with given type and loads data from given bytes array
			inline MAnimationClip(MAssetType type, const utility::MFixedArray<MUint8>& data) : MAsset(type, data) { }
			inline MAnimationClip(const MAnimationClip& c) : MAsset(c),
				_name(c._name), _frames(c._frames), _length(c._length), _skeletonPath(c._skeletonPath)
			{
			}

			inline virtual ~MAnimationClip()
			{
				Shutdown();
			}

			inline utility::MArray<animation::AnimationFrame>& GetFrames() const { return const_cast<utility::MArray<animation::AnimationFrame>&>(_frames); }
			inline MFloat32 GetLength() const { return _length; }
			inline const utility::MString& GetName() const { return _name; }
			inline utility::MString GetSkeletonPath() const { return _skeletonPath; }

			// Inherited via MAsset
			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data) override;
			virtual bool LoadFromData(const utility::MArray<MUint8>& data) override;
			virtual bool LoadFromFile(resourceManagement::fileSystem::File & file) override;
			virtual bool SaveToFile(resourceManagement::fileSystem::File & file) const override;
			virtual void Initialize() override;

#pragma endregion

		};

	}
}