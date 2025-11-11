#pragma once 
#include "CGameObject.h"

class CCamera
	:public CGameObject
{
public:
	CCamera();
	~CCamera();

	// CGameObject を介して継承されました
	void Update() override;

	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog) override;

	void UpdateForward(D3DXVECTOR3 vFwd);

	// レンズ設定（FOV/アスペクト/ニア/ファー）
	void SetPerspective(float fovY, float aspect, float zn, float zf);
	// リサイズ時などにアスペクトだけ更新
	void SetAspect(float aspect) { m_Aspect = aspect; }


	D3DXVECTOR3 GetForward() const { return m_vLook; }
	float GetYaw()    const { return m_Yaw; }
	float GetPitch()  const { return m_Pitch; }
	float GetFovY()   const { return m_FovY; }
	float GetAspect() const { return m_Aspect; }
	float GetNearZ()  const { return m_NearZ; }
	float GetFarZ()   const { return m_FarZ; }

private:
	D3DXVECTOR3 m_vLook;
	float m_Yaw;
	float m_Pitch;
	float m_FovY;
	float m_Aspect;
	float m_NearZ;
	float m_FarZ;
};