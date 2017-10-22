#pragma once

#include "GlobalDefines.h"
#include "Postprocesses/SSDOBase.h"

class ShaderBytecodeLoader
{
protected:

	static const std::wstring PATH_PREFIX;
	static const std::wstring PATH_SUFFIX_VS;
	static const std::wstring PATH_SUFFIX_HS;
	static const std::wstring PATH_SUFFIX_GS;
	static const std::wstring PATH_SUFFIX_DS;
	static const std::wstring PATH_SUFFIX_PS;
	static const std::wstring PATH_SUFFIX_CS;

public:
	// Data under arrayPtr gets allocated inside this function.
	static void LoadBytecode(const wstring& shaderFilename, uint8_t** outArrayPtr, size_t& outDataSize);
	static void LoadBytecodeVS(const wstring& shaderName, uint8_t** outArrayPtr, size_t& outDataSize);
	static void LoadBytecodeHS(const wstring& shaderName, uint8_t** outArrayPtr, size_t& outDataSize);
	static void LoadBytecodeGS(const wstring& shaderName, uint8_t** outArrayPtr, size_t& outDataSize);
	static void LoadBytecodeDS(const wstring& shaderName, uint8_t** outArrayPtr, size_t& outDataSize);
	static void LoadBytecodePS(const wstring& shaderName, uint8_t** outArrayPtr, size_t& outDataSize);
	static void LoadBytecodeCS(const wstring& shaderName, uint8_t** outArrayPtr, size_t& outDataSize);
};

class Shader
{
public:

	struct ConstantBufferDesc
	{
		size_t Size;

		ConstantBufferDesc(size_t size) :
			Size(size)
		{

		}
	};


	// shader-specific

#pragma region Structs

	__declspec(align(16))
		struct ColorBufferVS
	{
		XMFLOAT4X4A gMatWVP;
		XMFLOAT4X4A gMatW;
		XMFLOAT4X4A gMatWInvTransp;
	};

	__declspec(align(16))
		struct TextConstantBuffer
	{
		XMFLOAT4A Color = XMFLOAT4A(1.0f, 1.0f, 1.0f, 1.0f);
		XMFLOAT4A PositionScale = XMFLOAT4A(0.0f, 0.0f, 1.0f, 1.0f);
	};

	__declspec(align(16))
		struct ColorBufferPS
	{
		XMFLOAT4A gColBase;
		XMFLOAT4A gColSpecular;
		float gGloss;
	};

	__declspec(align(16))
		struct LightCommonDataPS
	{
		XMFLOAT4X4A gProjInverse;
	};

	__declspec(align(16))
		struct SSAOBasePS
	{
		XMFLOAT4X4A Proj;
		XMFLOAT4A Offsets[14];
		XMFLOAT4A Params;
	};

	__declspec(align(16))
		struct SSAOBlurMergePS
	{
		XMFLOAT2A TexelSize;
	};

	__declspec(align(16))
		struct SSDOBasePS
	{
		XMFLOAT4X4A Proj;
		XMFLOAT4A Offsets[Postprocesses::SSDOBase::SAMPLE_COUNT];
		XMFLOAT4A Params;
		XMFLOAT4A LightColor;
		XMFLOAT3A LightDirection;
	};

	__declspec(align(16))
		struct SSDOBlurMergePS
	{
		XMFLOAT2 TexelSize;
		bool bHorizontalBlur;
	};

	__declspec(align(16))
		struct SSDOImprovedPS
	{
		XMFLOAT4A LightColor;
		XMFLOAT3A LightDirection;
		XMFLOAT4A SatDimensionsAndRecs;
		float SampleBoxHalfSize;
		float OcclusionPower;
		float OcclusionFalloff;
		float PowFactor;
	};

	__declspec(align(16))
		struct SSDOImprovedBPS
	{
		XMFLOAT4 LightColor;
		XMFLOAT3 LightDirection;
		float SampleBoxHalfSize;
		float OcclusionPower;
		float OcclusionFalloff;
		float PowFactor;
	};

#pragma endregion

protected:

#pragma region Protected

	ID3D11VertexShader* _vs = nullptr;
	ID3D11PixelShader* _ps = nullptr;

	ID3D11InputLayout* _inputLayout = nullptr;

	ID3D11Buffer** _constantVsBuffers = nullptr;
	size_t _constantVsBufferCount = 0;

	ID3D11Buffer** _constantPsBuffers = nullptr;
	size_t _constantPsBufferCount = 0;

#pragma endregion

#pragma region Functions Protected


#pragma endregion

public:

	Shader(const wstring& shaderFilename, size_t inputLayoutNumElements, 
		ConstantBufferDesc* cbVsDescs, size_t cbVsCount,
		ConstantBufferDesc* cbPsDescs, size_t cbPsCount);
	~Shader();

	void Set() const;
	void* MapVsBuffer(size_t i) const;
	void UnmapVsBuffer(size_t i) const;
	void* MapPsBuffer(size_t i) const;
	void UnmapPsBuffer(size_t i) const;
	ID3D11Buffer* GetVsBuffer(size_t i) const;
	ID3D11Buffer* GetPsBuffer(size_t i) const;

	static Shader* CreateResource(const std::wstring& name);
};

