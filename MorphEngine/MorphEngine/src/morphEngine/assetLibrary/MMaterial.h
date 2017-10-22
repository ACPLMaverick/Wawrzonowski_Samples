#pragma once

#include "MAsset.h"
#include "utility/MArray.h"
#include "utility/MColor.h"
#include "utility/MMatrix.h"
#include "utility/MVector.h"

namespace morphEngine
{
	namespace renderer
	{
		namespace shaders
		{
			class ShaderObject;
		}
	}

	namespace gom
	{
		class Transform;
		class Camera;
	}

	namespace animation
	{
		class AnimationFrame;
	}

	namespace assetLibrary
	{
		class MTexture1D;
		class MTexture2D;
		class MTextureCube;
		class MMesh;
		/// <summary>
		/// Class that stores the material information. Its fields are generated automatically from an assiciated shader and their values can be set by
		/// reading from file or directly in code, but generally user must know what shader is assigned to this material.
		/// </summary>
		class MMaterial : public MAsset
		{
		public:

#pragma region Enums Public

			enum class AlphaMode : MUint32
			{
				MODE_OPAQUE,
				MODE_ADDITIVE,
				MODE_CUTOUT,
				MODE_ALPHA_BLEND
			};

			enum class LitMode : MUint32
			{
				UNLIT,
				DEFAULT_LIT
			};

#pragma endregion

		protected:

#pragma region StructsProtected
			/*
				File structure:
				AssetType (fixed size)
				InternalHeader (fixed size)
				Shader Name (varying size)
				MMatrix array (varying size)
				MColor array (varying size)
				MVector3 array (varying size)
				MVector2 array (varying size)
				Float array (varying size)
				Texture1D StringLength-String array (varying size)
				Texture2D StringLength-String array (varying size)
				TextureCube StringLength-String array (varying size)
			*/
			struct InternalHeader
			{
				MSize SizeMatrix;
				MSize SizeColor;
				MSize SizeVector3;
				MSize SizeVector2;
				MSize SizeUint;
				MSize SizeFloat;
				MSize SizeTexture1D;
				MSize SizeTexture2D;
				MSize SizeTextureCube;
				MSize ShaderNameLength;
			};

#pragma endregion

#pragma region Protected

			utility::MArray<utility::MMatrix> _paramsMatrix;
			utility::MArray<utility::MColor> _paramsColor;
			utility::MArray<utility::MVector3> _paramsVector3;
			utility::MArray<utility::MVector2> _paramsVector2;
			utility::MArray<MUint64> _paramsUint;
			utility::MArray<MFloat32> _paramsFloat;
			utility::MArray<MTexture1D*> _paramsTexture1D;
			utility::MArray<MTexture2D*> _paramsTexture2D;
			utility::MArray<MTextureCube*> _paramsTextureCube;

			LitMode _litMode = LitMode::DEFAULT_LIT;
			AlphaMode _alphaMode = AlphaMode::MODE_OPAQUE;

			renderer::shaders::ShaderObject* _shader = GetDefaultShader();

#pragma endregion

#pragma region Protected Functions

			inline void ClearInterface();
			inline void UpdateSetGlobalStatistics() const;
			inline void UpdateSetMaterialStatistics() const;
			inline void UpdateDrawCallStatistics(MSize vertices, MSize indices) const;
			static MTexture1D* GetDefaultTexture1D();
			static MTexture2D* GetDefaultTexture2D();
			static MTextureCube* GetDefaultTextureCube();
			static renderer::shaders::ShaderObject* GetDefaultShader();

			virtual void Shutdown() override;

#pragma endregion

		public:

#pragma region Public Functions

