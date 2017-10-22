#pragma once
#include "core/GlobalDefines.h"
#include "utility/MMatrix.h"
#include "utility/MArray.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MMesh;
		class MSkeleton;
	}

	namespace animation
	{

		class Bone
		{
			friend class assetLibrary::MMesh;
			friend class assetLibrary::MSkeleton;
		protected:

			utility::MString _name;
			utility::MMatrix* _inverseBindposePtr;
			utility::MMatrix* _worldTransformPtr;
			utility::MArray<Bone*> _children;
			MSize _flatIndex;
			Bone* _parent;

			Bone(const utility::MString& name, MSize flatIndex, utility::MMatrix* inverseBindpose, utility::MMatrix* worldTransform, Bone* parent);

		public:
			Bone(const Bone& copy);
			~Bone();

			Bone& operator=(const Bone& other);

			inline bool operator==(const Bone& other) const { return _inverseBindposePtr == other._inverseBindposePtr; }
			inline bool operator!=(const Bone& other) const { return _inverseBindposePtr != other._inverseBindposePtr; }

			inline const utility::MMatrix& GetInverseBindpose() const { return *_inverseBindposePtr; }
			inline const utility::MMatrix& GetWorldTransform() const { return *_worldTransformPtr; }
			inline MSize GetFlatIndex() const { return _flatIndex; }
			inline utility::MArray<Bone*>& GetChildren() const { return const_cast<utility::MArray<Bone*>&>(_children); }
			inline Bone* GetParent() const { return _parent; }
			inline const utility::MString& GetName() const { return _name; }
		};

	}
}