#pragma once
#include "gom/Transform.h"

#define GUI_TRANSFORM_BASE_SIZE utility::MVector2(256.0f, 256.0f)

namespace morphEngine
{
	namespace gom
	{
		class Camera;

		class GUITransform :
			public Transform
		{
			ME_TYPE
		protected:

			utility::MMatrix _guiMatrix;

			utility::MVector2 _origin = utility::MVector2::Zero; // (-1, 1)
			utility::MVector2 _size = GUI_TRANSFORM_BASE_SIZE;	// (0, w; 0, h)
			utility::MVector2 _anchor = utility::MVector2::Zero; // (-1, 1)

			memoryManagement::Handle<gom::Camera> _parentBatchCamera;
			memoryManagement::Handle<gom::GameObject> _parentBatchGameObject;

			GUITransform(const ObjectInitializer& initializer);

			virtual void RegisterProperties();

			virtual inline void CreateWorldMatrix() override;
			inline bool GetParentGUIBatchCamera();
			void OnScreenResized(MInt32 nWidth, MInt32 nHeight);
			void OnParentBatchCameraChanged(memoryManagement::Handle<gom::Camera> nuCamera);

		public:
			GUITransform(const GUITransform& c, bool bDeepCopy = true);
			virtual ~GUITransform();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			inline memoryManagement::Handle<gom::GameObject> GetBatchGameObject() { return _parentBatchGameObject; }

			// In clip space
			inline void SetOrigin(const utility::MVector2& origin) { _origin = origin; _bNeedCreateWorldMatrix = true; }
			// In screen space
			inline void SetSize(const utility::MVector2& size) { _size = size; _bNeedCreateWorldMatrix = true; }
			inline void SetSizeProportionalX(MFloat32 x)
			{
				SetSize(MVector2(x, _size.Y * (x / _size.X)));
			}
			inline void SetSizeProportionalY(MFloat32 y)
			{
				SetSize(MVector2(_size.X * (y / _size.Y), y));
			}
			// In clip space
			inline void SetAnchor(const utility::MVector2& anchor) { _anchor = anchor; _bNeedCreateWorldMatrix = true; }

			//void SetLocalOrigin(const utility::MVector2& origin);
			//void SetLocalAnchor(const utility::MVector2& anchor);

			// In clip space
			inline const utility::MVector2& GetOrigin() const { return _origin; }
			// In screen space
			inline const utility::MVector2& GetSize() const { return _size; }
			// In clip space
			inline const utility::MVector2& GetAnchor() const { return _anchor; }

			////utility::MVector2 GetLocalOrigin() const;
			////utility::MVector2 GetLocalAnchor() const;

			// Complete transformation from fullscreen plane to final screen position.
			inline const utility::MMatrix& GetGUIMatrix() const { return _guiMatrix; }
		};
	}
}