			//Default constructor
			inline MMaterial() : MAsset(),
				_paramsMatrix(3),
				_paramsColor(3),
				_paramsVector3(3),
				_paramsVector2(1),
				_paramsUint(1),
				_paramsFloat(1),
				_paramsTexture1D(1),
				_paramsTexture2D(2),
				_paramsTextureCube(1)
			{ 
				_type = MAssetType::MATERIAL; 
			}
			//Constructs MMaterial with given type and loads data from given bytes array
			inline MMaterial(MAssetType type, const utility::MFixedArray<MUint8>& data) : MAsset(type, data),
				_paramsMatrix(3),
				_paramsColor(3),
				_paramsVector3(3),
				_paramsVector2(1),
				_paramsUint(1),
				_paramsFloat(1),
				_paramsTexture1D(1),
				_paramsTexture2D(2),
				_paramsTextureCube(1)
			{
			}
			inline MMaterial(const MMaterial& other) : MAsset(other),
				_paramsMatrix(other._paramsMatrix),
				_paramsColor(other._paramsColor),
				_paramsVector3(other._paramsVector3),
				_paramsVector2(other._paramsVector2),
				_paramsUint(other._paramsUint),
				_paramsFloat(other._paramsFloat),
				_paramsTexture1D(other._paramsTexture1D),
				_paramsTexture2D(other._paramsTexture2D),
				_paramsTextureCube(other._paramsTextureCube),
				_shader(other._shader)
			{

			}

			inline virtual ~MMaterial()
			{
			}

			inline MMaterial& operator=(const MMaterial& other)
			{
				_paramsMatrix = other._paramsMatrix;
				_paramsColor = other._paramsColor;
				_paramsVector3 = other._paramsVector3;
				_paramsVector2 = other._paramsVector2;
				_paramsUint = other._paramsUint;
				_paramsFloat = other._paramsFloat;
				_paramsTexture1D = other._paramsTexture1D;
				_paramsTexture2D = other._paramsTexture2D;
				_paramsTextureCube = other._paramsTextureCube;
				_shader = other._shader;

				return *this;
			}

#pragma region Drawing pipeline

			// This must be called before drawing mesh via this material.
			void SetPass(const gom::Camera& camera, MUint8 passIndex = 0) const;
			void SetPassDeferred(const gom::Camera& camera, MUint8 passIndex = 0) const;
			void SetPassSkinned(const gom::Camera& camera, MUint8 passIndex = 0) const;
			void SetPassSkinnedDeferred(const gom::Camera& camera, MUint8 passIndex = 0) const;

			void UnsetPass(MUint8 passIndex = 0) const;

			void SetData(MUint8 passIndex = 0) const;
			void SetDataDeferred(MUint8 passIndex = 0) const;
			void SetDataSkinned(MUint8 passIndex = 0) const;
			void SetDataSkinnedDeferred(MUint8 passIndex = 0) const;

			void DrawMesh(const gom::Camera& camera, const gom::Transform& transform, const MMesh& mesh, MSize submeshIndex = 0) const;
			void DrawMeshSkinned(const gom::Camera& camera, const gom::Transform& transform, const MMesh& mesh, const animation::AnimationFrame& frame, MSize submeshIndex = 0) const;
			void DrawMeshDeferred(const gom::Camera& camera, const gom::Transform& transform, const MMesh& mesh, MSize submeshIndex = 0) const;
			void DrawMeshSkinnedDeferred(const gom::Camera& camera, const gom::Transform& transform, const MMesh& mesh, const animation::AnimationFrame& frame, MSize submeshIndex = 0) const;

#pragma endregion

#pragma region Inteface accessors

			// AddSlot functions return pointer to the added slot data.

