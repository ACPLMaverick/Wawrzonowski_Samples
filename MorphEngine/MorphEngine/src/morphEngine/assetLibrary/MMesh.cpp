#include "MMesh.h"
#include "renderer/RenderingManager.h"
#include "resourceManagement/ResourceManager.h"
#include "debugging/Debug.h"
#include "MAnimationClip.h"
#include "renderer/BoundsBox.h"
#include "renderer/BoundsSphere.h"
using namespace morphEngine::renderer;
using namespace morphEngine::renderer::device;
using namespace morphEngine::utility;

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader/tiny_obj_loader.h>

#include "FbxHelperDefines.h"

using namespace morphEngine::debugging;
using namespace morphEngine::animation;

#pragma region Helper classes and definitions

struct membuf : std::streambuf {
	membuf(char const* base, size_t size) {
		char* p(const_cast<char*>(base));
		this->setg(p, p, p + size);
	}
};
struct imemstream : virtual membuf, std::istream {
	imemstream(char const* base, size_t size)
		: membuf(base, size)
		, std::istream(static_cast<std::streambuf*>(this)) {
	}
};

#define Char2Int(x) (x) - '0'

#pragma endregion

morphEngine::assetLibrary::MMesh::~MMesh()
{
	Shutdown();
}

bool morphEngine::assetLibrary::MMesh::LoadOBJ(resourceManagement::fileSystem::File& file)
{
	_header.Type = MeshType::STATIC;

	utility::MFixedArray<MUint8> data;
	file.Read(data);

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	imemstream streamFromData(reinterpret_cast<const char*>(data.GetDataPointer()), data.GetSize());
	bool ret = tinyobj::LoadObj(&attrib, &shapes, nullptr, nullptr, &streamFromData);

	if (!ret)
	{
		return false;
	}

	MSize vCount = attrib.vertices.size() / 3;
	_vertices.Resize(vCount);
	_normals.Resize(vCount);
	_uvs.Resize(vCount);

	IndexLookupTable lut(vCount);

	for (auto it = _normals.GetBegin(); it.IsValid(); ++it)
	{
		(*it) = MVector3::Zero;
	}

	MSize submeshIndex = 0;
	for (auto it = shapes.begin(); it != shapes.end(); ++it)
	{
		_submeshes.Add(submeshIndex);
		for (auto jt = (*it).mesh.indices.begin(); jt != (*it).mesh.indices.end(); ++jt)
		{
			MUint16 index = (*jt).vertex_index;
			MVector3 normal = MVector3(attrib.normals[(*jt).normal_index * 3], attrib.normals[(*jt).normal_index * 3 + 1], attrib.normals[(*jt).normal_index * 3 + 2]);
			MVector2 uv = MVector2(attrib.texcoords[(*jt).texcoord_index * 2], attrib.texcoords[(*jt).texcoord_index * 2 + 1]);
			MVector3 position = MVector3(attrib.vertices[(*jt).vertex_index * 3], attrib.vertices[(*jt).vertex_index * 3 + 1], attrib.vertices[(*jt).vertex_index * 3 + 2]);

			LoadVertexIntoBuffers(lut, position, normal, uv, index);

			++submeshIndex;
		}
	}

	for (auto it = _indices.GetBegin(); it.IsValid(); it += 3)
	{
		MUint16 tmp = *(it + 2);
		*(it + 2) = (*it);
		(*it) = tmp;
	}

	for (auto it = _normals.GetBegin(); it.IsValid(); ++it)
	{
		(*it).Normalize();
	}

	_header.IndicesCount = _indices.GetSize();
	_header.VerticesCount = _vertices.GetSize();
	_header.SubmeshesCount = _submeshes.GetSize();

	return true;
}

bool morphEngine::assetLibrary::MMesh::LoadMMesh(resourceManagement::fileSystem::File& file)
{
	if(!file.IsOpened())
	{
		return false;
	}

	if (file.GetSize() < sizeof(MAssetType) + sizeof(MMeshHeader))
	{
		return false;
	}

	utility::MFixedArray<MUint8> readData;
	file.Read(readData);

	return LoadFromData(readData);
}

bool morphEngine::assetLibrary::MMesh::LoadFBX(resourceManagement::fileSystem::File& file)
{
	MFixedArray<MUint8> bytes(file.GetSize());
	file.Read(bytes.GetDataPointer(), 0, file.GetSize());
	FbxManager* manager = resourceManagement::ResourceManager::GetInstance()->GetFbxManager();
	FbxScene* scene = LoadSceneFromBuffer(manager, bytes);

	FbxGeometryConverter converter(manager);

	converter.SplitMeshesPerMaterial(scene, true);
	converter.Triangulate(scene, true);

	MInt32 nodeCount = scene->GetNodeCount();
	_header.VerticesCount = 0;
	_header.IndicesCount = 0;
	_header.SubmeshesCount = 0;

	IndexLookupTable lut;

	utility::MArray<fbxsdk::FbxMesh*> meshes;

	// try to load mesh
	for (MInt32 i = 0; i < nodeCount; ++i)
	{
		FbxMesh* m = LoadFbxMesh(lut, scene, scene->GetNode(i));
		if (m != nullptr)
		{
			meshes.Add(m);
		}
	}

	if (meshes.GetSize() == 0)
		return false;

	//FbxNode* combined = converter.MergeMeshes(meshNodes, "Janusz", scene);

	// try to load skeleton
	for (MInt32 i = 0; i < nodeCount; ++i)
	{
		if (LoadFbxSkeletonAndAnimations(file.GetPath(), &lut, scene, meshes[0], meshes, scene->GetNode(i))) break;
	}

	// fix flipped normals
	//if (_skeleton != nullptr)
	//{
	//	MMatrix tr(MMatrix::Scale(-1.0f, 1.0f, 1.0f));
	//	for (MSize i = 0; i < _vertices.GetSize(); ++i)
	//	{
	//		_vertices[i] = tr * MVector4(_vertices[i], 1.0f);
	//		_normals[i] = tr * MVector4(_normals[i], 0.0f);
	//	}
	//}

	scene->Destroy(true);

	return true;
}

