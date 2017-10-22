#include "Animator.h"
#include "assetLibrary/MAnimationClip.h"
#include "renderer/RendererSkinnedMesh.h"

namespace morphEngine
{
	using namespace memoryManagement;
	using namespace renderer;
	using namespace gom;

	namespace animation
	{
		void Animator::RegisterProperties()
		{
			RegisterProperty("StateMachine", &_sm);
		}

		void Animator::Initialize()
		{
			Handle<RendererSkinnedMesh> rskm(_owner->GetComponent<RendererSkinnedMesh>());
			if (rskm.IsValid() && !rskm->GetAnimator().IsValid())
			{
				rskm->SetAnimator(static_cast<Handle<Animator>>(_this));
			}
		}
	}
}