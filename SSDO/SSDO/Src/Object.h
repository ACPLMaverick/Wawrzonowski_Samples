#pragma once

#include "GlobalDefines.h"
#include "Material.h"

class Camera;
class Mesh;

class Object
{
protected:

#pragma region Protected

	XMFLOAT4X4A _matWorld;
	XMFLOAT4X4A _matWorldInvTransp;

	XMFLOAT3A _rotation;
	XMFLOAT3A _scale;
	XMFLOAT3A _position;

	const Mesh& _mesh;
	const Material& _material;

	bool _bNeedCreateWorldMatrix = false;

#pragma endregion

#pragma region Functions Protected

	inline void CreateWorldMatrix();

#pragma endregion

public:

#pragma region Functions Public

	Object(const Mesh& mesh, const Material& material,
		const XMFLOAT3A& pos = XMFLOAT3A(0.0f, 0.0f, 0.0f),
		const XMFLOAT3A& rot = XMFLOAT3A(0.0f, 0.0f, 0.0f),
		const XMFLOAT3A& scl = XMFLOAT3A(1.0f, 1.0f, 1.0f));
	~Object();

	virtual void Update();
	virtual inline void Draw(const Camera& camera) const { _material.DrawMesh(*this, camera, _mesh); }

#pragma region GettersSetters

	inline const XMFLOAT3A& GetPosition() const { return _position; }
	inline XMFLOAT3A GetRotation() const { return _rotation; }
	inline const XMFLOAT3A& GetScale() const { return _scale; }
	inline const XMFLOAT4X4A& GetWorldMatrix() const { return _matWorld; }
	inline const XMFLOAT4X4A& GetWorldInvTransMatrix() const { return _matWorldInvTransp; }

	inline void SetPosition(const XMFLOAT3A& pos) { _position = pos; _bNeedCreateWorldMatrix = true; }
	inline void SetRotation(const XMFLOAT3A& rot) { _rotation = rot; _bNeedCreateWorldMatrix = true; }
	inline void SetScale(const XMFLOAT3A& scl) { _scale = scl; _bNeedCreateWorldMatrix = true; }

#pragma endregion

#pragma endregion
};

