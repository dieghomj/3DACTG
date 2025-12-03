#include "CTest.h"
#include "CSoundManager.h"
#include "CMaze.h"
#include "CDebugColliderRender.h"
#include "CEffect.h"
#include "CGameStats.h"
#include <stdio.h>

int currX = 0;
int currY = 0;
// カメラ切り替え用フラグ
//　デバッグのみ使用
bool ghostCamera = false;
bool playerCamera = false;
bool staticCamera = true;

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene				(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pGroundStaticMesh	(nullptr)
	, m_pGround				(nullptr)

	// プレイヤー関連
	, m_pPlayer				(nullptr)
	, m_pWomanMesh			(nullptr)
	, m_pPlayerRayY			(nullptr)
	, m_pCrossRay			{ nullptr, nullptr, nullptr, nullptr }
	, m_pPlayerLight		(nullptr)
	, m_pLightBarSprite		(nullptr)
	, m_pLightBar			(nullptr)

	, m_pHealthBarSprite	(nullptr)
	, m_pHealthBar			(nullptr)

	, m_pTensionSprite		(nullptr)
	, m_pTensionUI			(nullptr)

	//アイテム関連
	, m_TMPItemMesh			(nullptr)
	, m_ItemMeshArray		()

	// ゴースト関連
	, m_pGhostList			()
	, m_pGhostMesh			(nullptr)

	// 迷路関連
	, m_pMazeData			()
	, m_pMazeGen			(nullptr)
	, m_MazeCellH			(8)
	, m_MazeCellW			(8)
	, m_MazeStride			(64)
	, m_MazeCellSize		(1.5 + SEWER_MESHWIDTH)
	, m_EnemyCount			(8)
	, m_EnemiesKilled		(0)
	, m_pZakoMesh			(nullptr)
	, m_pZakoStaticMesh		(nullptr)
	, m_pZakoList			()
	, m_EndSpotLight		(nullptr)

	// 迷路の壁リスト
	, m_pSewerPathArray	()
	, m_pSewerLineMesh		(nullptr)
	, m_pSewerTJunctionMesh	(nullptr)
	, m_pSewerCrossMesh		(nullptr)
	, m_pSewerTurnMesh		(nullptr)
	, m_pSewerEndMesh		(nullptr)
	, m_pWallStaticMesh		(nullptr)

	// デバッグ関連
	, m_SDFText				(nullptr)
	, m_pDbgCollider		(nullptr)
	, m_ShowCollider		(false)
{
	m_pDx11->SetDepth(true);
}


CTest::~CTest()
{

	//TODO:まだ破棄してないオブジェクトを破棄する.
	

	for (auto& path : m_pSewerPathArray)
	{
		delete path;
	}
	m_pSewerPathArray.clear();

	for (auto& ghost : m_pGhostList)
	{
		SAFE_DELETE(ghost);
	}

	delete m_pWallStaticMesh;
	m_pWallStaticMesh = nullptr;
	SAFE_DELETE(m_pPlayer);
	SAFE_DELETE(m_pGround);
	SAFE_DELETE(m_pGroundStaticMesh);
	SAFE_DELETE(m_SDFText);
	SAFE_DELETE(m_pDbgCollider);
}

void CTest::Create()
{
	// フォント作成
	m_SDFText = new CFont();
	
	// カメラ作成
	m_pCamera = new CCamera();
	m_pCamera->SetPerspective(D3DX_PI / 4.0f,
		static_cast<float>(WND_W) / static_cast<float>(WND_H),
		0.1f, 1000.0f);
	m_pCamera->SetPosition(0.f, 5.f, -15.f);
	m_pCameraController = new CCameraController(m_pCamera);

	m_pPlayerLight = new CSpotLight();

	// 地面メッシュ作成
	m_pGroundStaticMesh = new CStaticMesh();
	// 地面オブジェクト作成
	m_pGround = new CStaticMeshObject();		
	
	// 迷路作成
	m_pMazeGen = new CMaze(*m_pMazeData, m_MazeStride, m_MazeCellW, m_MazeCellH);

	m_EndSpotLight = new CSpotLight();
	// 壁メッシュ作成
	m_pWallStaticMesh = new CStaticMesh();

	// 下水道メッシュ作成
	m_pSewerLineMesh = new CStaticMesh();
	m_pSewerTJunctionMesh = new CStaticMesh();
	m_pSewerCrossMesh = new CStaticMesh();
	m_pSewerTurnMesh = new CStaticMesh();
	m_pSewerEndMesh = new CStaticMesh();

	// プレイヤー作成
	m_pPlayer = new CPlayer();
	m_pWomanMesh = new CSkinMesh();
	m_pPlayerRayY = new CRay();
	for( int dir = 0; dir < CROSSRAY::max; dir++ ) {
		m_pCrossRay[dir] = new CRay();
	}

	// 懐中電灯UI作成
	m_pLightBarSprite = new CSprite2D();
	m_pLightBar = new CUIObject();

	//
	m_pHealthBarSprite = new CSprite2D();
	m_pHealthBar = new CUIObject();

	m_pTensionSprite = new CSprite2D();
	m_pTensionUI = new CUIObject();

	// アイテムメッシュ作成
	m_TMPItemMesh = new CStaticMesh();		// アイテムで電灯を回復する予定

	// ゴーストメッシュ作成
	m_pGhostMesh = new CStaticMesh();
	m_pZakoMesh = new CSkinMesh();
	m_pZakoStaticMesh = new CStaticMesh();

	// ゴースト作成
	for (int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		m_pGhostList[i] = new CBaseEnemy();
	}

	for (int i = 0; i < MAX_ENEMY_COUNT; ++i )
	{
		m_pZakoList[i] = new CZako();
	}
	
	m_pDbgCollider = new CDebugColliderRender();

	CEffect::GetInstance()->Create(
		m_pDx11->GetDevice(),
		m_pDx11->GetContext());

}

HRESULT CTest::LoadData()
{
	//Effectクラス
	if (FAILED(CEffect::GetInstance()->LoadData())) {
		return E_FAIL;
	}

	if (FAILED(m_SDFText->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	m_pWomanMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Skin\\WomanAnime\\WomanAnime.X"));

	m_TMPItemMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Item\\Item.X"));

	if (FAILED(m_pGroundStaticMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Ground\\Ground.x"))))
	{
		return E_FAIL;
	}
	
	m_pSewerLineMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Sewer\\sewer_01.x"));
	m_pSewerTurnMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Sewer\\sewer_02.x"));
	m_pSewerTJunctionMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Sewer\\sewer_03.x"));
	m_pSewerCrossMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Sewer\\sewer_04.x"));
	m_pSewerEndMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Sewer\\sewer_05.x"));

	if (FAILED(m_pWallStaticMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Wall\\Wall.x"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGhostMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\EyeBall\\eyeball.x"))))
	{
		return E_FAIL;
	}

	//if(FAILED(m_pZakoMesh->Init(
	//	*m_pDx9, *m_pDx11,
	//	_T("Data\\Mesh\\Skin\\Zako\\zako.x"))))
	//{
	//	return E_FAIL;
	//}

	if(FAILED(m_pZakoStaticMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Skin\\zako\\zako.x"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDbgCollider->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	// 懐中電灯UI設定
	CSprite2D::SPRITE_STATE lightBarState = {
		320.0f, 131.0f,
		320.0f, 516.0f,
		320.0f, 131.0f
	};

	// 体力UI設定
	CSprite2D::SPRITE_STATE healthBarState = {
		300.0f, 100.0f,
		94.f, 64.f,
		94.f, 26.f
	};

	//
	CSprite2D::SPRITE_STATE tensionSpriteState = {
	WND_W, WND_H,
	800.0f, 600.0f,
	800.0f, 600.0f
	};

	if (FAILED(m_pTensionSprite->Init(
		*m_pDx11,
		_T("Data\\Texture\\RED.png"),
		tensionSpriteState)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pLightBarSprite->Init(
		*m_pDx11,
		_T("Data\\Texture\\lantern.png"),
		lightBarState)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pHealthBarSprite->Init(
		*m_pDx11,
		_T("Data\\Texture\\Health.png"),
		healthBarState)))
	{
		return E_FAIL;
	}

	m_pLightBar->AttachSprite(*m_pLightBarSprite);
	m_pHealthBar->AttachSprite(*m_pHealthBarSprite);
	m_pTensionUI->AttachSprite(*m_pTensionSprite);
	//m_pMiniMap->Init(*m_pDx11->GetDevice(), *m_pDx11->GetContext(), m_MazeCellW, m_MazeCellH);

	m_pGround->AttachMesh(*m_pGroundStaticMesh);
	m_pGround->SetPosition(0.f, 0.f, 0.f);

	m_pPlayer->AttachMesh(*m_TMPItemMesh);
	m_pPlayer->AttachSkinMesh(*m_pWomanMesh);
	m_pPlayer->SetRotation(0.f, D3DXToRadian(180.f), 0.f);
	m_pPlayer->SetScale(0.03f);
	m_pPlayer->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);

	RAY	ray = m_pPlayer->GetRayY();
	m_pPlayerRayY->Init(*m_pDx11, ray);

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		ray = m_pPlayer->GetCrossRay().Ray[dir];
		m_pCrossRay[dir]->Init(*m_pDx11, ray);
	}
	
	for(int i = 0; i < MAX_ENEMY_COUNT; ++i)
	{
		m_pGhostList[i]->AttachMesh(*m_pGhostMesh);
		m_pGhostList[i]->SetScale(2.f);
		m_pGhostList[i]->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
		m_pGhostList[i]->SetTargetPlayer(m_pPlayer);
	}

	for (int i = 0; i < m_EnemyCount; ++i)
	{
		m_pZakoList[i]->AttachMesh(*m_pZakoStaticMesh);
		//m_pZakoList[i]->AttachSkinMesh(*m_pZakoMesh);
		m_pZakoList[i]->SetScale(1.0f);
		m_pZakoList[i]->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
	}

	CStaticMeshObject* pSewerLine = new CStaticMeshObject();
	CStaticMeshObject* pSewerTurn = new CStaticMeshObject();
	CStaticMeshObject* pSewerTJunc = new CStaticMeshObject();
	CStaticMeshObject* pSewerCross = new CStaticMeshObject();
	CStaticMeshObject* pSewerEnd = new CStaticMeshObject();

	return S_OK;

}

void CTest::Release()
{
	//
}

void CTest::Start()
{
	switch (CGameStats::GetDifficulty())
	{
	case CGameStats::DIFF_EASY:
		m_MazeCellH = 4;
		m_MazeCellW = 4;
		m_EnemyCount = 4;
		break;
	case CGameStats::DIFF_NORMAL:
		m_MazeCellH = 8;
		m_MazeCellW = 8;
		m_EnemyCount = 8;
		break;
	case CGameStats::DIFF_HARD:
		m_MazeCellH = 12;
		m_MazeCellW = 12;
		m_EnemyCount = 12;
		break;
	}

	ClearMaze();
	//迷路グリッド作成
	m_pMazeGen->SetRegionSize(m_MazeCellW, m_MazeCellH);
	m_pMazeGen->GenerateMaze(0, 0);
	//迷路メッシュ作成
	GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);
	// ゴーストパス再生成
	for (int i = 0; i < m_EnemyCount; ++i)
	{
		auto ghost = m_pGhostList[i];
		ghost->Start();
		ghost->SetWidthHeight(m_MazeCellW, m_MazeCellH);
		ghost->SetRowCol(m_MazeCellH - i, i);
		ghost->SetPath(m_pMazeGen->GeneratePath(ghost->GetCurrentCol(), ghost->GetCurrentRow()));
	}

	m_pPlayer->SetPosition(m_pMazeGen->CellToWorldRC(0, 0, 2.f, m_MazeCellSize));
	m_pPlayer->ApplyHeal(100.0f);
	m_pPlayer->ApplyLightEffect(m_pPlayer->MAX_LIGHT_INT);

	m_pDx11->SetDepth(true);
	// 環境設定
	m_GlobalLight.fIntensity = 0.5f;

	m_Fog.Color = D3DXVECTOR4(0.1f, 0.f, 0.12f, 1.0f);
	m_Fog.Enable = m_bFog;
	m_Fog.Mode = D3DFOG_LINEAR;
	m_Fog.Start = 111;
	m_Fog.End = 0;
	m_Fog.Density = 0.1f;

	m_pCameraController->SetTPOffset(D3DXVECTOR3(0.f, 2.f, -5.f));

	for (int i = 0; i < m_MazeCellH; ++i)
	{
		for (int j = 0; j < m_MazeCellW; ++j)
		{
			D3DXVECTOR3 pos = m_pMazeGen->CellToWorldRC(i, j, 1.f, m_MazeCellSize);
			CStaticMeshObject* itemMeshObj = new CStaticMeshObject;
			itemMeshObj->AttachMesh(*m_TMPItemMesh);
			itemMeshObj->SetPosition(pos);
			itemMeshObj->SetScale(0.2f);
			itemMeshObj->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
			m_ItemMeshArray.push_back(itemMeshObj);
		}
	}
	static ::EsHandle hEffect = -1;
	for (auto& item : m_ItemMeshArray)
	{
		D3DXVECTOR3 pos = item->GetPosition();
		//hEffect = CEffect::Play(CEffect::enList::MagmaEffect, pos);
		//CEffect::SetScale(hEffect, D3DXVECTOR3(0.5f, 0.5f, 0.5f));
	}

	GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);

	m_pPlayerLight->SetPosition(D3DXVECTOR3(0.0f, 15.0f, 0.0f));
	m_pPlayerLight->SetDirection(D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	m_pPlayerLight->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.8f, 1.0f));
	m_pPlayerLight->SetRange(30.0f);
	m_pPlayerLight->SetInnerAngle(D3DXToRadian(15.0f));
	m_pPlayerLight->SetOuterAngle(D3DXToRadian(30.0f));
	m_pPlayerLight->SetIntensity(5.0f);
	if(m_pPlayerLight->GetSceneIndex() != -1)
	{
		UpdateSpotLight(m_pPlayerLight);
	}
	else m_pPlayerLight->SetSceneIndex(AddSpotLight(m_pPlayerLight));

	m_pLightBar->SetScale(0.5);
	m_pLightBar->SetPosition(5.f, WND_H / 2, 0.f);

	m_pHealthBar->SetScale(1);
	m_pHealthBar->SetPosition(5.f, WND_H / 2 - 131.f, 0.f);



	const D3DXVECTOR3 exitPos = m_pMazeGen->GetExitWorldPosition(3.f, m_MazeCellSize);
	m_EndSpotLight->SetPosition(exitPos + D3DXVECTOR3(0.f, 10.f, 0.f));
	m_EndSpotLight->SetDirection(D3DXVECTOR3(0.f, -1.f, 0.f));
	m_EndSpotLight->SetColor(D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f));
	m_EndSpotLight->SetRange(20.0f);
	m_EndSpotLight->SetInnerAngle(D3DXToRadian(15.0f));
	m_EndSpotLight->SetOuterAngle(D3DXToRadian(30.0f));
	m_EndSpotLight->SetIntensity(3.0f);
	if (m_EndSpotLight->GetSceneIndex() != -1)
	{
		UpdateSpotLight(m_EndSpotLight);
	}
	else m_EndSpotLight->SetSceneIndex(AddSpotLight(m_EndSpotLight));
}

