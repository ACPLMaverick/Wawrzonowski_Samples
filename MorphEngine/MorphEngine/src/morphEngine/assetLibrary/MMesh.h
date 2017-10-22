#pragma once

#include "MAsset.h"
#include "MSkeleton.h"
#include "utility/MVector.h"
#include "renderer/device/GraphicsDevice.h"
#include <algorithm>
#include "utility/MStack.h"
#include "core/Delegate.h"

namespace fbxsdk
{
	class FbxNode;
	class FbxMesh;
	class FbxScene;
	template <class T> class FbxLayerElementTemplate;
	class FbxVector4;
	class FbxVector2;
	class FbxAnimStack;
}

namespace morphEngine
{
	namespace renderer
	{
		class BoundsBox;
		class BoundsSphere;
	}

	namespace assetLibrary
	{
		//Enum indicating the type of mesh (skeletal or static)
		enum class MeshType : MUint8
		{
			STATIC,
			SKELETAL,
			UNKNOWN
		};

		//Header for MMesh asset file
		struct MMeshHeader
		{
		public:
			MSize VerticesCount;
			MSize IndicesCount;
			MSize SubmeshesCount;
			MeshType Type;
			MSize SkeletonPathLength;
			utility::MString SkeletonPath;

			MSize Load(MUint8* data);
			bool SaveAppend(resourceManagement::fileSystem::File& file) const;
		};

		class MAnimationClip;
		class MSkeleton;

		//Class for mesh asset
		class MMesh : public MAsset
		{
			friend class resourceManagement::ResourceManager;
			friend class MAnimationClip;
			friend class MSkeleton;
		protected:

			struct Triangle
			{
				union
				{
					struct
					{
						MUint16 Va, Vb, Vc;
						MUint16 Na, Nb, Nc;
						MUint16 Ua, Ub, Uc;
					};
					MUint16 tab[3][3];
					MUint16 tabFlat[9];
				};

				inline bool operator==(const Triangle& rhs) const
				{
					for (MSize i = 0; i < 9; ++i)
					{
						if (tabFlat[i] != rhs.tabFlat[i])
						{
							return false;
						}
					}
					return true;
				}

				inline bool operator!=(const Triangle& rhs) const
				{
					return !operator==(rhs);
				}
			};

			struct BoneIndex
			{
				MInt32 A, B, C, D;

				inline BoneIndex() :
					A(0),
					B(0),
					C(0),
					D(0)
				{
				}

				inline BoneIndex(MInt32 a, MInt32 b, MInt32 c, MInt32 d) :
					A(a),
					B(b),
					C(c),
					D(d)
				{
				}

				inline MInt32& operator[](MSize index)
				{
					return reinterpret_cast<MInt32*>(&A)[index];
				}

				inline MInt32 operator[](MSize index) const
				{
					return reinterpret_cast<const MInt32*>(&A)[index];
				}

				inline bool operator==(const BoneIndex& rhs) const
				{
					return A == rhs.A && B == rhs.B && C == rhs.C && D == rhs.D;
				}

				inline bool operator!=(const BoneIndex& rhs) const
				{
					return !operator==(rhs);
				}
			};

			//typedef utility::MVector4 BoneIndex;
			typedef utility::MVector4 BoneWeight;
			// This needs to keep track of newly added (copied) vertices aside from main vertices.
			struct IndexLookupTable
			{
				utility::MArray<utility::MArray<MUint16>> Indices;
				utility::MArray<MSize> SubmeshPerVertex;

				inline IndexLookupTable() : Indices(16), SubmeshPerVertex(16) { }
				inline IndexLookupTable(MSize lutSize) : Indices(lutSize), SubmeshPerVertex(lutSize) { }
			};

			struct TempSkeletonData
			{
				struct Joint
				{
					fbxsdk::FbxNode* Node = nullptr;
					MInt32 ParentIndex = -1;
					utility::MString Name;
					utility::MMatrix GlobalBindposeInverse;
					utility::MMatrix WorldTransform;
					utility::MArray<MInt32> Children;
					bool TakesPartInSkeleton = false;

