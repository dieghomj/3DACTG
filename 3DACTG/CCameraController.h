#pragma once
#include "CCamera.h"

class CCameraController
{

	public:
	CCameraController();
	CCameraController(CCamera* camera);

	~CCameraController();

	void Update(float deltaTime);

	void ThirdPersonCamera(
		const D3DXVECTOR3& TargetPos, float TargetRotY);

	void FirstPersonCamera(
		const D3DXVECTOR3& TargetPos, POINT delta, float sense);

	void SetPosition(float x, float y, float z) { m_vPosition = D3DXVECTOR3(x, y, z); };
	void SetRotation(float pitch, float yaw, float roll) { m_vRotation = D3DXVECTOR3(yaw, pitch, roll); };
	D3DXVECTOR3 GetPosition() const { return m_vPosition; };
	D3DXVECTOR3 GetRotation() const { return m_vRotation; };
	D3DXVECTOR3 GetForward() const { return m_pCamera->GetForward(); };
	D3DXVECTOR3 GetForwardVector() const;

private:
	CCamera* m_pCamera;
	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vRotation;
};