bool morphEngine::assetLibrary::MMesh::SaveToMMesh(resourceManagement::fileSystem::File& file) const
{
	if(!file.IsOpened())
	{
		return false;
	}

	file.WriteAppend(reinterpret_cast<const MUint8*>(&_type), sizeof(MAssetType));

	_header.SaveAppend(file);

	utility::MFixedArray<MUint8> verticesBytes(sizeof(utility::MVector3) * _vertices.GetSize());
	memcpy(verticesBytes.GetDataPointer(), _vertices.GetDataPointer(), sizeof(utility::MVector3) * _vertices.GetSize());
	file.WriteAppend(verticesBytes);

	utility::MFixedArray<MUint8> normalsBytes(sizeof(utility::MVector3) * _normals.GetSize());
	memcpy(normalsBytes.GetDataPointer(), _normals.GetDataPointer(), sizeof(utility::MVector3) * _normals.GetSize());
	file.WriteAppend(normalsBytes);

	utility::MFixedArray<MUint8> uvsBytes(sizeof(utility::MVector2) * _uvs.GetSize());
	memcpy(uvsBytes.GetDataPointer(), _uvs.GetDataPointer(), sizeof(utility::MVector2) * _uvs.GetSize());
	file.WriteAppend(uvsBytes);

	if (GetIsSkeletal())
	{
		utility::MFixedArray<MUint8> weightBytes(sizeof(BoneWeight) * _boneWeights.GetSize());
		memcpy(weightBytes.GetDataPointer(), _boneWeights.GetDataPointer(), sizeof(BoneWeight) * _boneWeights.GetSize());
		file.WriteAppend(weightBytes);

		utility::MFixedArray<MUint8> boneIndexBytes(sizeof(BoneIndex) * _boneIndices.GetSize());
		memcpy(boneIndexBytes.GetDataPointer(), _boneIndices.GetDataPointer(), sizeof(BoneIndex) * _boneIndices.GetSize());
		file.WriteAppend(boneIndexBytes);
	}

	utility::MFixedArray<MUint8> indicesBytes(sizeof(MUint16) * _indices.GetSize());
	memcpy(indicesBytes.GetDataPointer(), _indices.GetDataPointer(), sizeof(MUint16) * _indices.GetSize());
	file.WriteAppend(indicesBytes);

	utility::MFixedArray<MUint8> submeshesBytes(sizeof(MSize) * _submeshes.GetSize());
	memcpy(submeshesBytes.GetDataPointer(), _submeshes.GetDataPointer(), sizeof(MSize) * _submeshes.GetSize());
	file.WriteAppend(submeshesBytes);

	return true;
}

void morphEngine::assetLibrary::MMesh::InitBuffers()
{
	_resVertices = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();
	_resNormals = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();
	_resUvs = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();
	_resIndices = RenderingManager::GetInstance()->GetDevice()->CreateIndexBufferInstance();

	_resVertices->Initialize(reinterpret_cast<MFloat32*>(_vertices.GetDataPointer()), sizeof(utility::MVector3), _vertices.GetSize());
	_resNormals->Initialize(reinterpret_cast<MFloat32*>(_normals.GetDataPointer()), sizeof(utility::MVector3), _normals.GetSize());
	_resUvs->Initialize(reinterpret_cast<MFloat32*>(_uvs.GetDataPointer()), sizeof(utility::MVector2), _uvs.GetSize());
	_resIndices->Initialize(reinterpret_cast<MUint16*>(_indices.GetDataPointer()), _indices.GetSize());

	if (GetIsSkeletal())
	{
		_resBoneWeights = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();
		_resBoneIndices = RenderingManager::GetInstance()->GetDevice()->CreateVertexBufferInstance();

		_resBoneWeights->Initialize(reinterpret_cast<MFloat32*>(_boneWeights.GetDataPointer()), sizeof(BoneWeight), _boneWeights.GetSize());
		_resBoneIndices->Initialize(reinterpret_cast<MFloat32*>(_boneIndices.GetDataPointer()), sizeof(BoneIndex), _boneIndices.GetSize());
	}
}

inline void morphEngine::assetLibrary::MMesh::ReinitBuffers(device::BufferAccessMode bmMode)
{
	_resVertices->Shutdown();
	_resNormals->Shutdown();
	_resUvs->Shutdown();
	_resIndices->Shutdown();

	if (GetIsSkeletal())
	{
		_resBoneWeights->Shutdown();
		_resBoneIndices->Shutdown();
	}

	_resVertices->Initialize(reinterpret_cast<MFloat32*>(_vertices.GetDataPointer()), sizeof(utility::MVector3), _vertices.GetSize(), bmMode);
	_resNormals->Initialize(reinterpret_cast<MFloat32*>(_normals.GetDataPointer()), sizeof(utility::MVector3), _normals.GetSize(), bmMode);
	_resUvs->Initialize(reinterpret_cast<MFloat32*>(_uvs.GetDataPointer()), sizeof(utility::MVector2), _uvs.GetSize(), bmMode);
	_resIndices->Initialize(reinterpret_cast<MUint16*>(_indices.GetDataPointer()), _indices.GetSize(), bmMode);

	if (GetIsSkeletal())
	{
		_resBoneWeights->Initialize(reinterpret_cast<MFloat32*>(_boneWeights.GetDataPointer()), sizeof(BoneWeight), _boneWeights.GetSize(), bmMode);
		_resBoneIndices->Initialize(reinterpret_cast<MFloat32*>(_boneIndices.GetDataPointer()), sizeof(BoneIndex), _boneIndices.GetSize(), bmMode);
	}
}

inline void morphEngine::assetLibrary::MMesh::LoadVertexIntoBuffers(IndexLookupTable& lut, utility::MVector3 position, utility::MVector3 normal, utility::MVector2 uv, MUint16 index)
{
	MArray<MUint16>& inds = lut.Indices[index];

	for (auto it = inds.GetBegin(); it.IsValid(); ++it)
	{
		// check if normal or uv are already assigned to this vertex
		if (_vertices[(*it)] == position && normal == _normals[(*it)] && uv == _uvs[(*it)])
		{
			// already has assigned normal and uv and does not need to be altered.
			_indices.Add((*it));
			return;
		}
		else if(_vertices[(*it)] == position)	// if already written to this position
		{
			// already has assigned normal and uv but they're different, so we need to create a new vertex with a new id
			MUint16 newIndex = static_cast<MUint16>(_vertices.GetSize());
			_vertices.Add(position);
			_normals.Add(normal);
			_uvs.Add(uv);
			_indices.Add(newIndex);

			// add entry for this main index
			inds.Add(newIndex);
			return;
		}
	}

	// has not assigned any normal or uv yet
	_indices.Add(index);
	_vertices[index] = position;
	_normals[index] = normal;
	_uvs[index] = uv;

	// this is ok
	inds.Add(index);
}

