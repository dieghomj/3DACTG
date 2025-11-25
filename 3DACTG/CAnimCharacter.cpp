#include "stdafx.h"
#include "CAnimCharacter.h"

CAnimCharacter::CAnimCharacter()
	: CCharacter()
	, m_pSkinMesh(nullptr)
	, m_pAnimCtrl(nullptr)
	, m_AnimNo(10)
	, m_AnimTime(0.f)
	, m_AnimSpeed(0.002f)
	, m_vBonePos(0.f, 0.f, 0.f)
{

}

CAnimCharacter::~CAnimCharacter()
{
}

void CAnimCharacter::Update()
{

	m_pSkinMesh->SetAnimSpeed(m_AnimSpeed);

	//アニメーション切替
	if (GetAsyncKeyState('N') & 0x8000)
	{
		m_AnimNo = 10;		//登場アニメーション
		m_AnimTime = 0.0;	//アニメーション経過時間初期化
		m_pSkinMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);
	}
	

	m_AnimNo++;
	m_pSkinMesh->ChangeAnimSet(m_AnimNo, m_pAnimCtrl);

	//アニメーション切替
	

}

void CAnimCharacter::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog)
{

	if (m_pSkinMesh == nullptr) {
		return;
	}

	m_pSkinMesh->SetAnimSpeed(m_AnimSpeed);

	//描画直前で座標や回転情報などを更新.
	m_pSkinMesh->SetPosition(m_vPosition);
	m_pSkinMesh->SetRotation(m_vRotation);
	m_pSkinMesh->SetScale(m_vScale);
	//レンダリング.
	m_pSkinMesh->Render(View, Proj, Light, Camera.vPosition,Fog,
		m_pAnimCtrl);	//クローンを設定
}

void CAnimCharacter::AttachSkinMesh( CSkinMesh& pMesh)
{

	m_pSkinMesh = &pMesh;

	//アニメーションコントローラを取得
	LPD3DXANIMATIONCONTROLLER pAC = m_pSkinMesh->GetAnimationController();

	//アニメーションコントローラのクローンを作成
	if (FAILED(
		pAC->CloneAnimationController(
			pAC->GetMaxNumAnimationOutputs(),
			pAC->GetMaxNumAnimationSets(),
			pAC->GetMaxNumTracks(),
			pAC->GetMaxNumEvents(),
			&m_pAnimCtrl)))
	{
		_ASSERT_EXPR(false, L"アニメーションコントローラのクローン作成失敗");
	}
}

