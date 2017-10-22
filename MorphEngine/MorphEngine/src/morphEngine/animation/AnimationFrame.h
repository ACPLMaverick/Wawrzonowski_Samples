#pragma once
#include "core/GlobalDefines.h"
#include "utility/Collections.h"
#include "utility/MMatrix.h"

namespace morphEngine
{
	namespace assetLibrary
	{
		class MSkeleton;
	}

	namespace animation
	{
		/*
			This is a key frame for an animation. Consists of set of global transformation matrices (each matrix for each skeleton bone)
			When combined with inverse bindpose matrices from skeleton should give correct animation transforms, ready to pass into shader
		*/
		class AnimationFrame
		{
		protected:

			utility::MArray<utility::MMatrix> _data;
			MFloat32 _time;

		public:

			AnimationFrame();
			AnimationFrame(const AnimationFrame& c);
			~AnimationFrame();

			AnimationFrame& operator=(const AnimationFrame& c);

			inline bool operator==(const AnimationFrame& other) const
			{
				return _time == other._time && _data == other._data;
			}

			inline bool operator!=(const AnimationFrame& other) const
			{
				return _time != other._time || _data != other._data;
			}

			// Automatically bakes animation data matrices for given skeleton. Each animation frame is read only for now.
			void Initialize(utility::MArray<utility::MMatrix>& data, MFloat32 time, assetLibrary::MSkeleton* skeleton);
			void InitializeEmpty(MSize dataSize);

			inline utility::MArray<utility::MMatrix>& GetData() const { return const_cast<utility::MArray<utility::MMatrix>&>(_data); }
			inline const utility::MMatrix* GetDataPtr() const { _data.GetDataPointer(); }
			inline MSize GetDataSize() const { return _data.GetSize(); }
			inline MFloat32 GetTime() const { return _time; }

			inline static void Interpolate(const AnimationFrame& a, const AnimationFrame& b, MFloat32 interp, AnimationFrame& outFrame)
			{
				ME_ASSERT((a.GetDataSize() == b.GetDataSize()) && (a.GetDataSize() == outFrame.GetDataSize()) && ((b.GetDataSize() == outFrame.GetDataSize())), 
					"Animation frames not compatible.");
				for (MSize i = 0; i < a.GetDataSize(); ++i)
				{
					outFrame._data[i] = utility::MMatrix::Lerp(a._data[i], b._data[i], interp);
				}
			}

			inline static void InterpolateMultiple(utility::MArray<const AnimationFrame*>& frames, utility::MArray<MFloat32>& weights, AnimationFrame& outFrame)
			{
				ME_ASSERT(frames.GetSize() == weights.GetSize() && frames.GetSize() > 1, "Animation frames and weights not compatible.");

				// not using math cause speed
				MSize dataSize = frames[0]->GetDataSize();
				MSize dataSize2 = frames[1]->GetDataSize();
				MSize framesSize = frames.GetSize();
				for (MSize i = 0; i < dataSize; ++i)
				{
					outFrame._data[i] = utility::MMatrix(0.0f);
					for (MSize j = 0; j < framesSize; ++j)
					{
						MFloat32 w = weights[j];
						outFrame._data[i] += (frames[j]->_data[i] * w);
					}
				}
			}

		};

	}
}