void CTest::Update()
{
	CSoundManager::PlayLoop(CSoundManager::BGM_Game);

	m_pCamera->Update();
	m_pCameraController->Update(0);

	CScene::Update();

	m_Fog.Enable = m_bFog;

#if 0 // デバッグ用
	// 迷路再生成
	if (GetAsyncKeyState('R') & 0x0001)
	{
		ClearMaze();
		//迷路グリッド作成
		m_pMazeGen->GenerateMaze(0, 0);
		//迷路メッシュ作成
		GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);
		// ゴーストパス再生成
		for( int i = 0; i < m_EnemyCount; ++i)
		{
			auto ghost = m_pGhostList[i];
			ghost->SetPath(m_pMazeGen->GeneratePath(ghost->GetCurrentCol(), ghost->GetCurrentRow()));
		}
	}
	// デバッグ表示切替
	if (GetAsyncKeyState('1') & 0x0001) // 表示切替
	{
		m_ShowCollider = !m_ShowCollider;
	}
	// フォグ切替
	if (GetAsyncKeyState('2') & 0x0001) // 表示切替
	{
		m_bFog = !m_bFog;
	}
	// カメラ切替
	if (GetAsyncKeyState('B') & 0x0001)
	{
		m_pPlayer->SetRotation(0.f, 0.f, 0.f);
		m_pCamera->ResetCameraRot();
		playerCamera = !playerCamera;
	}
	if (GetAsyncKeyState('V') & 0x0001)
	{
		ghostCamera = !ghostCamera;
	}
	if (GetAsyncKeyState('C') & 0x0001)
	{
		m_pCamera->ResetCameraRot();
		staticCamera = !staticCamera;
	}
