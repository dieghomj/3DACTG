#include "stdafx.h"
#include "CPlayer.h"

CPlayer::CPlayer()
	: m_vVelocity	(0.f, 0.f, 0.f)
	, m_TurnSpeed	(0.05f)
	, m_MoveSpeed	(0.1f)
	, m_MoveState	(Stop)
	, m_PlayerState (Idle)
{

}

CPlayer::~CPlayer()
{
}

void CPlayer::Update()
{
	HandleInput();



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
		m_vRotation.y += m_TurnSpeed;
		break;
	case MoveState::Right:	//右移動
		m_vRotation.y -= m_TurnSpeed;
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
	if (GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) {
		m_MoveState = MoveState::Forward;
	}
	//後退
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000) {
		m_MoveState = MoveState::Backward;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) {
		m_MoveState = MoveState::Right;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) {
		m_MoveState = MoveState::Left;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		m_MoveState = MoveState::Up;
	}
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		m_MoveState = MoveState::Down;
	}
	RadioControl();
}




