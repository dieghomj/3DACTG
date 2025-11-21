#include "stdafx.h"
#include "CPlayer.h"

CPlayer::CPlayer()
	: m_vVelocity	(0.f, 0.f, 0.f)
	, m_TurnSpeed	(0.05f)
	, m_MoveSpeed	(0.1f)
	, m_MoveState	(Stop)
	, m_PlayerState (Idle)
	, m_pInput		(nullptr)
	, m_bTankControlMode(false)
{
	m_pInput = new CInput();
}

CPlayer::~CPlayer()
{
	SAFE_DELETE(m_pInput);
}

void CPlayer::Update()
{
	HandleInput();
	m_bTankControlMode = false;
	//レイの位置をプレイヤーの座標にそろえる
	m_pRayY->Position = m_vPosition;
	//地面めり込み回避のためプレイヤーの位置よりも少し上にしておく
	m_pRayY->Position.y += 0.2f;
	m_pRayY->RotationY = m_vRotation.y;

	//十字（前後左右に伸ばした）レイの設定	
	for (int dir = 0; dir < CROSSRAY::max; dir++)
	{
		m_pCrossRay->Ray[dir].Position = m_vPosition;
		m_pCrossRay->Ray[dir].Position.y += 0.1f;
		m_pCrossRay->Ray[dir].RotationY = m_vRotation.y;
	}
	CCharacter::Update();
	
}

void CPlayer::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog)
{
	CCharacter::Draw(View, Proj, Light, Camera, Fog);
}

void CPlayer::RadioControl()
{
	//Z軸ベクトル(Z+方向への単位ベクトル)
	//※大きさ（長さ）が１のベクトルを単位ベクトルという
	D3DXVECTOR3 vecAxisZ(0.f, 0.f, 1.f);
	D3DXVECTOR3 vecAxisX(1.f, 0.f, 0.f);

	//Y方向の回転行列
	D3DXMATRIX mRotationY;
	//Y軸回転行列を作成
	D3DXMatrixRotationY(
		&mRotationY,		//(out)行列
		m_vRotation.y);		//プレイヤーのY方向の回転値

	//Y軸回転行列を使ってZ軸ベクトルを座標変換する
	D3DXVec3TransformCoord(
		&vecAxisZ,		//(out)Z軸ベクトル
		&vecAxisZ,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	D3DXVec3TransformCoord(
		&vecAxisX,		//(out)Z軸ベクトル
		&vecAxisX,		//(in)Z軸ベクトル
		&mRotationY);	//Y軸回転行列

	//移動状態によって処理を分ける
	switch (m_MoveState) {
	case MoveState::Forward:	//前進
		m_vPosition += vecAxisZ * m_MoveSpeed;
		break;
	case MoveState::Backward:	//後退
		m_vPosition -= vecAxisZ * m_MoveSpeed;
		break;
	case MoveState::Left:	//左移動
		m_vPosition -= vecAxisX * m_MoveSpeed;
		break;
	case MoveState::Right:	//右移動
		m_vPosition += vecAxisX * m_MoveSpeed;
		break;
	case MoveState::Up:	//上昇
		m_vPosition.y += m_MoveSpeed;
		break;
	case MoveState::Down:	//下降
		m_vPosition.y -= m_MoveSpeed;
 		break;
	default:
		break;
	}
	//上記の移動処理が終われば停止状態にしておく
	m_MoveState = MoveState::Stop; 
}

void CPlayer::HandleInput()
{ 
	m_pInput->Update();
	if (m_pInput->GetKeyDown(VK_UP) || m_pInput->GetKeyDown('W')) {
		m_MoveState = MoveState::Forward;
	}
	//後退
	if (m_pInput->GetKeyDown(VK_DOWN) || m_pInput->GetKeyDown('S')) {
		m_MoveState = MoveState::Backward;
	}
	if (m_pInput->GetKeyDown(VK_LEFT) || m_pInput->GetKeyDown('A')) {
		if(m_bTankControlMode)
			m_vRotation.y -= m_TurnSpeed;
		else
			m_MoveState = MoveState::Left;
	}
	if (m_pInput->GetKeyDown(VK_RIGHT) || m_pInput->GetKeyDown('D')) {
		if (m_bTankControlMode)
			m_vRotation.y += m_TurnSpeed;
		else
			m_MoveState = MoveState::Right;
	}
	if (m_pInput->GetKeyDown(VK_SPACE)) {
		m_MoveState = MoveState::Up;
	}
	if (m_pInput->GetKeyDown(VK_LCONTROL)) {
		m_MoveState = MoveState::Down;
	}
	RadioControl();
}




