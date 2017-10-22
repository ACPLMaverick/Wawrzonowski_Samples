#include "Bone.h"


namespace morphEngine
{
	namespace animation
	{

		Bone::Bone(const utility::MString& name, MSize flatIndex, utility::MMatrix* inverseBindpose, utility::MMatrix* worldTransform, Bone* parent) :
			_name(name),
			_flatIndex(flatIndex),
			_inverseBindposePtr(inverseBindpose),
			_worldTransformPtr(worldTransform),
			_parent(parent)
		{
			if (_parent != nullptr)
			{
				_parent->_children.Add(this);
			}
		}


		Bone::Bone(const Bone & copy) :
			_name(copy._name),
			_flatIndex(copy._flatIndex),
			_inverseBindposePtr(copy._inverseBindposePtr),
			_worldTransformPtr(copy._worldTransformPtr),
			_parent(copy._parent),
			_children(copy._children)
		{
		}

		Bone::~Bone()
		{
		}

		Bone & Bone::operator=(const Bone & copy)
		{
			_name = copy._name;
			_flatIndex = copy._flatIndex;
			_inverseBindposePtr = copy._inverseBindposePtr;
			_worldTransformPtr = copy._worldTransformPtr;
			_parent = copy._parent;
			_children = copy._children;
			return *this;
		}

	}
}