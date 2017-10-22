#include "GUITransform.h"
#include "renderer/GUIBatch.h"
#include "gom/Camera.h"
#include "core/MessageSystem.h"

namespace morphEngine
{
	using namespace utility;
	using namespace memoryManagement;
	using namespace core;

	namespace gom
	{
		GUITransform::GUITransform(const ObjectInitializer& initializer) : Transform(initializer)
		{
			RegisterProperties();
		}

		GUITransform::GUITransform(const GUITransform & c, bool bDeepCopy) : Transform(c, bDeepCopy)
		{
			RegisterProperties();
		}

		GUITransform::~GUITransform()
		{
		}

		void GUITransform::Initialize()
		{
			// get parent gui batch camera. If none is found print warning.
			if (!GetParentGUIBatchCamera())
			{
				ME_WARNING(false, "GUITransform: No parent GUIBatch component found in GUI!");
			}
			else
			{
				// register for resize event if camera is registered for it
				if (_parentBatchCamera.IsValid() && _parentBatchCamera->GetResizesWithWindow())
				{
					MessageSystem::OnWindowResize += new Event<void, MInt32, MInt32>::ClassDelegate<GUITransform>(_this, &GUITransform::OnScreenResized);
				}
			}

			Transform::Initialize();
		}

		void GUITransform::Shutdown()
		{
			if (_parentBatchCamera.IsValid() && _parentBatchCamera->GetResizesWithWindow())
			{
				MessageSystem::OnWindowResize -= new Event<void, MInt32, MInt32>::ClassDelegate<GUITransform>(_this, &GUITransform::OnScreenResized);
			}
		}

		//void GUITransform::SetLocalOrigin(const utility::MVector2 & origin)
		//{
		//}

		//void GUITransform::SetLocalAnchor(const utility::MVector2 & anchor)
		//{
		//}

		//utility::MVector2 GUITransform::GetLocalOrigin() const
		//{
		//	return utility::MVector2();
		//}

		//utility::MVector2 GUITransform::GetLocalAnchor() const
		//{
		//	return utility::MVector2();
		//}

		void GUITransform::RegisterProperties()
		{
			RegisterProperty("Origin", &_origin);
			RegisterProperty("Size", &_size);
			RegisterProperty("Anchor", &_anchor);
		}

		inline void GUITransform::CreateWorldMatrix()
		{
			Transform::CreateWorldMatrix();

#if _DEBUG
			if (_parentBatchCamera.IsValid())
			{
#endif // _DEBUG

				MVector2 screenRec(
					1.0f / static_cast<MFloat32>(_parentBatchCamera->GetWidth()),
					1.0f / static_cast<MFloat32>(_parentBatchCamera->GetHeight()));
				MVector2 sizeToPixels(_size * screenRec);

				MMatrix originTrans(MMatrix::Translation(-_origin));

				MMatrix trans(MMatrix::Translation(MVector3(_anchor, 0.0f) + (_position * screenRec * 2.0f)));

				MMatrix scale(MMatrix::Scale(_scale * sizeToPixels));
				MMatrix rotationWorld = _rotation.ToMatrix();

				_guiMatrix = trans * rotationWorld * scale * originTrans;

				ME_ASSERT_S(!MMath::IsNan(_guiMatrix[0][0]) && !MMath::IsInf(_guiMatrix[0][0]));
#if _DEBUG
			}
			else
			{
				_guiMatrix = _matWorld;
			}
#endif // _DEBUG

		}

		inline bool GUITransform::GetParentGUIBatchCamera()
		{
			Handle<Transform> parentTransform = GetParent();
			while (parentTransform.IsValid())
			{
				Handle<GameObject> parentOwner = parentTransform->GetOwner();
				Handle<renderer::GUIBatch> batch = parentOwner->GetComponent<renderer::GUIBatch>();
				if (batch.IsValid())
				{
					_parentBatchCamera = batch->GetRenderCamera();
					_parentBatchGameObject = parentOwner;
					return true;
				}
				parentTransform = parentTransform->GetParent();
			}
			return false;
		}

		void GUITransform::OnScreenResized(MInt32 nWidth, MInt32 nHeight)
		{
			_bNeedCreateWorldMatrix = true;
		}

		void GUITransform::OnParentBatchCameraChanged(memoryManagement::Handle<gom::Camera> nuCamera)
		{
			_parentBatchCamera = nuCamera;
		}
	}
}