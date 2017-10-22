#pragma once

#include "core/GlobalDefines.h"
#include "utility/Collections.h"
#include "memoryManagement/Handle.h"
#include "gom/GameObjectComponent.h"
#include "BoundsBox.h"

namespace morphEngine
{
	namespace gom
	{
		class Camera;
		class Transform;
		struct RenderSet;
	}

	namespace renderer
	{
		class Renderer;
		class LightAmbient;
		class LightDirectional;
		class LightPoint;
		class LightSpot;
		class LightArea;

		// Abstract class, declaring that a class deriving from it, has its own bounds object.
		class OctTreeElement : public gom::GameObjectComponent
		{
			ME_TYPE
		protected:

			BoundsBox _bounds;
			bool _bForcePlaceAtRoot = false;

			inline OctTreeElement(const gom::ObjectInitializer& initializer) : gom::GameObjectComponent(initializer) { }

			virtual void RegisterComponent() override;
			virtual void DeregisterComponent() override;

			void UpdateBoundsWorld(memoryManagement::Handle<gom::Transform> transform);

		public:

			inline OctTreeElement(const OctTreeElement& c, bool bDeepCopy = true) : gom::GameObjectComponent(c, bDeepCopy), _bounds(c._bounds) { }
			inline virtual ~OctTreeElement() { }

			virtual void Initialize() override 
			{ 
				gom::GameObjectComponent::Initialize(); 
				_bounds.Initialize(); 
				_owner->GetTransform()->EvtTransformChanged += new core::Event<void, Handle<gom::Transform>>::ClassDelegate<OctTreeElement>(_this, &OctTreeElement::UpdateBoundsWorld);
			}
			virtual void Shutdown() override 
			{ 
				_owner->GetTransform()->EvtTransformChanged -= new core::Event<void, Handle<gom::Transform>>::ClassDelegate<OctTreeElement>(_this, &OctTreeElement::UpdateBoundsWorld);
				_bounds.Shutdown(); 
				gom::GameObjectComponent::Shutdown();
			}

			inline void SetForcePlaceAtRoot(bool val) { _bForcePlaceAtRoot = val; }

			virtual const BoundsBox& GetBounds() const { return _bounds; }
			inline bool GetForcePlaceAtRoot() const { return _bForcePlaceAtRoot; }
		};

		class OctTree
		{
		protected:

#pragma region Structs Protected

			struct Node
			{
			public:
				static const MSize NODE_COUNT = 8;
				static const MSize INVALID_INDEX = -1;

				BoundsBox Box;
				utility::MArray<memoryManagement::Handle<OctTreeElement>> Elements;

				MSize Index = INVALID_INDEX;
				MSize Depth = 0;

				// TLF, TRF, TRB, TLB, BLF, BRF, BRB, BLB
				Node* Children[NODE_COUNT] = { nullptr };
				Node* Parent = nullptr;

			protected:

				MSize _childCount = 0;

			public:

				void CreateChild(MSize i);
				void CreateChild(MSize i, const BoundsBox& externalBounds);
				void GetBoundsOfChild(MSize i, BoundsBox& outMax);
				void RemoveChild(MSize i);
				inline MSize GetChildCount() { return _childCount; }

				inline bool GetHasChildren() { return Elements.GetSize() != 0; }
				inline bool GetIsLeaf() { return GetChildCount() == 0; }
			};

			struct NodeElementPair
			{
				memoryManagement::Handle<OctTreeElement> Element;
				Node* NodePtr = nullptr;

				inline NodeElementPair() { }
				inline NodeElementPair(memoryManagement::Handle<OctTreeElement> element, Node* nodePtr) : Element(element), NodePtr(nodePtr) { }

				inline bool operator==(const NodeElementPair& other) const { return NodePtr == other.NodePtr && Element == other.Element; }
				inline bool operator!=(const NodeElementPair& other) const { return !operator==(other); }
			};

#pragma endregion

#pragma region Protected

			utility::MDictionary<memoryManagement::Handle<gom::Transform>, NodeElementPair> _mapTransformToNode;
			utility::MArray<memoryManagement::Handle<OctTreeElement>> _elements;
			utility::MArray<Node*> _nodes;
			MSize _maxDepth = 10;
			Node* _root = nullptr;

#pragma endregion

#pragma region Functions Protected

			inline Node* AddInternal(memoryManagement::Handle<OctTreeElement> element, Node* startNode);
			inline void AddRendererToRenderSet(gom::RenderSet& renderSet, memoryManagement::Handle<Renderer> renderer) const;

			void OnElementTransformChanged(memoryManagement::Handle<gom::Transform> transform);

#pragma endregion

		public:

#pragma region Functions Public

			OctTree();
			~OctTree();

			void Initialize(utility::MArray<memoryManagement::Handle<OctTreeElement>>& startElements, 
				const MVector3& minDimensions = MVector3(1000.0f, 1000.0f, 1000.0f),
				MSize maxDepth = 10);
			void Shutdown();

			// Returns visible elements from camera point of view
			void FrustumCull(memoryManagement::Handle<gom::Camera> camera,
				utility::MArray<memoryManagement::Handle<OctTreeElement>>& outElements) const;

			void FrustumCull(memoryManagement::Handle<gom::Camera> camera, gom::RenderSet& renderSet) const;

			void Add(memoryManagement::Handle<OctTreeElement> element);
			void Remove(memoryManagement::Handle<OctTreeElement> element);
			const utility::MArray<memoryManagement::Handle<OctTreeElement>>& GetAllElements();

			MSize GetMaxDepth() const { return _maxDepth; }
			void SetMaxDepth(MSize depth) { _maxDepth = depth; }

#pragma endregion

		};
	}
}