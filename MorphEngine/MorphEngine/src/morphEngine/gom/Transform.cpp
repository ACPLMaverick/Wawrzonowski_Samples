#include "Transform.h"
#include "core/Engine.h"
#include "gom/SceneManager.h"

namespace morphEngine
{
	namespace gom
	{

		const MFloat32 Transform::QUATERNION_NORMALIZE_DELTA = 0.5f;

		Transform::Transform(const ObjectInitializer& initializer) :
			GameObjectComponent(initializer)
		{
			RegisterProperties();
		}

		Transform::Transform(const Transform& c, bool bDeepCopy) :
			GameObjectComponent(c, bDeepCopy),
			_position(c._position),
			_rotation(c._rotation),
			_scale(c._scale),
			_matWorld(c._matWorld),
			_matWorldInvTransp(c._matWorldInvTransp),
			_bNeedCreateWorldMatrix(c._bNeedCreateWorldMatrix),
			_parent(c._parent)
		{
			if (bDeepCopy)
			{
				if (_parent.IsValid())
				{
					SetParent(_parent);
				}

				Scene* scene = SceneManager::GetInstance()->GetScene();

				if (c._owner.IsValid())
				{
					MSize size = c._children.GetSize();
					if (size > 0)
					{
						for (MSize i = 0; i < size; ++i)
						{
							if (scene != nullptr)
							{
								Handle<GameObject> childOwner = c._children[i]->GetOwner();
								Handle<GameObject> newChild;
								c._owner.GetType()->CloneInstance(static_cast<Handle<BaseObject>>(childOwner), newChild);
								scene->AddGameObject(newChild);

								AddChild(newChild->GetTransform());
							}
						}
					}
				}
			}

			RegisterProperties();
		}

		Transform::~Transform()
		{
		}

		void Transform::RegisterProperties()
		{
			RegisterProperty("Position", &_position);
			GetProperty<MVector3>("Position")->OnPropertyValueChanged += new core::Event<void>::ClassDelegate<Transform>(_this, &Transform::RecalculateMatrix);
			RegisterProperty("Rotation", &_rotation);
			GetProperty<MQuaternion>("Rotation")->OnPropertyValueChanged += new core::Event<void>::ClassDelegate<Transform>(_this, &Transform::RecalculateMatrix);
			RegisterProperty("Scale", &_scale);
			GetProperty<MVector3>("Scale")->OnPropertyValueChanged += new core::Event<void>::ClassDelegate<Transform>(_this, &Transform::RecalculateMatrix);
			RegisterProperty("Parent", &_parent, morphEngine::reflection::PropertyAttributes::Serializable | morphEngine::reflection::PropertyAttributes::HiddenInDetails);
			RegisterProperty("Children", &_children, morphEngine::reflection::PropertyAttributes::Serializable | morphEngine::reflection::PropertyAttributes::HiddenInDetails);
		}

		void Transform::Initialize()
		{
			GameObjectComponent::Initialize();

			if(_parent.IsValid() && !_parent.IsStoringID())
			{
				CreateWorldMatrix();
			}

			for (auto it = _children.GetIterator(); it.IsValid(); ++it)
			{
				if((*it).IsValid() && !(*it).IsStoringID())
				{
					(*it)->CreateWorldMatrix();
				}
			}
		}

		void Transform::Shutdown()
		{
			_socket.Shutdown();
			GameObjectComponent::Shutdown();
		}

		Transform& Transform::operator=(const Transform & c)
		{
			GameObjectComponent::operator=(c);

			_position = c._position;
			_rotation = c._rotation;
			_scale = c._scale;
			_bNeedCreateWorldMatrix = c._bNeedCreateWorldMatrix;

			if(_parent.IsValid() && !_parent.IsStoringID())
			{
				SetParent(_parent);
			}

			return *this;
		}

		void Transform::Update()
		{
			if (_socket.GetIsInitialized())
			{
				_socket.Update();
				RecalculateMatrix();
			}
			else if (_bNeedCreateWorldMatrix)
			{
				RecalculateMatrix();
			}

			if (_timerQuaternionNormalize > QUATERNION_NORMALIZE_DELTA)
			{
				_rotation.Normalize();
				_timerQuaternionNormalize = 0.0f;
			}
			else
			{
				_timerQuaternionNormalize += core::Engine::GetInstance()->GetGlobalTime().GetDeltaTime();
			}
		}

		void Transform::PostDeserialize()
		{
			GameObjectComponent::PostDeserialize();

			if(_parent.IsValid() && !_parent.IsStoringID() && _parent != _this)
			{
				//_parent->_children.Add(_this);
			}
		}

		inline void Transform::CreateWorldMatrix()
		{
			if (_socket.GetIsInitialized())
			{
				_matWorld = MMatrix::Translation(_position) *
					_rotation.ToMatrix() *
					MMatrix::Scale(GetWorldScale()) *
					_socket.GetMatrix();
			}
			else
			{
				_matWorld = MMatrix::Translation(_position) *
					_rotation.ToMatrix() *
					MMatrix::Scale(GetWorldScale());
			}
			
			_matWorldInvTransp = _matWorld.Inverted().Transposed();
		}

		inline void Transform::NotifyParentTransformChanged()
		{
			for (auto it = _children.GetIterator(); it.IsValid(); ++it)
			{
				(*it)->OnParentTransformChanged();
			}
			EvtTransformChanged(static_cast<memoryManagement::Handle<Transform>>(_this));
		}

		void Transform::OnParentTransformChanged()
		{
			// don't have to check if I have parent, do I?

			CreateWorldMatrix();

			EvtParentChanged(static_cast<memoryManagement::Handle<Transform>>(_this));

			NotifyParentTransformChanged();
		}

		MVector3 Transform::GetWorldScale()
		{
			if (!_parent.IsValid())
			{
				return _scale;
			}
			else
			{
				return _scale * _parent->GetWorldScale();
			}
		}

		
	}
}