inline void morphEngine::assetLibrary::MMesh::ReadFbxMeshVec3(fbxsdk::FbxMesh * mesh, fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector4>* elem, MInt32 controlPointIndex, MInt32 vertexIndex, utility::MVector3 & outData)
{
	MInt32 finalIndex = 0;

	if (elem->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
	{
		if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eDirect)
		{
			finalIndex = controlPointIndex;
		}
		else if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
		{
			finalIndex = elem->GetIndexArray().GetAt(controlPointIndex);
		}
		else
		{
			ME_ASSERT(false, "Incompatible mesh data.");
		}
	}
	else if (elem->GetMappingMode() == FbxLayerElement::EMappingMode::eByPolygonVertex)
	{
		if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eDirect)
		{
			finalIndex = vertexIndex;
		}
		else if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
		{
			finalIndex = elem->GetIndexArray().GetAt(vertexIndex);
		}
		else
		{
			ME_ASSERT(false, "Incompatible mesh data.");
		}
	}
	else
	{
		ME_ASSERT(false, "Incompatible mesh data.");
	}

	outData.X = static_cast<MFloat32>(elem->GetDirectArray().GetAt(finalIndex).mData[0]);
	outData.Y = static_cast<MFloat32>(elem->GetDirectArray().GetAt(finalIndex).mData[1]);
	outData.Z = static_cast<MFloat32>(elem->GetDirectArray().GetAt(finalIndex).mData[2]);
}

inline void morphEngine::assetLibrary::MMesh::ReadFbxMeshVec2(fbxsdk::FbxMesh * mesh, fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector2>* elem, MInt32 controlPointIndex, MInt32 vertexIndex, utility::MVector2 & outData)
{
	MInt32 finalIndex = 0;

	if (elem->GetMappingMode() == FbxLayerElement::EMappingMode::eByControlPoint)
	{
		if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eDirect)
		{
			finalIndex = controlPointIndex;
		}
		else if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
		{
			finalIndex = elem->GetIndexArray().GetAt(controlPointIndex);
		}
		else
		{
			ME_ASSERT(false, "Incompatible mesh data.");
		}
	}
	else if (elem->GetMappingMode() == FbxLayerElement::EMappingMode::eByPolygonVertex)
	{
		if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eDirect)
		{
			finalIndex = vertexIndex;
		}
		else if (elem->GetReferenceMode() == FbxLayerElement::EReferenceMode::eIndexToDirect)
		{
			finalIndex = elem->GetIndexArray().GetAt(vertexIndex);
		}
		else
		{
			ME_ASSERT(false, "Incompatible mesh data.");
		}
	}
	else
	{
		ME_ASSERT(false, "Incompatible mesh data.");
	}

	outData.X = static_cast<MFloat32>(elem->GetDirectArray().GetAt(finalIndex).mData[0]);
	outData.Y = static_cast<MFloat32>(elem->GetDirectArray().GetAt(finalIndex).mData[1]);
}

inline FbxMesh* morphEngine::assetLibrary::MMesh::LoadFbxMesh(IndexLookupTable& lut, fbxsdk::FbxScene* scene, fbxsdk::FbxNode * entryNode)
{
	FbxMesh* mesh = entryNode->GetMesh();
	if (mesh != nullptr)
	{
		_submeshes.Add(_header.IndicesCount);
		++_header.SubmeshesCount;

		lut.SubmeshPerVertex.Add(_header.VerticesCount);
		lut.Indices.ResizeWithReinit(_header.IndicesCount + mesh->GetControlPointsCount());

		FbxVector4* meshVerts = mesh->GetControlPoints();

		FbxLayerElementNormal* leNormal = mesh->GetElementNormal();
		FbxLayerElementArrayTemplate<FbxVector4>& normalArray = leNormal->GetDirectArray();		// ALWAYS ASSIGN TO REFERENCE OR IT'LL FUCK UP YOUR LIFE
		MInt32 normalArrayCount = normalArray.GetCount();

		FbxLayerElementUV* leUV = mesh->GetElementUV();
		FbxLayerElementArrayTemplate<FbxVector2>& uvArray = leUV->GetDirectArray();
		MInt32 uvArrayCount = uvArray.GetCount();

		auto indexArray = mesh->GetPolygonVertices();
		MInt32 indexArrayCount = mesh->GetPolygonVertexCount();

		// get global transform matrix
		MMatrix worldTransform = FbxMatToMatrix(entryNode->EvaluateGlobalTransform());
		MMatrix worldTransformInvTrans = worldTransform.Inverted().Transposed();

		MSize lastVerticesSize = _vertices.GetSize();

		_vertices.ResizeWithReinit(_vertices.GetSize() + mesh->GetControlPointsCount());
		_normals.ResizeWithReinit(_normals.GetSize() + mesh->GetControlPointsCount());
		_uvs.ResizeWithReinit(_uvs.GetSize() + mesh->GetControlPointsCount());

		// for each index
		for (MInt32 j = 0; j < indexArrayCount; ++j)
		{
			MInt32 index = indexArray[j];
			MInt32 globalIndex = index + static_cast<MInt32>(_header.VerticesCount);

			MInt32 indexPolygon = index / 3;
			MInt32 positionInPolygon = index % 3;
			MVector3 position = FbxVec4ToVector3(meshVerts[index]), normal;
			MVector2 uv;
			ReadFbxMeshVec3(mesh, leNormal, index, j, normal);
			ReadFbxMeshVec2(mesh, leUV, index, j, uv);

			LoadVertexIntoBuffers(lut, position, normal, uv, globalIndex);
		}

		_header.VerticesCount = _vertices.GetSize();

		for (MSize j = lastVerticesSize; j < _header.VerticesCount; ++j)
		{
			_vertices[j] = worldTransform * MVector4(_vertices[j], 1.0f);
			_normals[j] = worldTransformInvTrans * MVector4(_normals[j], 0.0f);
			//_normals[j] = MVector3(-_normals[j].X, _normals[j].Y, _normals[j].Z);
		}

		MSize newIndicesCount = _header.IndicesCount + indexArrayCount;

		_header.IndicesCount = newIndicesCount;
	}

	return mesh;
}

