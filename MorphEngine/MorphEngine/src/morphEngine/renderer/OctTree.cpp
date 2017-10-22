#include "OctTree.h"
#include "gom/Camera.h"
#include "gom/SceneManager.h"

#include "renderer/Renderer.h"
#include "assetLibrary/MMaterial.h"

#include "renderer/LightAmbient.h"
#include "renderer/LightDirectional.h"
#include "renderer/LightPoint.h"
#include "renderer/LightSpot.h"
#include "renderer/LightArea.h"

#include "gom/Scene.h"

namespace morphEngine
{
	using namespace gom;
	using namespace assetLibrary;

	namespace renderer
	{
		OctTree::OctTree()
		{
		}

		OctTree::~OctTree()
		{
		}

		void OctTree::Initialize(utility::MArray<memoryManagement::Handle<OctTreeElement>>& startElements, const MVector3& minDimensions, MSize maxDepth)
		{
			_maxDepth = maxDepth;

			if (_root != nullptr)
			{
				Shutdown();
			}

			// create root node
			_root = new Node();
			_root->Box.MinLocal = MVector3(FLT_MAX, FLT_MAX, FLT_MAX);
			_root->Box.MaxLocal = MVector3(FLT_MIN, FLT_MIN, FLT_MIN);

			for (auto it = startElements.GetBegin(); it.IsValid(); ++it)
			{
				_root->Box.MinLocal = MVector3::Min((*it)->GetBounds().GetMinWorld(), _root->Box.MinLocal);
				_root->Box.MaxLocal = MVector3::Max((*it)->GetBounds().GetMaxWorld(), _root->Box.MaxLocal);
			}

			_root->Box.MinLocal = MVector3::Min(-minDimensions * 0.5f, _root->Box.MinLocal);
			_root->Box.MaxLocal = MVector3::Max(minDimensions * 0.5f, _root->Box.MaxLocal);

			_root->Box.UpdateToWorldIdentity();

			_nodes.Add(_root);

			// emplace every element in octtree
			for (auto it = startElements.GetBegin(); it.IsValid(); ++it)
			{
				Add((*it));
			}
		}

		void OctTree::Shutdown()
		{
			for (MSize i = 0; i < _nodes.GetSize(); ++i)
			{
				delete _nodes[i];
			}

			_mapTransformToNode.Clear();
			_elements.Clear();
			_nodes.Clear();

			_root = nullptr;
		}

		void OctTree::FrustumCull(memoryManagement::Handle<Camera> camera, utility::MArray<memoryManagement::Handle<OctTreeElement>>& outElements) const
		{
			// DFS through tree to collect elements from all nodes that intersect with camera's frustum.
			// Check intersection on children, add only those who intersect.
			// Root node is not checked.

			const BoundsFrustum& fru = camera->GetBounds();
			MStack<Node*> nodeStack;
			nodeStack.Push(_root);

			while (!nodeStack.IsEmpty())
			{
				Node* node = nodeStack.Pop();

				// add elements from this node
				for (auto it = node->Elements.GetBegin(); it.IsValid(); ++it)
				{
					if((*it)->GetForcePlaceAtRoot() || fru.IntersectsWith((*it)->GetBounds()))
						outElements.Add((*it));
				}

				// push to stack elements which intersect with the frustum
				for (MSize i = 0; i < Node::NODE_COUNT; ++i)
				{
					if (node->Children[i] != nullptr)
					{
						if (fru.IntersectsWith(node->Children[i]->Box))
						{
							nodeStack.Push(node->Children[i]);
						}
					}
				}
			}
		}

