#include "DummyInit.h"

#include "Type.h"

#pragma region Types includes

#include "core\MObject.h"

#include "gom\BaseObject.h"
#include "gom\GameObject.h"
#include "gom\GameObjectComponent.h"
#include "gom\Camera.h"
#include "gom\LuaComponent.h"
#include "gom\Transform.h"
#include "gom\GUITransform.h"
#include "gom\EditorCameraController.h"
#include "gom\ObjectClickEvaluator.h"

#include "animation\Animator.h"
#include "animation\Animation.h"
#include "animation\MultiAnimation.h"

#include "debugging\Statistics.h"

#include "renderer\OctTree.h"
#include "renderer\Renderer.h"
#include "renderer\RendererStaticMesh.h"
#include "renderer\RendererSkinnedMesh.h"
#include "renderer\Skybox.h"
#include "renderer\Light.h"
#include "renderer\LightAmbient.h"
#include "renderer\LightArea.h"
#include "renderer\LightDirectional.h"
#include "renderer\LightPoint.h"
#include "renderer\LightSpot.h"
#include "renderer\GUIBatch.h"
#include "renderer\GUIImage.h"
#include "renderer\GUIEmpty.h"
#include "renderer\GUIImageAnimated.h"
#include "renderer\GUIButton.h"
#include "renderer\GUIText.h"

#include "physics\Collider.h"
#include "physics\BoxCollider.h"
#include "physics\CapsuleCollider.h"
#include "physics\MeshCollider.h"
#include "physics\SphereCollider.h"
#include "physics\Rigidbody.h"
#include "physics\DynamicRigidbody.h"

#include "input\InputComponent.h"

#include "audio\AudioListener.h"
#include "audio\AudioSource.h"

#include "ai\AIManager.h"

#include "tests\RendererTestsController.h"

#pragma endregion

#pragma region Usings

using namespace morphEngine::ai;
using namespace morphEngine::animation;
using namespace morphEngine::assetLibrary;
using namespace morphEngine::audio;
using namespace morphEngine::core;
using namespace morphEngine::debugging;
using namespace morphEngine::gom;
using namespace morphEngine::input;
using namespace morphEngine::MCollectionHelper;
using namespace morphEngine::memoryManagement;
using namespace morphEngine::physics;
using namespace morphEngine::reflection;
using namespace morphEngine::renderer;
using namespace morphEngine::resourceManagement;
using namespace morphEngine::utility;
using namespace morphEngine::tests;

#pragma endregion

#pragma region Types registration
//These four typs must be registered in this order (C++ doesn't initialize statics in different translation units when you didn't call any method from that translation unit BUT it ensures that initialization of statics is in order defined by YOU)
REGISTER_ME_TYPE(MObject, nullptr);
REGISTER_ME_TYPE(BaseObject, MObject::GetType());
REGISTER_ME_TYPE(GameObject, BaseObject::GetType());
REGISTER_ME_TYPE(GameObjectComponent, BaseObject::GetType());

//Ensure that type relying on another type is registered AFTER its parent type (example: Renderers)

REGISTER_ME_TYPE(Animator, GameObjectComponent::GetType());
REGISTER_ME_TYPE(Animation, BaseObject::GetType());
REGISTER_ME_TYPE(MultiAnimation, Animation::GetType());

REGISTER_ME_TYPE(Statistics, GameObjectComponent::GetType());

REGISTER_ME_TYPE(OctTreeElement, GameObjectComponent::GetType());
REGISTER_ME_TYPE(Renderer, OctTreeElement::GetType());
REGISTER_ME_TYPE(RendererStaticMesh, Renderer::GetType());
REGISTER_ME_TYPE(RendererSkinnedMesh, RendererStaticMesh::GetType());
REGISTER_ME_TYPE(Skybox, Renderer::GetType());
REGISTER_ME_TYPE(Light, OctTreeElement::GetType());
REGISTER_ME_TYPE(LightSpot, Light::GetType());
REGISTER_ME_TYPE(LightPoint, Light::GetType());
REGISTER_ME_TYPE(LightDirectional, Light::GetType());
REGISTER_ME_TYPE(LightArea, Light::GetType());
REGISTER_ME_TYPE(LightAmbient, Light::GetType());
REGISTER_ME_TYPE(GUIBatch, GameObjectComponent::GetType());
REGISTER_ME_TYPE(GUIImage, GameObjectComponent::GetType());
REGISTER_ME_TYPE(GUIEmpty, GUIImage::GetType());
REGISTER_ME_TYPE(GUIText, GUIImage::GetType());
REGISTER_ME_TYPE(GUIButton, GUIImage::GetType());

REGISTER_ME_TYPE(Collider, GameObjectComponent::GetType());
REGISTER_ME_TYPE(BoxCollider, Collider::GetType());
REGISTER_ME_TYPE(CapsuleCollider, Collider::GetType());
REGISTER_ME_TYPE(MeshCollider, Collider::GetType());
REGISTER_ME_TYPE(SphereCollider, Collider::GetType());
REGISTER_ME_TYPE(Rigidbody, GameObjectComponent::GetType());
REGISTER_ME_TYPE(DynamicRigidbody, Rigidbody::GetType());
REGISTER_ME_TYPE(InputComponent, GameObjectComponent::GetType());

REGISTER_ME_TYPE(Camera, GameObjectComponent::GetType());
REGISTER_ME_TYPE(LuaComponent, GameObjectComponent::GetType());
REGISTER_ME_TYPE(Transform, GameObjectComponent::GetType());
REGISTER_ME_TYPE(GUITransform, Transform::GetType());
REGISTER_ME_TYPE(EditorCameraController, GameObjectComponent::GetType());
REGISTER_ME_TYPE(ObjectClickEvaluator, GameObjectComponent::GetType());

REGISTER_ME_TYPE(AudioSource, GameObjectComponent::GetType());
REGISTER_ME_TYPE(AudioListener, GameObjectComponent::GetType());

REGISTER_ME_TYPE(RendererTestsController, GameObjectComponent::GetType());

#pragma endregion

namespace morphEngine
{
	namespace reflection
	{
		void DummyInit()
		{
			Assembly* a = Assembly::GetInstance();
			Assembly* b = a;
			a += 5;
		}
	}
}