#endif

	if (m_pPlayer->GetPlayerHealth() <= 0)
	{
		CGameStats::EnemiesKilled = m_EnemiesKilled;
		CGameStats::TimeMs = m_pTime->GetTotalTime();
		CGameStats::ComputeScore();
		CSoundManager::PlaySE(CSoundManager::SE_GameOver);
		CSoundManager::Stop(CSoundManager::BGM_Game);
		m_pManager->ChangeScene("GAME OVER");
	}
	
	auto playerPos = m_pPlayer->GetPosition();
	if (WorldToMazeCoords(playerPos).x == m_pMazeGen->GetExitCell().x &&
		WorldToMazeCoords(playerPos).y == m_pMazeGen->GetExitCell().y)
	{
		CGameStats::EnemiesKilled = m_EnemiesKilled;
		CGameStats::TimeMs = m_pTime->GetTotalTime();
		CGameStats::ComputeScore();
		CSoundManager::PlaySE(CSoundManager::SE_Result);
		CSoundManager::Stop(CSoundManager::BGM_Game);
		m_pManager->ChangeScene("RESULT");
	}

	// 地面更新
	m_pGround->Update();

	// ゴースト更新
	for (int i = 0; i < m_EnemyCount; ++i)
	{
		if (m_pGhostList[i]->IsDead())
		{
			continue;
		}
		m_pGhostList[i]->Update();

		int currAudio = 0;

		if (m_pGhostList[i]->GetState() == CBaseEnemy::Attack)
		{
			currAudio = CSoundManager::SE_GhostChase;
		}
		else
		{
			currAudio = CSoundManager::SE_GhostIdle;
		}

		CSoundManager::GetInstance()->PlayLoop(CSoundManager::enList(currAudio));

		m_pGhostList[i]->ReactToSpotLight(*m_pPlayerLight);

		if (m_pGhostList[i]->IsDead())
		{
			CSoundManager::Stop(CSoundManager::enList(currAudio));
			CSoundManager::PlaySE(CSoundManager::SE_GhostDead);
			OnEnemyKilled();
		}
	}

	 //ゴーストカメラ
	if (ghostCamera)
	{
		EnemyCamera();

		return;
	}
	
	// プレイヤーカメラ
	if (playerCamera)
	{
		UpdatePlayerCamera();

		//
		m_pPlayerLight->SetPosition(m_pPlayer->GetPosition() + D3DXVECTOR3(0.0f, 1.5f, 0.0f));
		m_pPlayerLight->SetDirection(m_pCameraController->GetForward());

		if (m_pPlayer->IsFlashOn())
		{
			m_pPlayerLight->SetIntensity(m_pPlayer->GetLightIntensity());
		}
		else
		{
			m_pPlayerLight->SetIntensity(0.0f);
		}

		UpdateSpotLight(m_pPlayerLight);

		return;
	}

	// スタティックカメラ
	if( staticCamera )
	{
		m_pPlayer->SetTankControlMode(true);
		bool prevFlashState = m_pPlayer->IsFlashOn();
		m_pPlayer->Update();
		
		if (m_pPlayer->GetPlayerHealth() <= 50)
		{
			m_pTensionUI->SetAlpha((50 - m_pPlayer->GetPlayerHealth()) / 50);
		}
		else
		{
			m_pTensionUI->SetAlpha(0.0f);
		}
		
		UpdateStaticCamera();
	
		
		D3DXVECTOR3 lightOffset = D3DXVECTOR3(0.0f, 0.5f, 0.0f);
		
		D3DXVECTOR3 playerFwd = m_pPlayer->GetDirection();
		lightOffset += playerFwd * -0.2f ;

		m_pPlayerLight->SetPosition(m_pPlayer->GetPosition() + lightOffset);
		m_pPlayerLight->SetDirection(m_pPlayer->GetDirection());

		if (prevFlashState != m_pPlayer->IsFlashOn())
		{
			CSoundManager::PlaySE(CSoundManager::SE_Flashlight);
		}

		if (m_pPlayer->IsFlashOn())
		{
			
			m_pPlayerLight->SetIntensity(m_pPlayer->GetLightIntensity());
		}
		else
		{
			m_pPlayerLight->SetIntensity(0.0f);
		}
		UpdateSpotLight(m_pPlayerLight);
		return;
	}

	UpdateFPCamera();

}

