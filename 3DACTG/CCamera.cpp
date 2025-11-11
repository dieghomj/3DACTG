#include "stdafx.h"
#include "CCamera.h"

CCamera::CCamera()
	: m_vLook		(D3DXVECTOR3(0.0f, 0.0f, 10.0f))
	, m_Yaw			(0.f)
	, m_Pitch		(0.f)
	, m_FovY		(D3DX_PI / 4.0f)	// 45°
	, m_Aspect		(16.0f / 9.0f)		// 初期既定。リサイズ時に更新推奨
	, m_NearZ		(0.1f)
	, m_FarZ		(1000.0f)
{

}

CCamera::~CCamera()
{
}

void CCamera::Update()
{
}

void CCamera::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog)
{
	D3DXVECTOR3 cam_pos = m_vPosition;
	D3DXVECTOR3 cam_look = m_vLook;
	D3DXVECTOR3	vUpVec(0.0f, 1.0f, 0.0f);	//上方（ベクトル）.

	D3DXMatrixLookAtLH(
		&View,	//(out)ビュー計算結果.
		&cam_pos, &cam_look, &vUpVec);

	// プロジェクション計算（レンズパラメータに基づく）
	D3DXMatrixPerspectiveFovLH(
		&Proj,	//(out)プロジェクション計算結果. 
		m_FovY, m_Aspect, m_NearZ, m_FarZ);

	// カメラ情報を構造体に格納
	Camera.vPosition = m_vPosition;
	Camera.vLook = m_vLook;
	Camera.yaw = m_Yaw;
	Camera.pitch = m_Pitch;

}

void CCamera::SetPerspective(float fovY, float aspect, float zn, float zf)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;
}

void CCamera::UpdateForward(D3DXVECTOR3 vFwd)
{
	m_vLook = m_vPosition + vFwd;
}