#pragma once

#include "GlobalDefines.h"
#include "GBuffer.h"

namespace Scenes
{
	class Scene;
}

class Camera
{
protected:

#pragma region Protected

	GBuffer _gBuffer;



	XMFLOAT4X4A _matViewProj;
	XMFLOAT4X4A _matView;
	XMFLOAT4X4A _matProj;
	XMFLOAT4X4A _matViewInverse;
	XMFLOAT4X4A _matProjInverse;

	XMFLOAT3A _position;
	XMFLOAT3A _target;
	XMFLOAT3A _up;

	XMFLOAT3A _direction;
	XMFLOAT3A _right;

	float _fov;
	float _near;
	float _far;

	bool _bNeedUpdateView = false;
	bool _bNeedUpdateProj = false;

#pragma endregion

#pragma region Functions Protected

	inline void UpdateView();
	inline void UpdateProjection();
	inline void MergeMatrices();

#pragma endregion

public:

#pragma region Functions Public

	Camera(	const XMFLOAT3A& position = XMFLOAT3A(5.0f, 5.0f, 5.0f),
			const XMFLOAT3A& target = XMFLOAT3A(0.0f, 0.0f, 0.0f),
			const XMFLOAT3A& up = XMFLOAT3A(0.0f, 1.0f, 0.0f),
			float fov = 0.7f,
			float near = 1.0f,
			float far = 100.0f);
	~Camera();

	void Update();
	void Draw(Scenes::Scene& scene) const;

#pragma region GettersSetters

	inline const GBuffer& GetGBuffer() const { return _gBuffer; }

	inline void SetPosition(const XMFLOAT3A& position) { _position = position; _bNeedUpdateView = true; }
	inline void SetTarget(const XMFLOAT3A& target) { _target = target; _bNeedUpdateView = true; }
	inline void SetUp(const XMFLOAT3A& up) { _up = up; _bNeedUpdateView = true; }
	void SetDirection(const XMFLOAT3A& direction);
	inline void SetPosition(const XMVECTOR& position) { XMStoreFloat3A(&_position, position); _bNeedUpdateView = true; }
	inline void SetTarget(const XMVECTOR& target) { XMStoreFloat3A(&_target, target); _bNeedUpdateView = true; }
	inline void SetUp(const XMVECTOR& up) { XMStoreFloat3A(&_up, up); _bNeedUpdateView = true; }
	void SetDirection(const XMVECTOR& direction);
	inline void SetFOV(float fov) { _fov = fov; _bNeedUpdateProj = true; }
	void SetNear(float nearPlane) { _near = nearPlane; _bNeedUpdateProj = true; }
	void SetFar(float farPlane) { _far = farPlane; _bNeedUpdateProj = true; }

	inline const XMFLOAT4X4A& GetMatViewProj() const { return _matViewProj; }
	inline const XMFLOAT4X4A& GetMatView() const { return _matView; }
	inline const XMFLOAT4X4A& GetMatProj() const { return _matProj; }
	inline const XMFLOAT4X4A& GetMatViewInverse() const { return _matViewInverse; }
	inline const XMFLOAT4X4A& GetMatProjInverse() const { return _matProjInverse; }
	inline const XMFLOAT3A& GetPosition() const { return _position; }
	inline const XMFLOAT3A& GetTarget() const { return _target; }
	inline const XMFLOAT3A& GetUp() const { return _up; }
	inline const XMFLOAT3A& GetDirection() const { return _direction; }
	inline const XMFLOAT3A& GetRight() const { return _right; }
	inline const float& GetFOV() const { return _fov; }
	inline const float& GetNearPlane() const { return _near; }
	inline const float& GetFarPlane() const { return _far; }

#pragma endregion

#pragma endregion

};

