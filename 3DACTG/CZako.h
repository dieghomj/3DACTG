#pragma once
#include "CBaseEnemy.h"

class CZako 
	: public CBaseEnemy
{

	public:
	CZako();
	CZako(std::vector<Pair> path, int width, int height);
	~CZako();
	void Update() override;
	void Draw(SCENE_DATA& sceneData) override;

	void TickAttackTimer(float deltaTime);
	void SetTargetPlayer(CGameObject* player) { m_pPlayer = player; }

private:
	float attackRange;
	float attackCooldown;
	float attackTimer;

	CGameObject* m_pPlayer;

};

