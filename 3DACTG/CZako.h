#pragma once
#include "CBaseEnemy.h"
#include "CPlayer.h"

class CZako 
	: public CBaseEnemy
{

	public:
	CZako();
	CZako(std::vector<Pair> path, int width, int height);
	~CZako();
	void Update() override;
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog) override;

	void TickAttackTimer(float deltaTime);
	void SetTargetPlayer(CPlayer* player) { m_pPlayer = player; }

private:
	float attackRange;
	float attackCooldown;
	float attackTimer;

	CPlayer* m_pPlayer;

};

