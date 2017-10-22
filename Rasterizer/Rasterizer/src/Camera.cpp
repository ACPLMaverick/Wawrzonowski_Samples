#include "Camera.h"
#include "Input.h"
#include "Timer.h"

Camera::Camera(
	const math::Float3* pos,
	const math::Float3* tgt,
	const math::Float3* up,
	float fovY,
	float ar,
	float np,
	float fp
) :
	_position(*pos),
	_target(*tgt),
	_up(*up),
	_fovY(fovY),
	_aspectRatio(ar),
	_nearPlane(np),
	_farPlane(fp)
{
	math::Matrix4x4::LookAt(&_position, &_target, &_up, &_viewMatrix);
	math::Matrix4x4::Perspective(_fovY, _aspectRatio, _nearPlane, _farPlane, &_projMatrix);
	math::Matrix4x4::Inverse(&_viewMatrix, &_viewInvMatrix);
	_viewProjMatrix = _viewMatrix * _projMatrix;

	_direction = _target - _position;
	math::Float3::Normalize(_direction);
	_right = math::Float3::Cross(_up, _direction);
}


Camera::~Camera()
{
}

void Camera::Update()
{
	// camera movement
	math::Float3 addition;
	if (Input::GetInstance()->GetKeyDown('W'))
	{
		addition = _direction * _cameraSpeed * (float)Timer::GetInstance()->GetDeltaTime();
		_position = _position + addition;
		_target = _target + addition;
		_viewMatrixNeedUpdate = true;
	}
	if (Input::GetInstance()->GetKeyDown('A'))
	{
		addition = _right * -_cameraSpeed * (float)Timer::GetInstance()->GetDeltaTime();
		_position = _position + addition;
		_target = _target + addition;
		_viewMatrixNeedUpdate = true;
	}
	if (Input::GetInstance()->GetKeyDown('S'))
	{
		addition = _direction * (-_cameraSpeed) * (float)Timer::GetInstance()->GetDeltaTime();
		_position = _position + addition;
		_target = _target + addition;
		_viewMatrixNeedUpdate = true;
	}
	if (Input::GetInstance()->GetKeyDown('D'))
	{
		addition = _right * _cameraSpeed * (float)Timer::GetInstance()->GetDeltaTime();
		_position = _position + addition;
		_target = _target + addition;
		_viewMatrixNeedUpdate = true;
	}
	if (Input::GetInstance()->GetKeyDown('Q'))
	{
		addition = _up * _cameraSpeed * (float)Timer::GetInstance()->GetDeltaTime();
		_position = _position + addition;
		_target = _target + addition;
		_viewMatrixNeedUpdate = true;
	}
	if (Input::GetInstance()->GetKeyDown('Z'))
	{
		addition = _up * -_cameraSpeed * (float)Timer::GetInstance()->GetDeltaTime();
		_position = _position + addition;
		_target = _target + addition;
		_viewMatrixNeedUpdate = true;
	}

	if (Input::GetInstance()->GetKeyDown(VK_RBUTTON))
	{
		math::Float2 delta = Input::GetInstance()->GetMouseRelativePosition() * _cameraSpeed * (float)Timer::GetInstance()->GetDeltaTime() * 2.0f;
		delta.x *= -1.0f;

		if (delta.x != 0.0f || delta.y != 0.0f)
		{
			_position = _position + _right * delta.x + _up * delta.y;
			_viewMatrixNeedUpdate = true;
		}
	}

	// update of the matrices

	if (_viewMatrixNeedUpdate || _projMatrixNeedUpdate)
	{
		if (_viewMatrixNeedUpdate)
		{
			math::Matrix4x4::LookAt(&_position, &_target, &_up, &_viewMatrix);
			math::Matrix4x4::Inverse(&_viewMatrix, &_viewInvMatrix);
			_direction = _target - _position;
			math::Float3::Normalize(_direction);
			_right = math::Float3::Cross(_up, _direction);
			_viewMatrixNeedUpdate = false;
		}
		if (_projMatrixNeedUpdate)
		{
			math::Matrix4x4::Perspective(_fovY, _aspectRatio, _nearPlane, _farPlane, &_projMatrix);
			_projMatrixNeedUpdate = false;
		}
		_viewProjMatrix = _viewMatrix * _projMatrix;
	}
}