		void OctTree::FrustumCull(memoryManagement::Handle<Camera> camera, RenderSet& renderSet) const
		{
			// DFS through tree to collect elements from all nodes that intersect with camera's frustum.
			// Check intersection on children, add only those who intersect.
			// Root node is not checked.

			const BoundsFrustum& fru = camera->GetBounds();
			MStack<Node*> nodeStack;
			nodeStack.Push(_root);

			while (!nodeStack.IsEmpty())
			{
				Node* node = nodeStack.Pop();

				// add elements from this node
				for (auto it = node->Elements.GetBegin(); it.IsValid(); ++it)
				{
					if ((*it)->GetForcePlaceAtRoot() || fru.IntersectsWith((*it)->GetBounds()))
					{
						// Check type of objects
						if ((*it).GetType()->IsA(Renderer::GetType()))
						{
							Handle<Renderer> renderer(static_cast<Handle<Renderer>>(*it));
							AddRendererToRenderSet(renderSet, renderer);
						}
						else if ((*it).GetType()->IsA(LightAmbient::GetType()))
						{
							renderSet.LightAmb = static_cast<Handle<LightAmbient>>(*it).GetPointer();
						}
						else if ((*it).GetType()->IsA(LightDirectional::GetType()))
						{
							renderSet.LightsDirectional.Add(static_cast<Handle<LightDirectional>>(*it).GetPointer());
						}
						else if ((*it).GetType()->IsA(LightPoint::GetType()))
						{
							renderSet.LightsPoint.Add(static_cast<Handle<LightPoint>>(*it).GetPointer());
						}
						else if ((*it).GetType()->IsA(LightSpot::GetType()))
						{
							renderSet.LightsSpot.Add(static_cast<Handle<LightSpot>>(*it).GetPointer());
						}
						else if ((*it).GetType()->IsA(LightArea::GetType()))
						{
							renderSet.LightsArea.Add(static_cast<Handle<LightArea>>(*it).GetPointer());
						}
						else
						{
							ME_ASSERT(false, "There is an invalid type of object in OctTree.");
						}
					}
				}

				// push to stack elements which intersect with the frustum
				for (MSize i = 0; i < Node::NODE_COUNT; ++i)
				{
					if (node->Children[i] != nullptr)
					{
						if (fru.IntersectsWith(node->Children[i]->Box))
						{
							nodeStack.Push(node->Children[i]);
						}
					}
				}
			}
		}

		void OctTree::Add(memoryManagement::Handle<OctTreeElement> element)
		{
			ME_ASSERT(_root != nullptr, "OctTree not initialized!");

			Node* elemNode = _root;
			if (element->GetForcePlaceAtRoot())
			{
				_root->Elements.Add(element);
			}
			else
			{
				elemNode = AddInternal(element, _root);
			}

			ME_ASSERT_S(elemNode != nullptr);
			_elements.Add(element);
			_mapTransformToNode.Add(element->GetOwner()->GetTransform(), NodeElementPair(element, elemNode));
			element->GetOwner()->GetTransform()->EvtTransformChanged += new core::Event<void, Handle<Transform>>::ClassRawDelegate<OctTree>(this, &OctTree::OnElementTransformChanged);
		}

		void OctTree::Remove(memoryManagement::Handle<OctTreeElement> element)
		{
			ME_WARNING_RETURN_STATEMENT(_elements.Contains(element), "Element not present in octtree.");

			element->GetOwner()->GetTransform()->EvtTransformChanged -= new core::Event<void, Handle<Transform>>::ClassRawDelegate<OctTree>(this, &OctTree::OnElementTransformChanged);
			_elements.Remove(element);
			NodeElementPair nodeElement = _mapTransformToNode.GetValue(element->GetOwner()->GetTransform());
			_mapTransformToNode.Remove(element->GetOwner()->GetTransform());
			nodeElement.NodePtr->Elements.Remove(element);

			// node stays in nodes array, unless it is leaf and no other elements are present in it
			if (nodeElement.NodePtr->GetIsLeaf() && !nodeElement.NodePtr->GetHasChildren() && nodeElement.NodePtr != _root)
			{
				nodeElement.NodePtr->Parent->RemoveChild(nodeElement.NodePtr->Index);
				_nodes.Remove(nodeElement.NodePtr);
			}
		}

		const utility::MArray<memoryManagement::Handle<OctTreeElement>>& OctTree::GetAllElements()
		{
			return _elements;
		}

		void OctTree::Node::CreateChild(MSize i)
		{
			BoundsBox bb;
			GetBoundsOfChild(i, bb);
			CreateChild(i, bb);
		}

		void OctTree::Node::CreateChild(MSize i, const BoundsBox & externalBounds)
		{
			ME_ASSERT_S(Children[i] == nullptr);
			Children[i] = new Node();
			Children[i]->Index = i;
			Children[i]->Depth = Depth + 1;
			Children[i]->Parent = this;

			Children[i]->Box = externalBounds;
			Children[i]->Box.Initialize();
			++_childCount;
		}