void CTest::EnemyCamera()
{
	m_pCameraController->ThirdPersonCamera(
		m_pGhostList[0]->GetPosition(),
		5.f,
		m_mouseDelta,
		m_mouseSense);
	D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
	m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
	m_pPlayer->SetRotation(playerRot);
	m_pPlayerLight->SetIntensity(0.f);
	UpdateSpotLight(m_pPlayerLight);
}

void CTest::Draw()
{

	m_pCamera->Draw(m_SceneInfo);
	m_pGround->Draw(m_SceneInfo);
	//m_pWomanMesh->Render(m_mView, m_mProj, m_GlobalLight, m_Camera.vPosition, m_Fog);
	m_pPlayer->Draw(m_SceneInfo);
	m_pPlayer->UpdateCollider();
	//レイの描画
	m_pPlayerRayY->Render(m_mView, m_mProj, m_pPlayer->GetRayY());
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir]->Render(
			m_mView, m_mProj, m_pPlayer->GetCrossRay().Ray[dir]);
	}

	// 迷路壁描画
	for (auto& path : m_pSewerPathArray)
	{
		path->Draw(m_SceneInfo);
	}
	// ゴースト描画
	for (int i = 0; i < m_EnemyCount; ++i)
	{
		if (m_pGhostList[i]->IsDead())
		{
			continue;
		}
		m_pGhostList[i]->UpdateCollider();
		m_pGhostList[i]->RenderStatic(m_SceneInfo);
		auto playerCol = m_pPlayer->GetCollider()->GetBSphere();
		bool isHit = m_pGhostList[i]->GetCollider()->GetBSphere()->IsHit(*playerCol);
		if (isHit)
		{
			m_pPlayer->ApplyDamage(0.8f);
		}
	}
	CEffect::GetInstance()->Draw(m_SceneInfo);

	// アイテム描画
	for (auto& item : m_ItemMeshArray)
	{
		item->Draw(m_SceneInfo);
		item->UpdateCollider();
		auto playerCol = m_pPlayer->GetCollider()->GetBSphere();
		bool isHit = item->GetCollider()->GetBSphere()->IsHit(*playerCol);
		if (isHit)
		{
			m_pPlayer->ApplyLightEffect(0.5f);
			CSoundManager::PlaySE(CSoundManager::SE_ItemGet);
			item->SetPosition(D3DXVECTOR3(1000.f, 1000.f, 1000.f)); // 遠くに移動して消す
		}
	}


