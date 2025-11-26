#pragma once
#include "CStaticMeshObject.h"
#include "CAnimCharacter.h"
#include "CInput.h"

class CPlayer 
	: public CAnimCharacter
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
		Idle = 1,
		Running = 4,
		Jumping = 3,
		Attacking = 2
	};

	//アニメーション状態
	//プレイヤーモデルによる修正が必要
	enum AnimationState {
		AnimIdle = 1,
		AnimAttack = 2,
		AnimJump = 3,
		AnimRun = 4,
	};

public:
	CPlayer();
	virtual ~CPlayer() override;
	virtual void Update() override;
	virtual void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog) override;
	int GetPlayerState() const {
		return m_PlayerState;
	}
	void SetTankControlMode(bool mode) {
		m_bTankControlMode = mode;
	}

private:
	void AnimControl();
	void RadioControl();
	void HandleInput();

protected:

	D3DXVECTOR3 m_vVelocity;				//速度ベクトル

	float			m_TurnSpeed;				//回転速度
	float			m_MoveSpeed;				//移動速度
	MoveState		m_MoveState;				//移動状態
	PlayerState		m_PlayerState;				//プレイヤー状態
	AnimationState	m_AnimationState;			//アニメーション状態
	CInput*			m_pInput;					//入力管理クラス
	bool			m_bTankControlMode = false;	//タンク操作モードかどうか
};