inline bool morphEngine::assetLibrary::MMesh::LoadFbxSkeletonAndAnimations(const utility::MString& fileName, IndexLookupTable* lut, 
	fbxsdk::FbxScene* scene, fbxsdk::FbxMesh* combinedMesh, utility::MArray<fbxsdk::FbxMesh*> meshes, fbxsdk::FbxNode* entryNode)
{
	if (entryNode->GetNodeAttribute() && entryNode->GetNodeAttribute()->GetAttributeType() && entryNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		MArray<MAnimationClip*> newClips;
		if (LoadFbxSkeletonAndAnimations_Intenral(this, lut, scene, combinedMesh, meshes, entryNode, &_skeleton, newClips))
		{
			MArray<char> splitters;
			MArray<MString> split;
			splitters.Add('.');
			MString::Split(fileName, splitters, split);
			MString skeletonName = split[0];
			split.Clear();
			splitters.Clear();
			splitters.Add('\\');
			MString::Split(skeletonName, splitters, split);
			skeletonName = "";
			for (MSize i = 1; i < split.GetSize(); ++i)
				skeletonName += "\\" + split[i];

			skeletonName += "_skeleton.masset";

			_skeleton->SetPath(skeletonName);
			resourceManagement::ResourceManager::GetInstance()->_skeletonCache.AddAsset(skeletonName, _skeleton);

			_header.Type = MeshType::SKELETAL;
			_header.SkeletonPath = skeletonName;
			_header.SkeletonPathLength = skeletonName.Length();

			MArray<MString> nameSplit;
			MArray<char> charsToSplit;
			charsToSplit.Add('\\');
			charsToSplit.Add('.');
			fileName.Split(fileName, charsToSplit, nameSplit);
			MString animNameBase = "Animations/";
			for (MSize i = 2; i < nameSplit.GetSize() - 1; ++i)
			{
				animNameBase += nameSplit[i] + "/";
			}

			for (auto it = newClips.GetBegin(); it.IsValid(); ++it)
			{
				(*it)->_skeletonPath = _skeleton->GetPath();
				MString animNameFinal = animNameBase + (*it)->_name + ".masset";
				animNameFinal.Replace('|', '_');
				(*it)->SetPath(animNameFinal);
				debugging::Debug::Log(debugging::ELogType::LOG, animNameFinal);
				resourceManagement::ResourceManager::GetInstance()->_animationClipCache.AddAsset(animNameFinal, (*it));
			}

			return true;
		}
		else return false;
	}
	else
	{
		return false;
	}
}