#if 0
#pragma region COLLIDER_DEBUG_DRAW
	// コライダーのデバッグ描画
	// デバッグ表示が有効な場合
	if (m_pDbgCollider && m_ShowCollider)
	{

		m_pDx11->SetDepth(false);
		for (auto& wall : m_pSewerPathArray)
		{
			if (auto* col = wall->GetCollider())
			{
				m_pDbgCollider->DrawCollider(*m_pDx11, m_mView, m_mProj,
					CCollider::COLLIDER_SHAPE_BOX, *col);
			}
		}

		for (int i = 0; i < m_EnemyCount; ++i)
		{
			if (auto* col = m_pGhostList[i]->GetCollider())
			{
				m_pDbgCollider->DrawCollider(*m_pDx11, m_mView, m_mProj,
					CCollider::COLLIDER_SHAPE_SPHERE, *col);
			}
		}

		m_pDx11->SetDepth(true);
	}
#pragma endregion
#endif

	//UI
	m_pDx11->SetDepth(false);
	m_pLightBar->SetFillPercent(1, true);
	m_pLightBar->SetAlpha(0.5f);
	m_pLightBar->Draw();
	m_pLightBar->SetAlpha(1.f);
	m_pLightBar->SetFillPercent(m_pPlayer->GetLightIntensity() / m_pPlayer->MAX_LIGHT_INT, true);
	m_pLightBar->Draw();

	m_pHealthBar->SetPatternNo(0, 0);
	m_pHealthBar->SetFillPercent(1, true);
	m_pHealthBar->SetAlpha(0.5f);
	m_pHealthBar->Draw();
	m_pHealthBar->SetAlpha(1.f);
	m_pHealthBar->SetFillPercent(m_pPlayer->GetPlayerHealth()/ 100.f, true);
	m_pHealthBar->Draw();

	m_pTensionUI->Draw();

	// テキスト描画
	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);      

	TCHAR text[64];
	
	//Pair playerRC = WorldToMazeCoords(m_pPlayer->GetPosition());
	//Pair cameraRC = WorldToMazeCoords(m_pCamera->GetPosition());
	//_stprintf_s(text, _T("PLAYER MAZE COORDS:%d,%d"), playerRC.x, playerRC.y);
	//m_SDFText->Render(text, 50, 50, 30.f);
	//_stprintf_s(text, _T("CAMERA MAZE COORDS:%d,%d"), cameraRC.x, cameraRC.y);
	//m_SDFText->Render(text, 50, 80, 30.f);
	//m_SDFText->Render(_T("PRESS R TO REGENERATE MAZE"), 50, 700, 25.f);

	float sec = m_pTime->GetTotalTime() / 1000.f;
	float min = sec / 60.f;
	float remSec = sec - (static_cast<int>(min) * 60.f);

	_stprintf_s(text, _T("%02.f:%02.f"), min, remSec);
	m_SDFText->Render(text, WND_W/2 - 70.f, 50, 70.f);

	//if(ghostCamera)
	//	m_SDFText->Render(_T("GHOST CAMERA"), 800, 50, 30.f);
	//else if (playerCamera)
	//	m_SDFText->Render(_T("PLAYER CAMERA"), 800, 80, 30.f);
	//else if (staticCamera)
	//	m_SDFText->Render(_T("STATIC CAMERA"), 800, 110, 30.f);
	//else
	//	m_SDFText->Render(_T("FIRST PERSON CAMERA"), 800, 140, 30.f);


	/*if (m_ShowCollider) {
		m_SDFText->Render(_T("COLLIDERS: ON"), 800, 170, 30.f);
	}
	else {
		m_SDFText->Render(_T("COLLIDERS: OFF"), 800, 170, 30.f);
	}*/
	//for (int i = 0; i < CGlobal::debugText.size(); i++)
	//{
	//	auto text = CGlobal::debugText[i];
	//	m_SDFText->Render(text, 200, 110 + i * 30.f, 30.f);
	//}

	m_pDx11->SetDepth(true);

}	

