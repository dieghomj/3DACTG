#include "stdafx.h"
#include "CCamera.h"

CCamera::CCamera()
	: m_vLook		(D3DXVECTOR3(0.0f, 2.0f, 10.0f))
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

void CCamera::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera)
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

//三人称カメラ
void CCamera::ThirdPersonCamera(
	const D3DXVECTOR3& TargetPos, float TargetRotY)
{
	//Z軸ベクトル（Z+方向の単位ベクトル）
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);

	//Y方向の回転行列
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,	//(out)行列
		TargetRotY);	//対象のY方向の回転値

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z軸ベクトル
		&vecAxisZ,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	//カメラの位置、注視位置を対象にそろえる
	m_vPosition = TargetPos;
	m_vLook = TargetPos;

	//カメラの位置、注視位置をZ軸ベクトルを用いて調整
	m_vPosition -= vecAxisZ * 6.f;	//対象の背中側
	m_vLook += vecAxisZ * 3.f;	//対象を挟んで向こう側

	//カメラの位置、注視位置の高さをそれぞれ微調整
	m_vPosition.y += 4.f;
	m_vLook.y += 0.5f;
}

void CCamera::FirstPersonCamera(
	const D3DXVECTOR3& TargetPos, POINT delta, float sense)
{
	m_Yaw += (float)delta.x * sense;
	m_Pitch -= (float)delta.y * sense;

	const float pitchLimit = D3DX_PI * 0.49f;
	if (m_Pitch > pitchLimit) m_Pitch = pitchLimit;
	if (m_Pitch < -pitchLimit) m_Pitch = -pitchLimit;

	D3DXVECTOR3 lookDirection;
	lookDirection.x = cosf(m_Pitch) * sinf(m_Yaw);
	lookDirection.y = sinf(m_Pitch);
	lookDirection.z = cosf(m_Pitch) * cosf(m_Yaw);

	//カメラの位置、注視位置を対象にそろえる
	m_vPosition = TargetPos;
	//カメラの位置、注視位置の高さをそれぞれ微調整
	m_vPosition.y += 2.f;

	m_vLook = m_vPosition + lookDirection;	//対象を挟んで向こう側
	//pCamera->m_vLook.y += 0.5f;

}

void CCamera::SetPerspective(float fovY, float aspect, float zn, float zf)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;
}
