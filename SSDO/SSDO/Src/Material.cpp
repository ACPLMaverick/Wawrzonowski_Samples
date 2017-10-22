#include "stdafx.h"
#include "Material.h"
#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Object.h"
#include "System.h"
#include "Scenes\Scene.h"

Material::Material(const Shader& shader, const XMFLOAT4A & colorBase, const XMFLOAT4A & colorSpecular, float gloss) :
	_shader(shader),
	_colorBase(colorBase),
	_colorSpecular(colorSpecular),
	_gloss(gloss)
{
	
}

Material::~Material()
{
}

void Material::Update()
{
}

void Material::DrawMesh(const Object& object, const Camera & camera, const Mesh & mesh) const
{
	// set shader parameters
	Shader::ColorBufferVS* bufferVs;
	Shader::ColorBufferPS* bufferPs;
	ID3D11DeviceContext* deviceContext = Renderer::GetInstance()->GetDeviceContext();

	XMFLOAT4X4A wvp;
	XMMATRIX w = XMLoadFloat4x4(&object.GetWorldMatrix());
	XMMATRIX v = XMLoadFloat4x4(&camera.GetMatView());
	XMMATRIX vp = XMLoadFloat4x4(&camera.GetMatViewProj());
	XMMATRIX transposedWVP = XMMatrixTranspose(w * vp);
	XMStoreFloat4x4(&wvp, transposedWVP);

	XMMATRIX wv = w * v;
	XMMATRIX transposedWV = XMMatrixTranspose(wv);
	XMMATRIX transposedWInvTranspV = XMMatrixInverse(&XMMatrixDeterminant(wv), wv);

	// set vertex and index buffers

	bufferVs = reinterpret_cast<Shader::ColorBufferVS*>(_shader.MapVsBuffer(0));

	XMStoreFloat4x4(&bufferVs->gMatWVP, transposedWVP);
	XMStoreFloat4x4(&bufferVs->gMatW, transposedWV);
	XMStoreFloat4x4(&bufferVs->gMatWInvTransp, transposedWInvTranspV);

	_shader.UnmapVsBuffer(0);

	bufferPs = reinterpret_cast<Shader::ColorBufferPS*>(_shader.MapPsBuffer(0));
	memcpy(&bufferPs->gColBase, &_colorBase, sizeof(XMFLOAT4A));
	memcpy(&bufferPs->gColSpecular, &_colorSpecular, sizeof(XMFLOAT4A));
	bufferPs->gGloss = _gloss;
	_shader.UnmapPsBuffer(0);

	_shader.Set();

	// DRAW!

	mesh.DrawBuffers();
}

Material * Material::CreateResource(const std::wstring & name)
{
	return new Material(*System::GetInstance()->GetScene()->LoadShader(name));
}
