#pragma once
#include "CStaticMeshObject.h"
#include "CCharacter.h"
#include "CInput.h"

class CPlayer 
	: public CCharacter
{

public:

	enum MoveState {
		Forward,
		Left,
		Right,
		Backward,
		Up,
		Down,
		Stop
	};

	enum PlayerState {
		Idle,
		Running,
		Jumping,
		Attacking
	};

public:
	CPlayer();
	virtual ~CPlayer() override;
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog) override;
	void SetTankControlMode(bool mode) {
		m_bTankControlMode = mode;
	}

private:
	void RadioControl();
	void HandleInput();

protected:

	D3DXVECTOR3 m_vVelocity;				//速度ベクトル

	float			m_TurnSpeed;				//回転速度
	float			m_MoveSpeed;				//移動速度
	MoveState		m_MoveState;				//移動状態
	PlayerState		m_PlayerState;				//プレイヤー状態
	CInput*			m_pInput;					//入力管理クラス
	bool			m_bTankControlMode = false;	//タンク操作モードかどうか
};