bool morphEngine::assetLibrary::MMesh::LoadFbxSkeletonAndAnimations_Intenral(MMesh* mInst, IndexLookupTable * lut, 
	fbxsdk::FbxScene * scene, fbxsdk::FbxMesh* combinedMesh, utility::MArray<fbxsdk::FbxMesh*> meshes, fbxsdk::FbxNode * entryNode, MSkeleton** outSkeleton, MArray<MAnimationClip*>& outAnimations)
{
	*outSkeleton = new MSkeleton();

	if (mInst != nullptr)
	{
		mInst->_boneWeights.Resize(mInst->_vertices.GetSize());
		mInst->_boneIndices.Resize(mInst->_vertices.GetSize());
		mInst->_boneWeights.Fill(MVector4(1.0f, 0.0f, 0.0f, 0.0f));
		mInst->_boneIndices.Fill(BoneIndex());
	}

	TempSkeletonData td;

	ProcessSkeletonHierarchyRecursively(td, entryNode, 0, -1);

	//for (MSize meshIndex = 0; meshIndex < meshes.GetSize(); ++meshIndex)
	//{

	//}
	FbxNode* meshNode = combinedMesh->GetNode();

	FbxAMatrix geometryTransform(meshNode->GetGeometricTranslation(FbxNode::eSourcePivot),
		meshNode->GetGeometricRotation(FbxNode::eSourcePivot), meshNode->GetGeometricScaling(FbxNode::eSourcePivot));

	MInt32 deformersCount = combinedMesh->GetDeformerCount();
	for (MInt32 deformerIndex = 0; deformerIndex < deformersCount; ++deformerIndex)
	{
		FbxSkin* skin = reinterpret_cast<FbxSkin*>(combinedMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (skin == nullptr)
			continue;

		MInt32 clusterCount = skin->GetClusterCount();
		for (MInt32 clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			// ====== MODEL SPACE TO BONE SPACE TRANSFORM MATRIX

			FbxCluster* cluster = skin->GetCluster(clusterIndex);
			MString jointName = cluster->GetLink()->GetName();

			MInt32 jointIndex = 0;
			for (MInt32 i = 0; i < td.Joints.GetSize(); ++i)
			{
				if (td.Joints[i].Name == jointName)
				{
					jointIndex = i;
					break;
				}
			}

			td.Joints[jointIndex].TakesPartInSkeleton = true;
			td.Joints[jointIndex].Node = cluster->GetLink();

			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;

			cluster->GetTransformMatrix(transformMatrix);
			cluster->GetTransformLinkMatrix(transformLinkMatrix);

			// Compute the initial position of the link relative to the reference.
			// This is bind pose of this joint
			td.Joints[jointIndex].GlobalBindposeInverse = FbxMatToMatrix(transformLinkMatrix.Inverse());
			td.Joints[jointIndex].WorldTransform = FbxMatToMatrix(transformLinkMatrix);

			// ====== Anim prep
			// Adding all keys' times, so I know in which moments to evaluate animation stack for all bones

			MInt32 animStackCount = scene->GetSrcObjectCount<FbxAnimStack>();

			for (MInt32 i = 0; i < animStackCount; ++i)
			{
				FbxAnimStack* stack = scene->GetSrcObject<FbxAnimStack>(i);
				FbxTakeInfo* takeInfo = scene->GetTakeInfo(stack->GetName());

				TempSkeletonData::Anim* anim;
				if (!td.Anims.TryGetValue(stack->GetName(), &anim))
				{
					anim = new TempSkeletonData::Anim();
					anim->Length = static_cast<MFloat32>(takeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble());
					anim->Stack = stack;
					td.Anims.Add(stack->GetName(), anim);
				}

				MInt32 animLayerCount = stack->GetMemberCount<FbxAnimLayer>();
				for (MInt32 j = 0; j < animLayerCount; ++j)
				{
					FbxAnimLayer* layer = stack->GetMember<FbxAnimLayer>(j);
					FbxAnimCurve* curves[3]{ nullptr, nullptr, nullptr };
					curves[0] = cluster->GetLink()->LclTranslation.GetCurve(layer);
					curves[1] = cluster->GetLink()->LclRotation.GetCurve(layer);
					curves[2] = cluster->GetLink()->LclScaling.GetCurve(layer);
					for (MInt32 k = 0; k < 3; ++k)
					{
						if (curves[k] != nullptr)
						{
							MInt32 curveKeys = curves[k]->KeyGetCount();
							for (MInt32 m = 0; m < curveKeys; ++m)
							{
								FbxAnimCurveKey key = curves[k]->KeyGet(m);
								MFloat32 secondsValue = static_cast<MFloat32>(key.GetTime().GetSecondDouble());
								anim->Keys.Add(secondsValue);
							}
						}
						else
						{
							continue;
						}
					}
				}
			}

		}
	}

	// ====== Setting up skeleton

	// Need to take into account that bone may not be taking part in skeleton :C
	for (auto it = td.Joints.GetBegin(); it.IsValid(); ++it)
	{
		TempSkeletonData::Joint& j = (*it);
		if (j.TakesPartInSkeleton)
		{
			if (j.ParentIndex == -1)
			{
				(*outSkeleton)->CreateBone(j.Name, j.GlobalBindposeInverse, j.WorldTransform, nullptr);
				debugging::Debug::Log(debugging::ELogType::LOG, "Null -> " + j.Name);
			}
			else
			{
				MInt32 pi = j.ParentIndex;
				while (!td.Joints[pi].TakesPartInSkeleton && pi != -1) pi = td.Joints[pi].ParentIndex;
				Bone* parent = pi != -1 ? (*outSkeleton)->GetBoneByName(td.Joints[pi].Name) : nullptr;
				debugging::Debug::Log(debugging::ELogType::LOG, td.Joints[pi].Name + " -> " + j.Name);
				(*outSkeleton)->CreateBone(j.Name, j.GlobalBindposeInverse, j.WorldTransform, parent);
			}
		}
	}


	if (mInst != nullptr)
	{
		// =========== Iterate through clusters once again to get proper weights and indices

		for (MSize meshIndex = 0; meshIndex < meshes.GetSize(); ++meshIndex)
		{
			FbxMesh* mesh = meshes[meshIndex];
			MFixedArray<MInt32> pwic(mInst->_vertices.GetSize());
			pwic.FillWithZeros();

			for (MInt32 deformerIndex = 0; deformerIndex < deformersCount; ++deformerIndex)
			{
				FbxSkin* skin = reinterpret_cast<FbxSkin*>(mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
				if (skin == nullptr)
					continue;

				MInt32 clusterCount = skin->GetClusterCount();
				for (MInt32 clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
				{
					FbxCluster* cluster = skin->GetCluster(clusterIndex);
					MString jointName = cluster->GetLink()->GetName();
					Bone* b = (*outSkeleton)->GetBoneByName(jointName);
					if (b != nullptr)
					{
						MInt32 jointIndex = static_cast<MInt32>(b->GetFlatIndex());

						MUint32 indexCount = cluster->GetControlPointIndicesCount();
						MInt32* indexArray = cluster->GetControlPointIndices();
						MDouble64* weightArray = cluster->GetControlPointWeights();

						for (MUint32 i = 0; i < indexCount; ++i)
						{
							MInt32 index = indexArray[i] + static_cast<MInt32>(lut->SubmeshPerVertex[meshIndex]);
							MFloat32 weight = static_cast<MFloat32>(weightArray[i]);
							for (auto it = lut->Indices[index].GetBegin(); it.IsValid(); ++it)
							{
								MUint16 vertexIndex = (*it);
								if (pwic[vertexIndex] < 4)
								{
									mInst->_boneWeights[vertexIndex][pwic[vertexIndex]] = weight;
									mInst->_boneIndices[vertexIndex][pwic[vertexIndex]] = jointIndex;
									pwic[vertexIndex] = pwic[vertexIndex] + 1;
								}
							}
						}
					}
				}
			}
		}
	}

	// ======= Normalize weights just in case because we dont have enough iterations
	for (auto it = mInst->_boneWeights.GetBegin(); it.IsValid(); ++it)
	{
		MFloat32 sum = 0;
		for (int i = 0; i < 4; ++i)
			sum += (*it)[i];
		(*it) = (*it) / sum;
	}

	// ====== Anim data gathering

	for (auto it = td.Anims.GetBegin(); it.IsValid(); ++it)
	{
		if ((*it).GetValue()->Keys.GetSize() < 1)
			continue;

		TempSkeletonData::Anim* anim = (*it).GetValue();
		anim->RemoveDuplicateKeys();
		anim->Keys.Sort(CompareAscending<MFloat32>);

		scene->SetCurrentAnimationStack(anim->Stack);
		FbxAnimEvaluator* evaluator = scene->GetAnimationEvaluator();

		// gather all transform matrices for all keys for all joints
		for (auto jt = anim->Keys.GetBegin(); jt.IsValid(); ++jt)
		{
			FbxTime keyTime;
			keyTime.SetSecondDouble(*jt);
			anim->LocalTransforms.Add(MArray<MMatrix>(td.Joints.GetSize()));
			MArray<MMatrix>& matrices = anim->LocalTransforms[anim->LocalTransforms.GetSize() - 1];
			for (auto kt = td.Joints.GetBegin(); kt.IsValid(); ++kt)
			{
				if ((*kt).Node == nullptr)
				{
					matrices.Add(MMatrix::Identity);
				}
				else
				{
					FbxAMatrix currentTransformOffset = entryNode->EvaluateGlobalTransform(keyTime) * geometryTransform;
					FbxAMatrix finalTransform = (*kt).Node->EvaluateGlobalTransform(keyTime);
					MMatrix fin = MMatrix::Scale(-1.0f, 1.0f, 1.0f) * FbxMatToMatrix(finalTransform);
					/*Debug::Log(ELogType::LOG, "Trans: " + MString::FromFloat(fin[0][3]) + ", "
						+ MString::FromFloat(fin[1][3]) + ", " + MString::FromFloat(fin[2][3]));*/
					matrices.Add(fin);
				}
			}
		}
	}

	//MMatrix scaleOnceAgain = MMatrix::Scale(-1.0f, 1.0f, 1.0f);
	//MMatrix scaleOnceAgainIT = scaleOnceAgain.Inverted().Transposed();
	//for (MSize i = 0; i < mInst->_header.VerticesCount; ++i)
	//{
	//	mInst->_vertices[i] = MVector3(scaleOnceAgain * MVector4(mInst->_vertices[i], 1.0f));
	//	mInst->_normals[i] = MVector3(scaleOnceAgainIT * MVector4(mInst->_normals[i], 0.0f));

	//	mInst->_normals[i].Normalize();
	//}

	// ====== AnimationClip creation. Finally. Damn me if it wasn't a long journey.

	for (auto it = td.Anims.GetBegin(); it.IsValid(); ++it)
	{
		TempSkeletonData::Anim* anim = (*it).GetValue();

		if (anim->Keys.GetSize() < 1)
			continue;

		MAnimationClip* clip = new MAnimationClip();

		clip->_name = anim->Stack->GetName();
		clip->_length = anim->Length;

		for (MSize i = 0; i < anim->Keys.GetSize(); ++i)
		{
			clip->_frames.Add(AnimationFrame());
			AnimationFrame* frame = &clip->_frames[clip->_frames.GetSize() - 1];

			// get matrices only for nodes that take part in skeleton
			MArray<MMatrix> usedMatrices;
			for (MSize j = 0; j < td.Joints.GetSize(); ++j)
			{
				if (td.Joints[j].TakesPartInSkeleton)
				{
					usedMatrices.Add(anim->LocalTransforms[i][j]);
				}
			}

			frame->Initialize(usedMatrices, anim->Keys[i], (*outSkeleton));
		}

		outAnimations.Add(clip);
	}

	return true;
}

void morphEngine::assetLibrary::MMesh::ProcessSkeletonHierarchyRecursively(TempSkeletonData& td, FbxNode * inNode, MInt32 myIndex, MInt32 inParentIndex)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		TempSkeletonData::Joint joint;
		joint.ParentIndex = inParentIndex;
		joint.Name = inNode->GetName();
		//debugging::Debug::Log(debugging::ELogType::LOG, joint.Name);

		td.Joints.Add(joint);
		if(inParentIndex >= 0) td.Joints[inParentIndex].Children.Add(myIndex);


		for (MInt32 i = 0; i < inNode->GetChildCount(); ++i)
		{
			ProcessSkeletonHierarchyRecursively(td, inNode->GetChild(i), static_cast<MInt32>(td.Joints.GetSize()), myIndex);
		}
	}
	else
	{
		for (MInt32 i = 0; i < inNode->GetChildCount(); ++i)
		{
			ProcessSkeletonHierarchyRecursively(td, inNode->GetChild(i), static_cast<MInt32>(td.Joints.GetSize()), myIndex);
		}
	}
}

void morphEngine::assetLibrary::MMesh::MakeGlobalTransform(TempSkeletonData & td, utility::MArray<utility::MMatrix>& transforms, MInt32 joint, const utility::MMatrix & parentTransform)
{
	transforms[joint] = transforms[joint] * parentTransform;
	for (MInt32 i = 0; i < td.Joints[joint].Children.GetSize(); ++i)
	{
		MInt32 child = td.Joints[joint].Children[i];
		MakeGlobalTransform(td, transforms, child, transforms[joint]);
	}
}

void morphEngine::assetLibrary::MMesh::Shutdown()
{
	_header.IndicesCount = 0;
	_header.SkeletonPathLength = 0;
	_header.SkeletonPath = "";
	_header.SubmeshesCount = 0;
	_header.Type = MeshType::UNKNOWN;
	_header.VerticesCount = 0;

	_vertices.Clear();
	_normals.Clear();
	_uvs.Clear();
	_boneWeights.Clear();
	_boneIndices.Clear();
	_indices.Clear();
	_submeshes.Clear();

	_skeleton = nullptr;
	
	if (_resVertices != nullptr)
	{
		_resVertices->Shutdown();
		delete _resVertices;
		_resVertices = nullptr;
	}
	if (_resNormals != nullptr)
	{
		_resNormals->Shutdown();
		delete _resNormals;
		_resNormals = nullptr;
	}
	if (_resUvs != nullptr)
	{
		_resUvs->Shutdown();
		delete _resUvs;
		_resUvs = nullptr;
	}
	if (_resBoneWeights != nullptr)
	{
		_resBoneWeights->Shutdown();
		delete _resBoneWeights;
		_resBoneWeights = nullptr;
	}
	if (_resBoneIndices != nullptr)
	{
		_resBoneIndices->Shutdown();
		delete _resBoneIndices;
		_resBoneIndices = nullptr;
	}
	if (_resIndices != nullptr)
	{
		_resIndices->Shutdown();
		delete _resIndices;
		_resIndices = nullptr;
	}
}

//void morphEngine::assetLibrary::MMesh::MakeGlobalTransform(Bone * bone, const utility::MMatrix & parentTransform)
//{
//	*(bone->_inverseBindposePtr) = *(bone->_inverseBindposePtr) * parentTransform;
//
//	for (auto it = bone->_children.GetBegin(); it.IsValid(); ++it)
//	{
//		MakeGlobalTransform((*it), *(bone->_inverseBindposePtr));
//	}
//}

bool morphEngine::assetLibrary::MMesh::LoadFromData(const utility::MFixedArray<MUint8>& readData)
{
	if (!CheckType(readData.GetDataPointer()))
		return false;

	MUint8* dataPtr = readData.GetDataPointer();

	MSize offset = sizeof(MAssetType);
	offset += _header.Load(dataPtr + offset);

	_vertices.Resize(_header.VerticesCount);
	memcpy(_vertices.GetDataPointer(), dataPtr + offset, sizeof(utility::MVector3) * _header.VerticesCount);
	offset += sizeof(utility::MVector3) * _header.VerticesCount;

	_normals.Resize(_header.VerticesCount);
	memcpy(_normals.GetDataPointer(), dataPtr + offset, sizeof(utility::MVector3) * _header.VerticesCount);
	offset += sizeof(utility::MVector3) * _header.VerticesCount;

	_uvs.Resize(_header.VerticesCount);
	memcpy(_uvs.GetDataPointer(), dataPtr + offset, sizeof(utility::MVector2) * _header.VerticesCount);
	offset += sizeof(utility::MVector2) * _header.VerticesCount;

	if (_header.Type == MeshType::SKELETAL)
	{
		_boneWeights.Resize(_header.VerticesCount);
		memcpy(_boneWeights.GetDataPointer(), dataPtr + offset, sizeof(BoneWeight) * _header.VerticesCount);
		offset += sizeof(BoneWeight) * _header.VerticesCount;

		_boneIndices.Resize(_header.VerticesCount);
		memcpy(_boneIndices.GetDataPointer(), dataPtr + offset, sizeof(BoneIndex) * _header.VerticesCount);
		offset += sizeof(BoneIndex) * _header.VerticesCount;

		// auto load associated skeleton

		_skeleton = resourceManagement::ResourceManager::GetInstance()->GetSkeleton(_header.SkeletonPath);
		ME_WARNING(_skeleton != nullptr, "Loading a skeletal mesh but skeleton may not be present.");
	}

	_indices.Resize(_header.IndicesCount);
	memcpy(_indices.GetDataPointer(), dataPtr + offset, sizeof(MUint16) * _header.IndicesCount);
	offset += sizeof(MUint16) * _header.IndicesCount;

	_submeshes.Resize(_header.SubmeshesCount);
	memcpy(_submeshes.GetDataPointer(), dataPtr + offset, sizeof(MSize) * _header.SubmeshesCount);

	return true;
}

bool morphEngine::assetLibrary::MMesh::LoadFromData(const utility::MArray<MUint8>& data)
{
	return LoadFromData(reinterpret_cast<const MFixedArray<MUint8>&>(data));
}

void morphEngine::assetLibrary::MMesh::UpdateVerticesResource()
{
	if (_resVertices == nullptr)
		return;

	if (_resVertices->_accessMode != device::BufferAccessMode::WRITE)
	{
		// reinitialize subresources
		_resVertices->Shutdown();
		_resVertices->Initialize(reinterpret_cast<MFloat32*>(_vertices.GetDataPointer()), sizeof(utility::MVector3), _vertices.GetSize(), device::BufferAccessMode::WRITE);
	}

	if (_header.VerticesCount == _vertices.GetSize())
	{
		// update subresource via mapping
		MVector3* vRes;
		_resVertices->Map(reinterpret_cast<void**>(&vRes));
		memcpy(vRes, _vertices.GetDataPointer(), _vertices.GetSize() * sizeof(MVector3));
		_resVertices->Unmap();
	}
	else
	{
		_header.VerticesCount = _vertices.GetSize();

		// reinitialize subresource
		_resVertices->Shutdown();
		_resVertices->Initialize(reinterpret_cast<MFloat32*>(_vertices.GetDataPointer()), sizeof(utility::MVector3), _vertices.GetSize(), device::BufferAccessMode::WRITE);
	}

	EvtVerticesChanged(this);
}

void morphEngine::assetLibrary::MMesh::UpdateNormalsResource()
{
	if (_resNormals == nullptr)
		return;

	if (_resNormals->_accessMode != device::BufferAccessMode::WRITE)
	{
		// reinitialize subresource
		_resNormals->Shutdown();
		_resNormals->Initialize(reinterpret_cast<MFloat32*>(_normals.GetDataPointer()), sizeof(utility::MVector3), _normals.GetSize(), device::BufferAccessMode::WRITE);
	}

	if (_header.VerticesCount == _normals.GetSize())
	{
		// update subresource via mapping
		MVector3* vRes;
		_resNormals->Map(reinterpret_cast<void**>(&vRes));
		memcpy(vRes, _normals.GetDataPointer(), _normals.GetSize() * sizeof(MVector3));
		_resNormals->Unmap();
	}
	else
	{
		_header.VerticesCount = _normals.GetSize();

		// reinitialize subresources
		_resNormals->Shutdown();
		_resNormals->Initialize(reinterpret_cast<MFloat32*>(_normals.GetDataPointer()), sizeof(utility::MVector3), _normals.GetSize(), device::BufferAccessMode::WRITE);
	}
}

void morphEngine::assetLibrary::MMesh::UpdateUVsResource()
{
	if (_resUvs == nullptr)
		return;

	if (_resUvs->_accessMode != device::BufferAccessMode::WRITE)
	{
		// reinitialize subresources
		_resUvs->Shutdown();
		_resUvs->Initialize(reinterpret_cast<MFloat32*>(_uvs.GetDataPointer()), sizeof(utility::MVector2), _uvs.GetSize(), device::BufferAccessMode::WRITE);
	}

	if (_header.VerticesCount == _uvs.GetSize())
	{
		// update subresource via mapping
		MVector2* vRes;
		_resUvs->Map(reinterpret_cast<void**>(&vRes));
		memcpy(vRes, _uvs.GetDataPointer(), _uvs.GetSize() * sizeof(MVector2));
		_resUvs->Unmap();
	}
	else
	{
		_header.VerticesCount = _uvs.GetSize();

		// reinitialize subresources
		_resUvs->Shutdown();
		_resUvs->Initialize(reinterpret_cast<MFloat32*>(_uvs.GetDataPointer()), sizeof(utility::MVector2), _uvs.GetSize(), device::BufferAccessMode::WRITE);
	}
}

void morphEngine::assetLibrary::MMesh::UpdateIndicesResource()
{
	if (_resIndices == nullptr)
		return;

	//ME_WARNING_RETURN_STATEMENT(!GetIsSkeletal() && _vertices.GetSize() == _normals.GetSize() == _uvs.GetSize(), "Setting indices failed, some buffers have unequal number of elements.");
	//ME_WARNING_RETURN_STATEMENT(GetIsSkeletal() && 
	//	_vertices.GetSize() == _normals.GetSize() == _uvs.GetSize() == _boneWeights.GetSize() == _boneIndices.GetSize(), 
	//	"Setting indices failed, some buffers have unequal number of elements.");

	if (_resIndices->_accessMode != device::BufferAccessMode::WRITE)
	{
		// reinitialize subresource
		_resIndices->Shutdown();
		_resIndices->Initialize(reinterpret_cast<MUint16*>(_indices.GetDataPointer()), _indices.GetSize(), device::BufferAccessMode::WRITE);
	}

	if (_header.IndicesCount == _indices.GetSize())
	{
		// update subresource via mapping
		MUint16* vRes;
		_resIndices->Map(reinterpret_cast<void**>(&vRes));
		memcpy(vRes, _indices.GetDataPointer(), _indices.GetSize() * sizeof(MUint16));
		_resIndices->Unmap();
	}
	else
	{
		_header.IndicesCount = _indices.GetSize();

		// reinitialize subresource
		_resIndices->Shutdown();
		_resIndices->Initialize(reinterpret_cast<MUint16*>(_indices.GetDataPointer()), _indices.GetSize(), device::BufferAccessMode::WRITE);
	}
}

void morphEngine::assetLibrary::MMesh::UpdateBoneWeightsResource()
{
	if (_resBoneWeights == nullptr)
		return;

	if (_resBoneWeights->_accessMode != device::BufferAccessMode::WRITE)
	{
		// reinitialize subresources
		_resBoneWeights->Shutdown();
		_resBoneWeights->Initialize(reinterpret_cast<MFloat32*>(_boneWeights.GetDataPointer()), sizeof(BoneWeight), _boneWeights.GetSize(), device::BufferAccessMode::WRITE);
	}

	if (_header.VerticesCount == _boneWeights.GetSize())
	{
		// update subresource via mapping
		BoneWeight* vRes;
		_resBoneWeights->Map(reinterpret_cast<void**>(&vRes));
		memcpy(vRes, _boneWeights.GetDataPointer(), _boneWeights.GetSize() * sizeof(BoneWeight));
		_resBoneWeights->Unmap();
	}
	else
	{
		_header.VerticesCount = _boneWeights.GetSize();

		// reinitialize subresources
		_resBoneWeights->Shutdown();
		_resBoneWeights->Initialize(reinterpret_cast<MFloat32*>(_boneWeights.GetDataPointer()), sizeof(BoneWeight), _boneWeights.GetSize(), device::BufferAccessMode::WRITE);
	}
}

void morphEngine::assetLibrary::MMesh::UpdateBoneIndicesResource()
{
	if (_resBoneIndices == nullptr)
		return;

	if (_resBoneIndices->_accessMode != device::BufferAccessMode::WRITE)
	{
		// reinitialize subresources
		_resBoneIndices->Shutdown();
		_resBoneIndices->Initialize(reinterpret_cast<MFloat32*>(_boneIndices.GetDataPointer()), sizeof(BoneIndex), _boneIndices.GetSize(), device::BufferAccessMode::WRITE);
	}

	if (_header.VerticesCount == _boneIndices.GetSize())
	{
		// update subresource via mapping
		BoneIndex* vRes;
		_resBoneIndices->Map(reinterpret_cast<void**>(&vRes));
		memcpy(vRes, _boneIndices.GetDataPointer(), _boneIndices.GetSize() * sizeof(BoneIndex));
		_resBoneIndices->Unmap();
	}
	else
	{
		_header.VerticesCount = _boneIndices.GetSize();

		// reinitialize subresources
		_resBoneIndices->Shutdown();
		_resBoneIndices->Initialize(reinterpret_cast<MFloat32*>(_boneIndices.GetDataPointer()), sizeof(BoneIndex), _boneIndices.GetSize(), device::BufferAccessMode::WRITE);
	}
}

void morphEngine::assetLibrary::MMesh::UpdateBounds(renderer::BoundsBox & bounds)
{
	bounds.MinLocal = MVector3(FLT_MAX, FLT_MAX, FLT_MAX);
	bounds.MaxLocal = -bounds.MinLocal;

	for (auto it = _vertices.GetBegin(); it.IsValid(); ++it)
	{
		MVector3& vert(*it);

		if (vert.X < bounds.MinLocal.X)
			bounds.MinLocal.X = vert.X;
		if (vert.Y < bounds.MinLocal.Y)
			bounds.MinLocal.Y = vert.Y;
		if (vert.Z < bounds.MinLocal.Z)
			bounds.MinLocal.Z = vert.Z;
		if (vert.X > bounds.MaxLocal.X)
			bounds.MaxLocal.X = vert.X;
		if (vert.Y > bounds.MaxLocal.Y)
			bounds.MaxLocal.Y = vert.Y;
		if (vert.Z > bounds.MaxLocal.Z)
			bounds.MaxLocal.Z = vert.Z;
	}
}

void morphEngine::assetLibrary::MMesh::UpdateBounds(renderer::BoundsSphere & bounds)
{
	bounds.CenterLocal = MVector3::Zero;
	for (auto it = _vertices.GetBegin(); it.IsValid(); ++it)
	{
		bounds.CenterLocal += (*it);
	}
	bounds.CenterLocal /= static_cast<MFloat32>((_vertices.GetSize()));

	bounds.RadiusLocal = -FLT_MAX;
	for (auto it = _vertices.GetBegin(); it.IsValid(); ++it)
	{
		MFloat32 distSqr = ((*it) - bounds.CenterLocal).LengthSquared();
		if (distSqr > bounds.RadiusLocal)
			bounds.RadiusLocal = distSqr;
	}
	bounds.RadiusLocal = MMath::Sqrt(bounds.RadiusLocal);
}

MSize morphEngine::assetLibrary::MMeshHeader::Load(MUint8 * data)
{
	MSize bytesRead = 0;
	memcpy(&VerticesCount, data + bytesRead, sizeof(MSize));
	bytesRead += sizeof(MSize);
	memcpy(&IndicesCount, data + bytesRead, sizeof(MSize));
	bytesRead += sizeof(MSize);
	memcpy(&SubmeshesCount, data + bytesRead, sizeof(MSize));
	bytesRead += sizeof(MSize);
	memcpy(&Type, data + bytesRead, sizeof(MeshType));
	bytesRead += sizeof(MeshType);
	memcpy(&SkeletonPathLength, data + bytesRead, sizeof(MSize));
	bytesRead += sizeof(MSize);

	SkeletonPath = "";
	for (MSize i = 0; i < SkeletonPathLength; ++i)
	{
		char p;
		memcpy(&p, data + bytesRead, sizeof(char));
		SkeletonPath += p;
		bytesRead += sizeof(char);
	}

	return bytesRead;
}

bool morphEngine::assetLibrary::MMeshHeader::SaveAppend(resourceManagement::fileSystem::File & file) const
{
	file.WriteAppend(reinterpret_cast<const MUint8*>(&VerticesCount), sizeof(MSize));
	file.WriteAppend(reinterpret_cast<const MUint8*>(&IndicesCount), sizeof(MSize));
	file.WriteAppend(reinterpret_cast<const MUint8*>(&SubmeshesCount), sizeof(MSize));
	file.WriteAppend(reinterpret_cast<const MUint8*>(&Type), sizeof(MeshType));
	file.WriteAppend(reinterpret_cast<const MUint8*>(&SkeletonPathLength), sizeof(MSize));
	file.WriteAppend(reinterpret_cast<const MUint8*>(SkeletonPath.Data()), sizeof(char) * SkeletonPathLength);

	return true;
}