					inline Joint() : Children(4)
					{

					}

					inline Joint(const Joint& c) :
						Node(c.Node),
						ParentIndex(c.ParentIndex),
						Name(c.Name),
						GlobalBindposeInverse(c.GlobalBindposeInverse),
						WorldTransform(c.WorldTransform),
						Children(c.Children),
						TakesPartInSkeleton(c.TakesPartInSkeleton)
					{

					}

					inline ~Joint()
					{
						Children.Shutdown();
					}

					inline Joint& operator=(const Joint& c)
					{
						Node = c.Node;
						ParentIndex = c.ParentIndex;
						Name = c.Name;
						GlobalBindposeInverse = c.GlobalBindposeInverse;
						WorldTransform = c.WorldTransform;
						Children = c.Children;
						TakesPartInSkeleton = c.TakesPartInSkeleton;
					}

					inline bool operator==(const Joint& other) const { return ParentIndex == other.ParentIndex && Name == other.Name && TakesPartInSkeleton == other.TakesPartInSkeleton; }
					inline bool operator!=(const Joint& other) const { return !operator==(other); }
				};

				struct Anim
				{
					// keys are in seconds
					utility::MArray<MFloat32> Keys;
					// for each key -> for each joint
					utility::MArray<utility::MArray<utility::MMatrix>> LocalTransforms;
					fbxsdk::FbxAnimStack* Stack;
					MFloat32 Length;

					Anim() : 
						LocalTransforms(64)
					{

					}

					~Anim()
					{
						Keys.Shutdown();
						for (MSize i = 0; i < LocalTransforms.GetSize(); ++i)
						{
							LocalTransforms[i].Shutdown();
						}
						LocalTransforms.Shutdown();
					}

					void RemoveDuplicateKeys()
					{
						utility::MArray<MFloat32> UniqueKeys;
						for (MSize i = 0; i < Keys.GetSize(); ++i)
						{
							bool unique = true;
							for (MSize j = 0; j < UniqueKeys.GetSize(); ++j)
							{
								if (Keys[i] == UniqueKeys[j])
								{
									unique = false;
									break;
								}
							}

							if (unique)
							{
								UniqueKeys.Add(Keys[i]);
							}
						}
						Keys = UniqueKeys;
					}
				};

				utility::MArray<Joint> Joints;
				utility::MDictionary<utility::MString, Anim*> Anims;

				inline TempSkeletonData()/* : 
					Joints(96)*/
				{

				}

				inline ~TempSkeletonData()
				{
					for (auto it = Joints.GetBegin(); it.IsValid(); ++it)
					{
						(*it).Name.Shutdown();
						(*it).Children.Shutdown();
					}

					for (auto it = Anims.GetBegin(); it.IsValid(); ++it)
					{
						(*it).GetValue()->Keys.Shutdown();
						for (MSize i = 0; i < (*it).GetValue()->LocalTransforms.GetSize(); ++i)
						{
							(*it).GetValue()->LocalTransforms[i].Shutdown();
						}
						(*it).GetValue()->LocalTransforms.Shutdown();
						delete (*it).GetValue();
					}

					Joints.Shutdown();
					Anims.Shutdown();
				}
			};

			MMeshHeader _header;
			utility::MArray<utility::MVector3> _vertices;
			utility::MArray<utility::MVector3> _normals;
			utility::MArray<utility::MVector2> _uvs;
			// tangents
			// binormals
			utility::MArray<BoneWeight> _boneWeights;
			utility::MArray<BoneIndex> _boneIndices;
			utility::MArray<MUint16> _indices;
			utility::MArray<MSize> _submeshes;

			MSkeleton* _skeleton = nullptr;

