#include "stdafx.h"
#include "CCamera.h"

CCamera::CCamera()
	: m_vLook		(D3DXVECTOR3(0.0f, 0.0f, 10.0f))
	, m_vUp			(D3DXVECTOR3(0.0f, 1.0f, 0.0f))
	, m_vRight		(D3DXVECTOR3(1.0f, 0.0f, 0.0f))
	, m_NearWindowHeight(0.0f)
	, m_FarWindowHeight(0.0f)
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
	UpdateViewMatrix(View, Proj); 
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

void CCamera::SetLens(float fovY, float aspect, float zn, float zf)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight	= 2.0f * m_NearZ * tanf(0.f * m_FovY);
	m_FarWindowHeight	= 2.0f * m_FarZ * tanf(0.f * m_FovY);

}

void CCamera::Walk(float distance)
{
	m_vPosition += distance * m_vLook ;
}

void CCamera::Strafe(float distance)
{
	m_vPosition += distance * m_vRight ;
}

void CCamera::Pitch(float pitch)
{
	D3DXMATRIX mRot;

	D3DXMatrixRotationAxis(&mRot, &m_vRight, pitch);
	D3DXVec3TransformNormal(&m_vUp, &m_vUp, &mRot);
	D3DXVec3TransformNormal(&m_vLook, &m_vLook, &mRot);
}

void CCamera::Yaw(float yaw)
{
	D3DXMATRIX mRot;
	D3DXMatrixRotationY(&mRot, yaw);
	D3DXVec3TransformNormal(&m_vRight, &m_vRight, &mRot);
	D3DXVec3TransformNormal(&m_vUp, &m_vUp, &mRot);
	D3DXVec3TransformNormal(&m_vLook, &m_vLook, &mRot);

}

void CCamera::UpdateViewMatrix(D3DXMATRIX& mView, D3DXMATRIX& mProj)
{
	D3DXVECTOR3 cam_pos = m_vPosition;
	D3DXVECTOR3 vLookVec = m_vLook;
	D3DXVECTOR3	vUpVec = m_vUp;	//上方（ベクトル）.
	D3DXVECTOR3	vRightVec = m_vRight; //右方（ベクトル）.

	D3DXVec3Normalize(&vLookVec, &vLookVec);

	D3DXVec3Cross(&vUpVec, &vLookVec, &vRightVec);
	D3DXVec3Normalize(&vUpVec, &vUpVec);

	D3DXVec3Cross(&vRightVec, &vUpVec, &vLookVec);

	m_vLook		= vLookVec;
	m_vUp		= vUpVec;
	m_vRight	= vRightVec;

	float x = -D3DXVec3Dot(&vRightVec, &cam_pos);
	float y = -D3DXVec3Dot(&vUpVec, &cam_pos);
	float z = -D3DXVec3Dot(&vLookVec, &cam_pos);
	
	D3DXVECTOR3 vAt = cam_pos + vLookVec;

	//// ビュー計算（D3DX関数利用版）
	//D3DXMatrixLookAtLH(
	//	&mView,
	//	&cam_pos,					//(in)視点の位置.
	//	&(vAt),			//(in)注視点の位置.
	//	&vUpVec);					//(in)上方向ベクトル.

	// ビュー計算
	mView(0, 0) = vRightVec.x;
	mView(1, 0) = vRightVec.y;
	mView(2, 0) = vRightVec.z;
	mView(3, 0) = x;

	mView(0, 1) = vUpVec.x;
	mView(1, 1) = vUpVec.y;
	mView(2, 1) = vUpVec.z;
	mView(3, 1) = y;

	mView(0, 2) = vLookVec.x;
	mView(1, 2) = vLookVec.y;
	mView(2, 2) = vLookVec.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;

	//mView = mNewView;

	// プロジェクション計算（レンズパラメータに基づく）
	D3DXMatrixPerspectiveFovLH(
		&mProj,	//(out)プロジェクション計算結果. 
		m_FovY, m_Aspect, m_NearZ, m_FarZ);

}