void CTest::GenerateMazeMeshObj(int regionHeight, int regionWidth, int stride)
{
	const float wallHeight = 5.0f;	// 壁の高さ

	// 迷路の壁を配置
	for (int i = 0; i < regionHeight; ++i)
	{
		for (int j = 0; j < regionWidth; ++j)
		{
			D3DXVECTOR3 pos = m_pMazeGen->CellToWorldRC(i, j, 0.f, m_MazeCellSize);
			// 壁の位置計算
			float x = pos.x;	// X座標
			float z = pos.z; // Z座標
			CSewerPath* path = new CSewerPath();
			path->SetMazeCoords(i, j);
			path->SetPlayer(*m_pPlayer);

			unsigned bitCount = 4 - __popcnt(m_pMazeData[i][j]);
			int mazeData = m_pMazeData[i][j];

			switch (bitCount)
			{
				case 0: // 十字路
				{
					path->SetType(CSewerPath::CROSS);	
					path->AttachMesh(*m_pSewerCrossMesh);
					path->SetPosition(x, wallHeight-1, z);
					break;
				}

				case 1: // T字路
				{
					path->SetType(CSewerPath::T_JUNCTION);
					path->AttachMesh(*m_pSewerTJunctionMesh);
					path->SetPosition(x, wallHeight, z);
					if (!(mazeData & CMaze::North))
					{
						path->SetRotation(0, -D3DX_PI / 2.0f, 0);
					}
					else if (!(mazeData & CMaze::South))
					{
						path->SetRotation(0, D3DX_PI / 2.0f, 0);
					}
					else if (!(mazeData & CMaze::West))
					{
						path->SetRotation(0, D3DX_PI, 0);
					}
					break;
				}

				case 2:
					// 直線 or 曲がり角
				{
					if ((mazeData & CMaze::North && mazeData & CMaze::South) ||
						(mazeData & CMaze::East && mazeData & CMaze::West))
					{
						// 直線
						path->SetType(CSewerPath::LINE);
						path->AttachMesh(*m_pSewerLineMesh);
						path->SetPosition(x, wallHeight, z);
						if (mazeData & CMaze::East && mazeData & CMaze::West)
						{
							path->SetRotation(0, D3DX_PI / 2.0f, 0);
						}
					}
					else
					{
						// 曲がり角
						path->SetType(CSewerPath::TURN);
						path->AttachMesh(*m_pSewerTurnMesh);
						path->SetPosition(x, wallHeight, z);
						if (mazeData & CMaze::North && mazeData & CMaze::East)
						{
							path->SetRotation(0, D3DX_PI, 0);
						}
						else if (mazeData & CMaze::East && mazeData & CMaze::South)
						{
							path->SetRotation(0, -D3DX_PI / 2.0f, 0);
						}
						else if (mazeData & CMaze::North && mazeData & CMaze::West)
						{
							path->SetRotation(0, D3DX_PI / 2.0f, 0);
						}
					}
					break;
				}

				case 3: // 行き止まり
				{
					path->SetType(CSewerPath::END);	
					path->AttachMesh(*m_pSewerEndMesh);
					path->SetPosition(x, wallHeight, z);
					if (mazeData & CMaze::North)
					{
						path->SetRotation(0, D3DX_PI, 0);
					}
					else if (mazeData & CMaze::West)
					{
						path->SetRotation(0, D3DX_PI / 2.0f, 0);
					}
					else if (mazeData & CMaze::South)
					{
						path->SetRotation(0, 0, 0);
					}
					else if (mazeData & CMaze::East)
					{
						path->SetRotation(0, -D3DX_PI / 2.0f, 0);
					}
					break;
				}
			}

			m_pSewerPathArray.push_back(path);
		}
	}


	for (int i = 0; i < m_pSewerPathArray.size(); ++i)
	{
		m_pSewerPathArray[i]->CreateCollider(CCollider::COLLIDER_SHAPE_BOX);
	}
}

