#include "stdafx.h"
#include "CEffect.h"
#include "CGameTest.h"

CGameTest::CGameTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pMeshObject	(nullptr)
	, m_pGroundMeshObject(nullptr)
	, m_pGroundMesh(nullptr)
	, m_pPlayerMesh(nullptr)
	, m_pPlayerSkinMesh(nullptr)
	, m_pPlayer		(nullptr)
	, m_pEnemyList	()
	, m_pEnemyMesh(nullptr)
{
}

CGameTest::~CGameTest()
{

}

void CGameTest::Create()
{
	m_pCamera = new CCamera();
	m_pCamera->SetPerspective(D3DX_PI / 4.0f,
		static_cast<float>(WND_W) / static_cast<float>(WND_H),
		0.1f, 1000.0f);
	m_pCameraController = new CCameraController(m_pCamera);

	m_pGroundMesh = new CStaticMesh();
	m_pGroundMeshObject = new CStaticMeshObject();
	m_pMeshObject = new CStaticMeshObject();
	m_pPlayer = new CPlayer();
	m_pPlayerMesh = new CStaticMesh();
	m_pPlayerSkinMesh = new CSkinMesh();
	m_pEnemyMesh = new CStaticMesh();
	for (int i = 0; i < 5; i++)
	{
		m_pEnemyList.emplace_back(new CZako());
	}

	CEffect::GetInstance()->Create(
		m_pDx11->GetDevice(),
		m_pDx11->GetContext());
}

void CGameTest::Release()
{

	SAFE_DELETE(m_pGroundMesh);
	SAFE_DELETE(m_pGroundMeshObject);
	SAFE_DELETE(m_pMeshObject);
	SAFE_DELETE(m_pPlayer);
	for (auto& enemy : m_pEnemyList)
	{
		SAFE_DELETE(enemy);
	}
	m_pEnemyList.clear();
}

HRESULT CGameTest::LoadData()
{
	m_pGroundMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Ground\\Ground.x");
	m_pEnemyMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\zako\\zako.x");
	m_pPlayerMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\WomanAnime\\animeWoman.x");
	m_pPlayerSkinMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Skin\\WomanAnime\\animeWoman.x");
	m_pGroundMeshObject->AttachMesh(*m_pGroundMesh);

	//m_pPlayer->AttachMesh(*m_pPlayerMesh);
	m_pPlayer->AttachSkinMesh(*m_pPlayerSkinMesh);
	m_pPlayer->SetScale(0.01f);
	for( auto& enemy : m_pEnemyList )
	{
		enemy->AttachMesh(*m_pEnemyMesh);
	}

	return S_OK;
}

void CGameTest::Start()
{
	m_pDx11->SetDepth(true);
	m_GlobalLight.fIntensity = 100.f;
	m_GlobalLight.vDirection = D3DXVECTOR3(0.0f, -1.0f, 4.0f);
	m_GlobalLight.Position = D3DXVECTOR3(0.0f, 10.0f, -5.0f);

	m_Fog.Enable = false;

	int cnt = 0;
	for( auto& enemy : m_pEnemyList )
	{
		enemy->SetTargetPlayer(m_pPlayer);
		enemy->SetScale(0.005f);
		enemy->SetPosition(D3DXVECTOR3(5*cnt++, 0.0f, 5.0f));
	}
	m_pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	m_pCamera->SetPosition(D3DXVECTOR3(0.0f, 5.0f, -15.0f));
}

void CGameTest::Update()
{
	CScene::Update();
	m_pCamera->Update();
	m_pCameraController->Update(0);


	m_pPlayer->Update();
	//for (auto& enemy : m_pEnemyList)
	//{
	//	enemy->TickAttackTimer(m_pTime->GetDeltaTime());
	//	enemy->Update();
	//}

	static ::EsHandle hEffect = -1;
	if (m_pPlayer->GetPlayerState() == CPlayer::Attacking)
	{
		hEffect = CEffect::Play(CEffect::enList::AttackEffect, m_pPlayer->GetPosition() + D3DXVECTOR3(0.f,0.5f,0.3f));
		CEffect::SetSpeed(hEffect, 1.f);
		CEffect::SetScale(hEffect, D3DXVECTOR3(0.06f, 0.06f, 0.06f));
	}


	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
	m_pCameraController->ThirdPersonCamera(
		playerPos,
		5.f,
		m_mouseDelta,
		m_mouseSense);
	D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
	m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
	m_pPlayer->SetRotation(playerRot);

}

void CGameTest::Draw()
{

	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);

	m_pGroundMeshObject->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	m_pPlayer->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	for (auto& enemy : m_pEnemyList)
	{
		enemy->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

	CEffect::GetInstance()->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera);

}

