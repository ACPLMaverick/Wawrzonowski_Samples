#include "AnimationFrame.h"
#include "assetLibrary/MSkeleton.h"

namespace morphEngine
{
	using namespace assetLibrary;
	using namespace utility;

	namespace animation
	{

		AnimationFrame::AnimationFrame() : _data(16)
		{
		}

		AnimationFrame::AnimationFrame(const AnimationFrame & c) : _data(c._data), _time(c._time)
		{
		}


		AnimationFrame::~AnimationFrame()
		{
		}

		AnimationFrame & AnimationFrame::operator=(const AnimationFrame & c)
		{
			_data = c._data;
			_time = c._time;
			return *this;
		}

		void AnimationFrame::Initialize(utility::MArray<utility::MMatrix>& data, MFloat32 time, assetLibrary::MSkeleton * skeleton)
		{
			_data = data;
			_time = time;

			if (skeleton != nullptr)
			{
				ME_ASSERT(data.GetSize() == skeleton->GetBoneCount(), "Skeleton not compatible with animation data!");
				// Baking animation data into final matrices
				const MArray<MMatrix>& invBindPoses = skeleton->GetBindposesGlobalInverse();
				for (MSize i = 0; i < _data.GetSize(); ++i)
				{
					_data[i] = ((_data[i] * invBindPoses[i]));
				}
			}
		}

		void AnimationFrame::InitializeEmpty(MSize dataSize)
		{
			_data.Resize(dataSize);
			for (auto it = _data.GetBegin(); it.IsValid(); ++it)
			{
				(*it) = MMatrix::Identity;
			}
		}

	}
}