void CTest::ClearMaze()
{
	for (auto& path : m_pSewerPathArray)
	{
		delete path;
	}
	m_pSewerPathArray.clear();
}

Pair CTest::NextMazePosition()
{

	if (currX >= m_MazeCellW)
	{
		currX = 0;
		currY++;
		if (currY >= m_MazeCellH)
		{
			currY = 0;
		}

		return { currY, currX };
	}

	currX++;
	return { currY, currX };

}

Pair CTest::WorldToMazeCoords(const D3DXVECTOR3& worldPos)
{
	// 迷路の原点を取得 (0,0セルのワールド座標)
	D3DXVECTOR3 origin = m_pMazeGen->CellToWorldRC(0, 0, 0.f, m_MazeCellSize);

	// ワールド座標を迷路のローカル座標に変換
	D3DXVECTOR3 offset = D3DXVECTOR3(-m_MazeCellSize / 2.0f, 0.f, m_MazeCellSize / 2.0f);
	D3DXVECTOR3 localPos = worldPos - (origin + offset);

	// ローカル座標をグリッド座標に変換
	float col = localPos.x / m_MazeCellSize;
	float row = -localPos.z / m_MazeCellSize;

	// 迷路の範囲内にクランプ
	col = max(0.0f, min(static_cast<float>(m_MazeCellW - 1), col));
	row = max(0.0f, min(static_cast<float>(m_MazeCellH - 1), row));

	// 整数のグリッド座標として返す
	return { static_cast<int>(row), static_cast<int>(col) };
}