			renderer::device::VertexBufferResource* _resVertices = nullptr;
			renderer::device::VertexBufferResource* _resNormals = nullptr;
			renderer::device::VertexBufferResource* _resUvs = nullptr;
			renderer::device::VertexBufferResource* _resBoneWeights = nullptr;
			renderer::device::VertexBufferResource* _resBoneIndices = nullptr;
			renderer::device::IndexBufferResource* _resIndices = nullptr;

		protected:
			//Loads OBJ from file
			bool LoadOBJ(resourceManagement::fileSystem::File& file);
			//Loads FBX
			bool LoadFBX(resourceManagement::fileSystem::File& file);
			//Loads MAsset (MMesh) from file
			bool LoadMMesh(resourceManagement::fileSystem::File& file);
			//Saves data to given .masset file
			bool SaveToMMesh(resourceManagement::fileSystem::File& file) const;

			//Initialize GPU buffer resources
			void InitBuffers();
			inline void ReinitBuffers(renderer::device::BufferAccessMode bmMode = renderer::device::BufferAccessMode::NONE);

			inline void LoadVertexIntoBuffers(IndexLookupTable& lut, utility::MVector3 position, utility::MVector3 normal, utility::MVector2 uv, MUint16 index);
			inline void ReadFbxMeshVec3(fbxsdk::FbxMesh* mesh, fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector4>* elem, MInt32 controlPointIndex, MInt32 vertexIndex, utility::MVector3& outData);
			inline void ReadFbxMeshVec2(fbxsdk::FbxMesh* mesh, fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector2>* elem, MInt32 controlPointIndex, MInt32 vertexIndex, utility::MVector2& outData);
			
			inline fbxsdk::FbxMesh* LoadFbxMesh(IndexLookupTable& lut, fbxsdk::FbxScene* scene, fbxsdk::FbxNode* entryNode);

			inline bool LoadFbxSkeletonAndAnimations(const utility::MString& fileName, IndexLookupTable* lut, fbxsdk::FbxScene* scene, 
				fbxsdk::FbxMesh* combinedMesh, utility::MArray<fbxsdk::FbxMesh*> meshes, fbxsdk::FbxNode* entryNode);

			static bool LoadFbxSkeletonAndAnimations_Intenral(MMesh* mInst, IndexLookupTable* lut, fbxsdk::FbxScene* scene, 
				fbxsdk::FbxMesh* combinedMesh, utility::MArray<fbxsdk::FbxMesh*> meshes, fbxsdk::FbxNode* entryNode, MSkeleton** outSkeleton, utility::MArray<MAnimationClip*>& outAnimations);
			static void ProcessSkeletonHierarchyRecursively(TempSkeletonData& td, fbxsdk::FbxNode* inNode, MInt32 myIndex, MInt32 inParentIndex);
			static void MakeGlobalTransform(TempSkeletonData& td, utility::MArray<utility::MMatrix>& transforms, MInt32 joint, 
				const utility::MMatrix& parentTransform);

			virtual void Shutdown() override;

		public:

#pragma region EventsPublic

			core::Event<void, MMesh*> EvtVerticesChanged;

#pragma endregion

			//Default constructor
			inline MMesh() : MAsset()
			{
				_type = MAssetType::MESH;
			}
			//Constructs MMesh from given data (i.e. file bytes array)
			inline MMesh(const utility::MFixedArray<MUint8>& data) : MAsset(MAssetType::MESH, data)
			{
			}
			//Copy constructor
			inline MMesh(const MMesh& c) : MAsset(c),
				_header(c._header),
				_vertices(c._vertices),
				_normals(c._normals),
				_uvs(c._uvs),
				_indices(c._indices),
				_boneWeights(c._boneWeights),
				_boneIndices(c._boneIndices),
				_submeshes(c._submeshes),
				_skeleton(c._skeleton)
			{
			}

			virtual ~MMesh();

#pragma region Rendering Pipeline

