#include "stdafx.h"
#include "CZako.h"

CZako::CZako()
	: CBaseEnemy()
	, attackRange(2.0f)
	, attackCooldown(3.0f)
	, attackTimer(0.0f)
	, m_pPlayer(nullptr)
{

}

CZako::CZako(std::vector<Pair> path, int width, int height)
	: CBaseEnemy(path, width, height)
	, attackRange(2.0f)
	, attackCooldown(3.0f)
	, attackTimer(0.0f)
{
}

CZako::~CZako()
{
}

void CZako::Update()
{

	if (m_State == enState::Attack)
	{
		// プレイヤーとの距離を計算
		D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
		D3DXVECTOR3 enemyPos = GetPosition();
		D3DXVECTOR3 diff = playerPos - enemyPos;
		float distanceToPlayer = D3DXVec3Length(&diff);
		// 攻撃範囲内にプレイヤーがいるかチェック
		if (distanceToPlayer <= attackRange)
		{
			// 攻撃クールダウンが完了しているかチェック
			if (attackTimer >= attackCooldown)
			{
				// 攻撃処理
				m_Shot = true; // 弾を飛ばすフラグを立てる
				// 攻撃タイマーリセット
				attackTimer = 0.0f;
			}
		}
		else
		{
			m_Shot = false; // プレイヤーが範囲外なら弾を飛ばさない
			m_State = enState::Move; // プレイヤーが範囲外なら追跡状態に戻る
		}
		return;
	}

	D3DXVECTOR3 vDirection = m_pPlayer->GetPosition() - m_vPosition;
	float distance = D3DXVec3Length(&vDirection);

	if (distance > 0.5f)
	{
		D3DXVec3Normalize(&vDirection, &vDirection);
		m_vPosition += vDirection * 0.05f;
	}
	else
	{
		m_State = enState::Attack;	// 攻撃状態に切り替え
	}

	// 基底クラスのUpdateを呼び出す
	CBaseEnemy::Update();
}

void CZako::Draw(SCENE_DATA& sceneData)
{
	CBaseEnemy::Draw(sceneData);
}

void CZako::TickAttackTimer(float deltaTime)
{
	// 攻撃タイマーを進める
	attackTimer += deltaTime;
}