void CTest::UpdateFPCamera()
{
	// 通常カメラ
	m_pCameraController->FirstPersonCamera(
		m_mouseDelta,
		m_mouseSense);
	//D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
	//m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
	//m_pPlayer->SetRotation(playerRot);
}

void CTest::UpdateStaticCamera()
{

	Pair playerRC = WorldToMazeCoords(m_pPlayer->GetPosition());
	D3DXVECTOR3 staticCamPos = m_pMazeGen->CellToWorldRC(playerRC.x, playerRC.y, 2.5f, m_MazeCellSize);
	D3DXVECTOR3 playerFwd = m_pPlayer->GetDirection();
	D3DXVECTOR3 statiCamOffset = playerFwd * -2.f;
	statiCamOffset.y += 2.f;
	statiCamOffset.x += -2.f;
	D3DXVECTOR3 forwardOffset = playerFwd * 2.f;

	m_pCamera->SetPosition(staticCamPos + statiCamOffset);
	m_pCameraController->StaticCamera(
		m_pPlayer->GetPosition() + forwardOffset,
		m_mouseDelta,
		m_mouseSense);

	m_pCamera->SetLens(D3DX_PI / 2.50f,
		static_cast<float>(WND_W) / static_cast<float>(WND_H),
		0.1f, 1000.0f);

	for (auto& path : m_pSewerPathArray)
	{
		Pair pathCoords = path->GetMazeCoords();
		Pair playerCoords = WorldToMazeCoords(m_pPlayer->GetPosition());
		if ((pathCoords.x == playerCoords.x) && (pathCoords.y == playerCoords.y))
			path->Update();
	}
	return;
}

void CTest::UpdatePlayerCamera()
{
	m_pPlayer->Update();
	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
	m_pCameraController->ThirdPersonCamera(
		playerPos,
		5.f,
		m_mouseDelta,
		m_mouseSense);
	D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
	m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
	m_pPlayer->SetRotation(playerRot);
	for (auto& path : m_pSewerPathArray)
	{
		Pair pathCoords = path->GetMazeCoords();
		Pair playerCoords = WorldToMazeCoords(m_pPlayer->GetPosition());
		if ((pathCoords.x == playerCoords.x) && (pathCoords.y == playerCoords.y))
			path->Update();
	}

	return;
}