			inline utility::MMatrix* AddSlotMatrix(utility::MMatrix& mem = utility::MMatrix::Identity) { _paramsMatrix.Add(mem); return &_paramsMatrix[_paramsMatrix.GetSize() - 1]; }
			inline utility::MColor* AddSlotColor(utility::MColor& mem = utility::MColor(1.0f, 1.0f, 1.0f, 1.0f)) { _paramsColor.Add(mem); return &_paramsColor[_paramsColor.GetSize() - 1]; }
			inline utility::MVector3* AddSlotVector3(utility::MVector3& mem = utility::MVector3::Zero) { _paramsVector3.Add(mem); return &_paramsVector3[_paramsVector3.GetSize() - 1]; }
			inline utility::MVector2* AddSlotVector2(utility::MVector2& mem = utility::MVector2::Zero) { _paramsVector2.Add(mem); return &_paramsVector2[_paramsVector2.GetSize() - 1]; }
			inline MUint64* AddSlotUint(MUint64 mem = 0) { _paramsUint.Add(mem); return &_paramsUint[_paramsUint.GetSize() - 1]; }
			inline MFloat32* AddSlotFloat(MFloat32 mem = 0.0f) { _paramsFloat.Add(mem); return &_paramsFloat[_paramsFloat.GetSize() - 1]; }
			inline MTexture1D** AddSlotTexture1D(MTexture1D* mem = GetDefaultTexture1D()) { _paramsTexture1D.Add(mem); return &_paramsTexture1D[_paramsTexture1D.GetSize() - 1]; }		// remember to change null pointers to DefaultTexture (or sth)
			inline MTexture2D** AddSlotTexture2D(MTexture2D* mem = GetDefaultTexture2D()) { _paramsTexture2D.Add(mem); return &_paramsTexture2D[_paramsTexture2D.GetSize() - 1]; }
			inline MTextureCube** AddSlotTextureCube(MTextureCube* mem = GetDefaultTextureCube()) { _paramsTextureCube.Add(mem); return &_paramsTextureCube[_paramsTextureCube.GetSize() - 1]; }

			inline MSize GetSlotCountMatrix() const { return _paramsMatrix.GetSize(); }
			inline MSize GetSlotCountColor() const { return _paramsColor.GetSize(); }
			inline MSize GetSlotCountVector3() const { return _paramsVector3.GetSize(); }
			inline MSize GetSlotCountVector2() const { return _paramsVector2.GetSize(); }
			inline MSize GetSlotCountUint() const { return _paramsUint.GetSize(); }
			inline MSize GetSlotCountFloat() const { return _paramsFloat.GetSize(); }
			inline MSize GetSlotCountTexture1D() const { return _paramsTexture1D.GetSize(); }
			inline MSize GetSlotCountTexture2D() const { return _paramsTexture2D.GetSize(); }
			inline MSize GetSlotCountTextureCube() const { return _paramsTextureCube.GetSize(); }

			inline void RemoveSlotMatrix(utility::MMatrix& mem) { _paramsMatrix.Remove(mem); }
			inline void RemoveSlotColor(utility::MColor& mem) { _paramsColor.Remove(mem); }
			inline void RemoveSlotVector3(utility::MVector3& mem) { _paramsVector3.Remove(mem); }
			inline void RemoveSlotVector2(utility::MVector2& mem) { _paramsVector2.Remove(mem); }
			inline void RemoveSlotUint(MUint64 mem) { _paramsUint.Remove(mem); }
			inline void RemoveSlotFloat(MFloat32 mem) { _paramsFloat.Remove(mem); }
			inline void RemoveSlotTexture1D(MTexture1D* mem) { _paramsTexture1D.Remove(mem); }
			inline void RemoveSlotTexture2D(MTexture2D* mem) { _paramsTexture2D.Remove(mem); }
			inline void RemoveSlotTextureCube(MTextureCube* mem) { _paramsTextureCube.Remove(mem); }

			inline void RemoveSlotMatrix(MSize index) { _paramsMatrix.RemoveAt(index); }
			inline void RemoveSlotColor(MSize index) { _paramsColor.RemoveAt(index); }
			inline void RemoveSlotVector3(MSize index) { _paramsVector3.RemoveAt(index); }
			inline void RemoveSlotVector2(MSize index) { _paramsVector2.RemoveAt(index); }
			inline void RemoveSlotFloat(MSize index) { _paramsFloat.RemoveAt(index); }
			inline void RemoveSlotTexture1D(MSize index) { _paramsTexture1D.RemoveAt(index); }
			inline void RemoveSlotTexture2D(MSize index) { _paramsTexture2D.RemoveAt(index); }
			inline void RemoveSlotTextureCube(MSize index) { _paramsTextureCube.RemoveAt(index); }