		void OctTree::Node::GetBoundsOfChild(MSize i, BoundsBox & outBox)
		{
			// TLF, TRF, TRB, TLB, BLF, BRF, BRB, BLB
			switch (i)
			{
			case 0:

				outBox.MinLocal = MVector3(Box.GetMinWorld().X, Box.GetCenterWorld().Y, Box.GetCenterWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetCenterWorld().X, Box.GetMaxWorld().Y, Box.GetMaxWorld().Z);

				break;
			case 1:

				outBox.MinLocal = Box.GetCenterWorld();
				outBox.MaxLocal = Box.GetMaxWorld();

				break;
			case 2:

				outBox.MinLocal = MVector3(Box.GetCenterWorld().X, Box.GetCenterWorld().Y, Box.GetMinWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetMaxWorld().X, Box.GetMaxWorld().Y, Box.GetCenterWorld().Z);

				break;
			case 3:

				outBox.MinLocal = MVector3(Box.GetMinWorld().X, Box.GetCenterWorld().Y, Box.GetMinWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetCenterWorld().X, Box.GetMaxWorld().Y, Box.GetCenterWorld().Z);

				break;
			case 4:

				outBox.MinLocal = MVector3(Box.GetMinWorld().X, Box.GetMinWorld().Y, Box.GetCenterWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetCenterWorld().X, Box.GetCenterWorld().Y, Box.GetMaxWorld().Z);

				break;
			case 5:

				outBox.MinLocal = MVector3(Box.GetCenterWorld().X, Box.GetMinWorld().Y, Box.GetCenterWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetMaxWorld().X, Box.GetCenterWorld().Y, Box.GetMaxWorld().Z);

				break;
			case 6:

				outBox.MinLocal = MVector3(Box.GetCenterWorld().X, Box.GetMinWorld().Y, Box.GetMinWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetMaxWorld().X, Box.GetCenterWorld().Y, Box.GetCenterWorld().Z);

				break;
			case 7:

				outBox.MinLocal = MVector3(Box.GetMinWorld().X, Box.GetMinWorld().Y, Box.GetMinWorld().Z);
				outBox.MaxLocal = MVector3(Box.GetCenterWorld().X, Box.GetCenterWorld().Y, Box.GetCenterWorld().Z);

				break;
			default:
				ME_ASSERT_S(false);
				break;
			}

			outBox.UpdateToWorldIdentity();
		}

		void OctTree::Node::RemoveChild(MSize i)
		{
			ME_ASSERT_S(Children[i] != nullptr && Children[i]->GetIsLeaf())
			{
				--_childCount;
				delete Children[i];
				Children[i] = nullptr;
			}
		}

		inline OctTree::Node* OctTree::AddInternal(memoryManagement::Handle<OctTreeElement> element, Node * startNode)
		{
			// traverse tree in order to find "just thight fit" node
			MSize depthCounter = 0;
			Node* node = startNode;
			bool childFound = false;
			BoundsBox tempBounds;
			while (depthCounter <= _maxDepth)
			{
				// check if current bounds fits completely into one of the possibly future children

				if (depthCounter < _maxDepth)
				{
					for (MSize i = 0; i < Node::NODE_COUNT; ++i)
					{
						if (node->Children[i] == nullptr)
						{
							node->GetBoundsOfChild(i, tempBounds);
							if (element->GetBounds().IsInside(tempBounds))
							{
								node->CreateChild(i, tempBounds);
								// every newly created node goes into nodes array
								_nodes.Add(node->Children[i]);
								node = node->Children[i];
								childFound = true;
								break;
							}
						}
						else
						{
							if (element->GetBounds().IsInside(node->Children[i]->Box))
							{
								node = node->Children[i];
								childFound = true;
								break;
							}
						}
					}

					if (childFound)
					{
						childFound = false;
						continue;
					}
				}

				// nope. So let's leave this object here. Add node to nodes and object-node pair to both maps
				// also register for transform change

				node->Elements.Add(element);
				return node;

				break;
			}

			return nullptr;
		}

