#pragma once

#include "gom/GameObjectComponent.h"
#include "utility/MMatrix.h"
#include "utility/MVector.h"
#include "utility/MArray.h"
#include "animation/Socket.h"
#include "core/Delegate.h"

#include "PhysX\PxPhysicsAPI.h"

namespace morphEngine
{
	namespace renderer
	{
		class RendererSkinnedMesh;
	}

	namespace gom
	{
		class Transform :
			public GameObjectComponent
		{
			ME_TYPE

		protected:

#pragma region Protected Const

			static const MFloat32 QUATERNION_NORMALIZE_DELTA;

#pragma endregion

#pragma region Protected

			//union
			//{
			//	struct
			//	{
					MQuaternion _rotation;				// WORLD SPACE
					MVector3 _position;					// WORLD SPACE
			//	};
			//	physx::PxTransform _physxTransform;
			//};
			MVector3 _scale = MVector3::One;	// LOCAL SPACE

			MMatrix _matWorld;
			MMatrix _matWorldInvTransp;

			memoryManagement::Handle<Transform> _parent;
			utility::MArray<memoryManagement::Handle<Transform>> _children;

			animation::Socket _socket;

			MFloat32 _timerQuaternionNormalize = 0.0f;

			bool _bNeedCreateWorldMatrix = false;

			virtual void RegisterProperties() override;

#pragma endregion

#pragma region Functions Protected

			Transform(const ObjectInitializer& initializer);

			virtual inline void CreateWorldMatrix();
			void NotifyParentTransformChanged();
			inline void OnParentTransformChanged();
			MVector3 GetWorldScale();

			inline MQuaternion GetRotationFromDirection(const MVector3& dir) const
			{
				MVector3 rot;
				MVector3 nDir = dir.Normalized();
				MVector3 w0 = MVector3(-nDir.Z, 0.0f, nDir.X);
				MVector3 u0 = MVector3::Cross(w0, nDir);

				// pitch
				rot.X = MMath::AsinD(nDir.Y);

				// yaw
				rot.Y = -MMath::Atan2D(nDir.X, nDir.Z);

				// roll
				rot.Z = MMath::Atan2D(MVector3::Dot(w0, MVector3::Up), MVector3::Dot(u0, MVector3::Up) / w0.Length() * u0.Length())/* * -0.5f*/;
				if (MMath::IsNan(rot.Z))
					rot.Z = 0.0f;
				//rot.Z = MMath::Atan2D(nDir.X, nDir.Y);
				//rot.Z = currentRot.Z;

				return MQuaternion::FromEuler(rot);
			}

#pragma endregion

		public:

#pragma region Events Public

			core::Event<void, memoryManagement::Handle<Transform>> EvtTransformChanged;
			core::Event<void, memoryManagement::Handle<Transform>> EvtParentChanged;

#pragma endregion

#pragma region Functions Public

			Transform(const Transform& c, bool bDeepCopy = true);
			virtual ~Transform();

			virtual void Initialize() override;
			virtual void Shutdown() override;

			Transform& operator=(const Transform& c);

			virtual void Update() override;

			virtual void PostDeserialize() override;

#pragma region GettersSetters

			inline memoryManagement::Handle<Transform> GetParent() const { return _parent; }
			inline bool HasParent() const { return _parent.IsValid(); }
			inline utility::MArray<memoryManagement::Handle<Transform>>::MIteratorArray GetChildrenIterator() { return _children.GetIterator(); }
			inline memoryManagement::Handle<Transform> GetChildAt(MSize index) const { return _children[index]; }
			inline MSize GetChildrenCount() const { return _children.GetSize(); }
			inline bool HasChildren() const { return _children.IsEmpty(); }

			inline const MMatrix& GetMatWorld() const { return _matWorld; }
			inline const MMatrix& GetMatWorldInverseTranspose() const { return _matWorldInvTransp; }

			inline void SetSocket(memoryManagement::Handle<renderer::RendererSkinnedMesh> renderer, MSize boneIndex)
			{
				if (_socket.GetIsInitialized())
				{
					_socket.Shutdown();
				}

				_socket.Initialize(static_cast<Handle<Transform>>(_this), renderer, boneIndex);
			}

			inline void SetSocketOfCurrentSkeleton(MSize boneIndex)
			{
				ME_WARNING_RETURN_STATEMENT(_socket.GetIsInitialized(), "Setting socket bone index but it is uninitialized.");
				_socket.SetBoneIndex(boneIndex);
			}

			inline void AddChild(memoryManagement::Handle<Transform> tr) { tr->SetParent(static_cast<memoryManagement::Handle<Transform>>(_this)); }
			inline void RemoveChild(memoryManagement::Handle<Transform> tr) { _children.Remove(tr); }
			inline void RemoveChildAt(MSize index) { _children.RemoveAt(index); }
			inline void ClearParent() 
			{
				if(_parent.IsValid())
				{
					_parent->RemoveChild(_this);
				}
				_parent.Invalidate();
				OnParentTransformChanged();
			}
			inline void SetParent(memoryManagement::Handle<Transform> tr) 
			{ 
				if(_parent.IsValid())
				{
					_parent->RemoveChild(_this);
					_parent.Invalidate();
				}
				_parent = tr; 
				OnParentTransformChanged(); 
				_parent->_children.Add(static_cast<memoryManagement::Handle<Transform>>(_this));
			}

			inline void RecalculateMatrix()
			{
				CreateWorldMatrix();
				NotifyParentTransformChanged();
				_bNeedCreateWorldMatrix = false;
			}

			inline const animation::Socket& GetSocket() { return _socket; }

			inline MVector3 GetPosition() const { return _position; }
			inline MVector3 GetRotation() const { return _rotation.ToEuler(); }
			inline MVector3 GetDirection() const { return (_rotation * MVector3::Forward).Normalized(); }
			inline MVector3 GetRight() const { return MVector3::Cross(_matWorld * MVector4(MVector3::Up, 0.0f), GetDirection()).Normalized(); }
			inline MVector3 GetUp() const { return MVector3::Cross(GetDirection(), GetRight()).Normalized(); }

			inline MVector3 GetLocalPosition() const
			{
				if (!_parent.IsValid())
				{
					return GetPosition();
				}
				else
				{
					return (_position / _parent->GetWorldScale() - _parent->_position);
				}
			}

			inline MVector3 GetLocalRotation() const
			{
				if (!_parent.IsValid())
				{
					return GetRotation();
				}
				else
				{
					return (_rotation * _parent->_rotation.Inversed()).ToEuler();
				}
			}

			inline MVector3 GetLocalDirection() const
			{
				if (!_parent.IsValid())
				{
					return GetDirection();
				}
				else
				{
					return (((_rotation * _parent->_rotation.Inversed()).ToMatrix() * MVector3::Forward)).Normalized();
				}
			}

			inline MVector3 GetLocalScale() const { return _scale; }
			inline MVector3 GetLocalRight() const { return MVector3::Cross(MVector3::Up, GetLocalDirection()); }

			//Methods for DynamicRigidbody and Rigidbody - useful for making objects that are moved by Physics AND by transform

			//Returns PhysX transform based on current transform
			inline const physx::PxTransform& GetPhysxTransform() const { return reinterpret_cast<const physx::PxTransform&>(_rotation); }
			//Sets transform based on given PhysX transform
			inline void SetPhysxTransform(const physx::PxTransform& transform) 
			{ 
				_rotation.X = transform.q.x;
				_rotation.Y = transform.q.y;
				_rotation.Z = transform.q.z;
				_rotation.W = transform.q.w;
				_position.X = transform.p.x;
				_position.Y = transform.p.y;
				_position.Z = transform.p.z;
			}


			inline void AddRotation(MVector3 rot)
			{
				MQuaternion nRot = MQuaternion::FromEuler(rot);
				_rotation = (_rotation * nRot).Normalized();
				_bNeedCreateWorldMatrix = true;
			}

			inline void SetPosition(const MVector3& pos)
			{
				_position = pos;
				_bNeedCreateWorldMatrix = true;
			}

			inline void SetRotation(const MVector3& rot)
			{
				_rotation = MQuaternion::FromEuler(rot);
				_bNeedCreateWorldMatrix = true;
			}

			inline void SetDirection(const MVector3 dir)
			{
				_rotation = GetRotationFromDirection(dir);
				_bNeedCreateWorldMatrix = true;
			}


			inline void SetLocalPosition(const MVector3& pos)
			{
				if (!_parent.IsValid())
				{
					SetPosition(pos);
				}
				else
				{
					_position = (pos * _parent->GetWorldScale()) + _parent->_position;
					_bNeedCreateWorldMatrix = true;
				}
			}

			inline void SetLocalRotation(const MVector3& rot)
			{
				if (!_parent.IsValid())
				{
					SetRotation(rot);
				}
				else
				{
					_rotation = (MQuaternion::FromEuler(rot) * _parent->_rotation);
					_bNeedCreateWorldMatrix = true;
				}
			}

			inline void SetLocalScale(const MVector3& scl)
			{
				_scale = scl;
				_bNeedCreateWorldMatrix = true;
			}

			inline void SetLocalDirection(const MVector3& dir)
			{
				if (!_parent.IsValid())
				{
					SetDirection(dir);
				}
				else
				{
					MVector3 dirWorld = _parent->_rotation * MVector4(dir, 0.0f);
					_rotation = (GetRotationFromDirection(dirWorld));
					_bNeedCreateWorldMatrix = true;
				}
			}

#pragma endregion

#pragma endregion

		};

	}
}