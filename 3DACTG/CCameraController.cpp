#include "stdafx.h"
#include "CCameraController.h"

CCameraController::CCameraController()
	: m_pCamera		()
	, m_vPosition	(0.f, 0.f, 0.f)
	, m_vRotation	(0.f, 0.f, 0.f)
{
}

CCameraController::CCameraController(CCamera* camera)
	: m_pCamera		(camera)
	, m_vPosition	(0.f, 0.f, 0.f)
	, m_vRotation	(0.f, 0.f, 0.f)
{
}

CCameraController::~CCameraController()
{
}

void CCameraController::Update(float deltaTime)
{

}

//三人称カメラ
void CCameraController::ThirdPersonCamera(
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

	//カメラの位置、注視位置をZ軸ベクトルを用いて調整
	m_vPosition -= vecAxisZ * 6.f;	//対象の背中側

	//カメラの位置、注視位置の高さをそれぞれ微調整
	m_vPosition.y += 4.f;
}

void CCameraController::FirstPersonCamera(
	const D3DXVECTOR3& TargetPos, POINT delta, float sense)
{
	float yaw = m_vRotation.y + (float)delta.x * sense;
	float pitch	= m_vRotation.x + (float)delta.y * sense;

	const float pitchLimit = D3DX_PI * 0.49f;
	if (pitch > pitchLimit) pitch = pitchLimit;
	if (pitch < -pitchLimit) pitch = -pitchLimit;

	m_pCamera->Pitch(pitch);
	m_pCamera->Yaw(yaw);

	HandleInput();

}

void CCameraController::HandleInput()
{

	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
		m_pCamera->Walk(0.3f);
	}
	//後退
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
		m_pCamera->Walk(-0.3f);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
		m_pCamera->Strafe(-0.3f);
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
		m_pCamera->Strafe(0.3f);
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		m_pCamera->SetPosition(m_pCamera->GetPosition() + D3DXVECTOR3(0, 0.3f, 0));
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		m_pCamera->SetPosition(m_pCamera->GetPosition() + D3DXVECTOR3(0, -0.3f, 0));
	}


}