			inline const renderer::device::VertexBufferResource* GetVertexPositionsResource() const { return _resVertices; }
			inline const renderer::device::VertexBufferResource* GetVertexNormalsResource() const { return _resNormals; }
			inline const renderer::device::VertexBufferResource* GetVertexUVsResource() const { return _resUvs; }
			inline const renderer::device::VertexBufferResource* GetVertexBoneWeightsResource() const { return _resBoneWeights; }
			inline const renderer::device::VertexBufferResource* GetVertexBoneIndicesResource() const { return _resBoneIndices; }
			inline const renderer::device::IndexBufferResource* GetIndicesResouce() const { return _resIndices; }

			inline MSize GetVertexCount(MSize submeshID = 0) const
			{
				return 0;	// TODO:Implement
			}

			inline MSize GetIndexCount(MSize submeshID = 0) const 
			{ 
				if(submeshID == (GetSubmeshCount() - 1))
				{
					return _indices.GetSize() - _submeshes[submeshID];
				}
				else
				{
					return _submeshes[submeshID + 1] - _submeshes[submeshID];
				}
			}
			inline MSize GetSubmeshCount() const { return _submeshes.GetSize(); }
			inline MSize GetStartIndex(MSize submeshID = 0) const { ME_ASSERT_S(submeshID < _submeshes.GetSize()); return _submeshes[submeshID]; }
			// This is unimplemented yet and returns 0.
			inline MSize GetStartVertex() const { return 0; }
			inline bool GetIsSkeletal() const { return !_boneWeights.IsEmpty(); }
			inline MSkeleton* GetSkeleton() const { return _skeleton; }

#pragma endregion

			//Returns info of the MMesh (verts count, indices count or mesh type)
			inline const MMeshHeader& GetInfo() const
			{
				return _header;
			}

			//Loads data from given bytes array (NOTE: data should contain whole .masset file data with MAssetType info)
			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data);

			//Loads data from given bytes array (NOTE: data should contain whole .masset file data with MAssetType info)
			virtual bool LoadFromData(const utility::MArray<MUint8>& data);

			//Loads data from file depending on extension of given fileName. NOTE: binaryFile MUST BE opened when passing to this function
			inline virtual bool LoadFromFile(resourceManagement::fileSystem::File& file)
			{
				if(!file.IsOpened())
				{
					return false;
				}
				SetPath(file.GetPath());

				utility::MString ext;
				GetExtension(file, ext);

				if(ext == "obj")
				{
					return LoadOBJ(file);
				}
				else if(ext == "masset")
				{
					return LoadMMesh(file);
				}
				else if(ext == "fbx")
				{
					return LoadFBX(file);
				}

				return true;
			}

			//Saves MMesh to given file
			inline virtual bool SaveToFile(resourceManagement::fileSystem::File& file) const
			{
				if(!file.IsOpened())
				{
					return false;
				}

				return SaveToMMesh(file);
			}

			inline virtual void Initialize()
			{
				InitBuffers();
			}

			// Buffer modification functions

			void UpdateVerticesResource();
			void UpdateNormalsResource();
			void UpdateUVsResource();
			void UpdateIndicesResource();
			void UpdateBoneWeightsResource();
			void UpdateBoneIndicesResource();

			inline utility::MArray<utility::MVector3>& GetVertexPositionsBuffer() { return _vertices; }
			inline utility::MArray<utility::MVector3>& GetVertexNormalsBuffer() { return _normals; }
			inline utility::MArray<utility::MVector2>& GetVertexUVsBuffer() { return _uvs; }
			inline utility::MArray<BoneWeight>& GetVertexBoneWeightsBuffer() { return _boneWeights; }
			inline utility::MArray<BoneIndex>& GetVertexBoneIndicesBuffer() { return _boneIndices; }
			inline utility::MArray<MUint16>& GetIndicesBuffer() { return _indices; }

			void UpdateBounds(renderer::BoundsBox& bounds);
			void UpdateBounds(renderer::BoundsSphere& bounds);

		};
	}
}