			inline void SetMatrix(const utility::MMatrix& mat, MSize slotIndex) { ME_ASSERT_S(_paramsMatrix.GetSize() > slotIndex); _paramsMatrix[slotIndex] = mat; }
			inline void SetColor(const utility::MColor& col, MSize slotIndex) { ME_ASSERT_S(_paramsColor.GetSize() > slotIndex); _paramsColor[slotIndex] = col; }
			inline void SetVector3(const utility::MVector3& vec, MSize slotIndex) { ME_ASSERT_S(_paramsVector3.GetSize() > slotIndex); _paramsVector3[slotIndex] = vec; }
			inline void SetVector2(const utility::MVector2& vec, MSize slotIndex) { ME_ASSERT_S(_paramsVector2.GetSize() > slotIndex); _paramsVector2[slotIndex] = vec; }
			inline void SetUint(MUint64 val, MSize slotIndex) { ME_ASSERT_S(_paramsUint.GetSize() > slotIndex); _paramsUint[slotIndex] = val; }
			inline void SetFloat(const MFloat32 val, MSize slotIndex) { ME_ASSERT_S(_paramsFloat.GetSize() > slotIndex); _paramsFloat[slotIndex] = val; }
			inline void SetTexture1D(const MTexture1D& tex, MSize slotIndex) { ME_ASSERT_S(_paramsTexture1D.GetSize() > slotIndex); _paramsTexture1D[slotIndex] = const_cast<MTexture1D*>(&tex); }
			inline void SetTexture2D(const MTexture2D& tex, MSize slotIndex) { ME_ASSERT_S(_paramsTexture2D.GetSize() > slotIndex); _paramsTexture2D[slotIndex] = const_cast<MTexture2D*>(&tex); }
			inline void SetTextureCube(const MTextureCube& tex, MSize slotIndex) { ME_ASSERT_S(_paramsTextureCube.GetSize() > slotIndex); _paramsTextureCube[slotIndex] = const_cast<MTextureCube*>(&tex); }

			inline utility::MMatrix GetMatrix(MSize index) const { return _paramsMatrix[index]; }
			inline utility::MColor GetColor(MSize index) const { return _paramsColor[index]; }
			inline utility::MVector3 GetVector3(MSize index) const { return _paramsVector3[index]; }
			inline utility::MVector2 GetVector2(MSize index) const { return _paramsVector2[index]; }
			inline MUint64 GetUint(MSize index) const { return _paramsUint[index]; }
			inline MFloat32 GetFloat(MSize index) const { return _paramsFloat[index]; }
			inline MTexture1D* GetTexture1D(MSize index) const { return _paramsTexture1D[index]; }
			inline MTexture2D* GetTexture2D(MSize index) const { return _paramsTexture2D[index]; }
			inline MTextureCube* GetTextureCube(MSize index) const { return _paramsTextureCube[index]; }

			inline renderer::shaders::ShaderObject* GetShader() const { return _shader; }
			void SetShader(renderer::shaders::ShaderObject* shd);

			inline LitMode GetLitMode() const { return _litMode; }
			inline AlphaMode GetAlphaMode() const { return _alphaMode; }

			inline void SetLitMode(LitMode litMode) { _litMode = litMode; }
			inline void SetAlphaMode(AlphaMode alphaMode) { _alphaMode = alphaMode; }

#pragma endregion

			// Inherited via MAsset

			virtual bool LoadFromData(const utility::MFixedArray<MUint8>& data) override;
			virtual bool LoadFromData(const utility::MArray<MUint8>& data) override;
			virtual bool LoadFromFile(resourceManagement::fileSystem::File & file) override;
			virtual bool SaveToFile(resourceManagement::fileSystem::File & file) const override;
			inline virtual void Initialize()
			{

			}

#pragma endregion

		};

	}
}