		inline void OctTree::AddRendererToRenderSet(gom::RenderSet & renderSet, memoryManagement::Handle<Renderer> renderer) const
		{
			renderSet.GameObjectsPerID.Add(renderer->GetOwner()->GetUniqueID(), renderer->GetOwner());

			for (MSize i = 0; i < renderer->GetMaterialCount(); ++i)
			{
				RenderSet::RendererIndexPair rp(renderer.GetPointer(), i);

				// add to flat collection, either transparent or opaque
				if (rp.RendererPtr->GetMaterial(i)->GetAlphaMode() == MMaterial::AlphaMode::MODE_OPAQUE)
				{
					renderSet.RenderersOpaqueFrontToBack.Add(rp);

					// add to render set based on first shader, then material
					// assuming any existing array is not empty
					// assuming matching materials also have matching shaders
					bool added(false);

					MArray<MArray<MArray<RenderSet::RendererIndexPair>>>& perShader = renderSet.RenderersOpaqueGrouped;
					for (MSize ji = 0; ji < perShader.GetSize(); ++ji)
					{
						MArray<MArray<RenderSet::RendererIndexPair>>& perMaterial = perShader[ji];

						if (perMaterial[0][0].RendererPtr->GetMaterial(perMaterial[0][0].MaterialIndex)->GetShader() !=
							rp.RendererPtr->GetMaterial(rp.MaterialIndex)->GetShader())
						{
							continue;
						}

						for (MSize jj = 0; jj < perShader.GetSize(); ++jj)
						{
							MArray<RenderSet::RendererIndexPair>& perRenderer = perMaterial[jj];
							if (perRenderer[0].RendererPtr->GetMaterial(perRenderer[0].MaterialIndex) == rp.RendererPtr->GetMaterial(rp.MaterialIndex))
							{
								added = true;
								perShader[ji][jj].Add(rp);
								break;
							}
						}

						// matching material not found but this is the correct group of shaders
						// create new material group and break
						if (!added)
						{
							MArray<RenderSet::RendererIndexPair>& newMaterialGroup = perMaterial.AddCreate();
							newMaterialGroup.Add(rp);
							added = true;
						}
						
						break;
					}

					// matching shader or material not found, adding as an element with new shader group with new material group
					if (!added)
					{
						MArray<MArray<RenderSet::RendererIndexPair>>& newShaderGroup = perShader.AddCreate();
						MArray<RenderSet::RendererIndexPair>& newMaterialGroup = newShaderGroup.AddCreate();
						newMaterialGroup.Add(rp);
						added = true;
					}
				}
				else
				{
					renderSet.RenderersTransparentBackToFront.Add(rp);
				}
			}
		}

		void OctTree::OnElementTransformChanged(memoryManagement::Handle<Transform> transform)
		{
			NodeElementPair ne;
			_mapTransformToNode.TryGetValue(transform, &ne);
			ME_ASSERT_S(ne.NodePtr != nullptr);

			if (ne.Element->GetForcePlaceAtRoot())
				return;

			// check if still fits in current node's bounds
			// nope. Because there might be a situation when element didn't fit into children before and it does now.
			//if (ne.Element->GetBounds().IsInside(ne.NodePtr->Box))
			//{
			//	return;
			//}

			// nope, so move upper until a fitting node is found.
			// then, perform adding, as usual
			// if reached root, put it in there and don't give a shit.

			const BoundsBox& bb = ne.Element->GetBounds();

			ne.NodePtr->Elements.Remove(ne.Element);

			Node* node = ne.NodePtr != _root ? ne.NodePtr->Parent : _root;
			while (node != _root && !bb.IsInside(node->Box))
				node = node->Parent;

			Node* changedNode = AddInternal(ne.Element, node);

			// update transform-nodeelement map if necessary
			if (changedNode != ne.NodePtr)
			{
				_mapTransformToNode.Remove(transform);
				_mapTransformToNode.Add(transform, NodeElementPair(ne.Element, changedNode));
			}
		}

		void OctTreeElement::RegisterComponent()
		{
			SceneManager::GetInstance()->GetScene()->RegisterOctTreeElement(static_cast<Handle<OctTreeElement>>(_this));
		}

		void OctTreeElement::DeregisterComponent()
		{
			SceneManager::GetInstance()->GetScene()->DeregisterOctTreeElement(static_cast<Handle<OctTreeElement>>(_this));
		}

		void OctTreeElement::UpdateBoundsWorld(memoryManagement::Handle<Transform> transform)
		{
			_bounds.UpdateToWorld(transform);
		}
}
}