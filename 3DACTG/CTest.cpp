#include "CTest.h"
#include "CSoundManager.h"
#include "CMaze.h"
#include "CDebugColliderRender.h"
#include <stdio.h>

#define ENEMY_COUNT 7
#define SEWER_MESHWIDTH 10.5f 

int currX = 0;
int currY = 0;
bool ghostCamera = false;
bool playerCamera = false;
bool staticCamera = false;

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene				(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pGroundStaticMesh	(nullptr)
	, m_pGround				(nullptr)

	// プレイヤー関連
	, m_pPlayer				(nullptr)
	, m_pWomanMesh			(nullptr)
	, m_pPlayerRayY			(nullptr)

	, m_TMPItemMesh			(nullptr)
	, m_ItemMeshArray		()


	// ゴースト関連
	, m_pGhostList			()
	, m_pGhostMesh			(nullptr)

	// 迷路関連
	, m_pMazeData			()
	, m_pMazeGen			(nullptr)
	, m_MazeCellH			(7)
	, m_MazeCellW			(7)
	, m_MazeStride			(64)
	, m_MazeCellSize		(1.5 + SEWER_MESHWIDTH)
	
	// 迷路の壁リスト
	, m_pSewerPathArray	()
	, m_pSewerLineMesh		(nullptr)
	, m_pSewerTJunctionMesh	(nullptr)
	, m_pSewerCrossMesh		(nullptr)
	, m_pSewerTurnMesh		(nullptr)
	, m_pSewerEndMesh		(nullptr)
	, m_pWallStaticMesh		(nullptr)
	, m_pMiniMapUI			(nullptr)
	, m_pMiniMapSprite		(nullptr)
	, m_pMiniMap			(nullptr)
	, m_miniMapStartX		(20)
	, m_miniMapStartY		(20)
	, m_miniMapCellSize		(9)
	, m_miniMapFontSize		(11)

	// デバッグ関連
	, m_SDFText				(nullptr)
	, m_pDbgCollider		(nullptr)
	, m_ShowCollider		(false)
{
	m_pDx11->SetDepth(true);
}


CTest::~CTest()
{
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

	// 地面メッシュ作成
	m_pGroundStaticMesh = new CStaticMesh();
	// 地面オブジェクト作成
	m_pGround = new CStaticMeshObject();		
	
	// 迷路作成
	m_pMazeGen = new CMaze(*m_pMazeData, m_MazeStride, m_MazeCellW, m_MazeCellH);

	// 壁メッシュ作成
	m_pWallStaticMesh = new CStaticMesh();

	m_pSewerLineMesh = new CStaticMesh();
	m_pSewerTJunctionMesh = new CStaticMesh();
	m_pSewerCrossMesh = new CStaticMesh();
	m_pSewerTurnMesh = new CStaticMesh();
	m_pSewerEndMesh = new CStaticMesh();

	m_pMiniMap = new CMiniMapTexture();
	m_pMiniMapSprite = new CSprite2D();
	m_pMiniMapUI = new CUIObject();

	// プレイヤー作成
	m_pPlayer = new CPlayer();
	m_pWomanMesh = new CStaticMesh();
	m_pPlayerRayY = new CRay();
	for( int dir = 0; dir < CROSSRAY::max; dir++ ) {
		m_pCrossRay[dir] = new CRay();
	}

	m_TMPItemMesh = new CStaticMesh();

	// ゴーストメッシュ作成
	m_pGhostMesh = new CStaticMesh();
	// ゴースト作成
	for (int i = 0; i < ENEMY_COUNT; ++i)
	{
		m_pGhostList[i] = new CBaseEnemy(m_pMazeGen->GeneratePath(i, (m_MazeCellH - 1) - i), m_MazeCellW, m_MazeCellH);
	}
	
	m_pDbgCollider = new CDebugColliderRender();

}

HRESULT CTest::LoadData()
{
	HRESULT hr = S_OK;

	if (FAILED(m_SDFText->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	m_pWomanMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Skin\\Woman\\PSXVillageWoman.x"));

	m_TMPItemMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\DebugSphere.x"));

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
		_T("Data\\Mesh\\Static\\Ghost\\GhostB\\Ghost.X"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pDbgCollider->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	//m_pMiniMap->Init(*m_pDx11->GetDevice(), *m_pDx11->GetContext(), m_MazeCellW, m_MazeCellH);

	m_pGround->AttachMesh(*m_pGroundStaticMesh);
	m_pGround->SetPosition(0.f, 0.f, 0.f);

	m_pPlayer->AttachMesh(*m_pWomanMesh);
	m_pPlayer->SetScale(0.6f);
	m_pPlayer->SetPosition(m_pMazeGen->CellToWorldRC(0,0,2.f,m_MazeCellSize));

	RAY	ray = m_pPlayer->GetRayY();

	m_pPlayerRayY->Init(*m_pDx11, ray);

	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		ray = m_pPlayer->GetCrossRay().Ray[dir];
		m_pCrossRay[dir]->Init(*m_pDx11, ray);
	}
	
	for(int i = 0; i < ENEMY_COUNT; ++i)
	{
		m_pGhostList[i]->AttachMesh(*m_pGhostMesh);
		m_pGhostList[i]->SetScale(0.09f);
		m_pGhostList[i]->CreateCollider(CCollider::COLLIDER_SHAPE_BOX);
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
	
}

void CTest::Start()
{
	m_pDx11->SetDepth(true);
	// 環境設定
	m_GlobalLight.fIntensity = 1.f;

	m_Fog.Color = D3DXVECTOR4(0.066f, 0.078f, 0.065f, 1.0f);
	m_Fog.Enable = m_bFog;
	m_Fog.Mode = D3DFOG_LINEAR;
	m_Fog.Start = 5.0f;
	m_Fog.End = 150.0f;
	m_Fog.Density = 0.1f;

	for (int i = 0; i < m_MazeCellH; ++i)
	{
		for (int j = 0; j < m_MazeCellW; ++j)
		{
			D3DXVECTOR3 pos = m_pMazeGen->CellToWorldRC(i, j, 3.f, m_MazeCellSize);
			CStaticMeshObject* itemMeshObj = new CStaticMeshObject;
			itemMeshObj->AttachMesh(*m_TMPItemMesh);
			itemMeshObj->SetPosition(pos);
			itemMeshObj->SetScale(0.2f);
			itemMeshObj->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
			m_ItemMeshArray.push_back(itemMeshObj);
		}
	}

	GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);
}

void CTest::Update()
{

	m_pCamera->Update();
	m_pCameraController->Update(0);

	CScene::Update();

	m_Fog.Enable = m_bFog;

	// 迷路再生成
	if (GetAsyncKeyState('R') & 0x0001)
	{
		ClearMaze();
		//迷路グリッド作成
		m_pMazeGen->GenerateMaze(0, 0);
		//迷路メッシュ作成
		GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);
		// ゴーストパス再生成
		for( int i = 0; i < ENEMY_COUNT; ++i)
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

	// 地面更新
	m_pGround->Update();

	// ゴースト更新
	for (int i = 0; i < ENEMY_COUNT; ++i)
	{
		m_pGhostList[i]->Update();
	}

	// ゴーストカメラ
	if (ghostCamera)
	{
		m_pCameraController->ThirdPersonCamera(
			m_pGhostList[0]->GetPosition(),
			5.f,
			m_mouseDelta,
			m_mouseSense);
		D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
		m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
		m_pPlayer->SetRotation(playerRot);

		return;
	}

	// プレイヤーカメラ
	if (playerCamera)
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
	// スタティックカメラ
	if( staticCamera )
	{
		m_pPlayer->Update();
		Pair playerRC = WorldToMazeCoords(m_pPlayer->GetPosition());
		D3DXVECTOR3 staticCamPos = m_pMazeGen->CellToWorldRC(playerRC.x, playerRC.y, 8.f, m_MazeCellSize);
		m_pCamera->SetPosition(staticCamPos);
		
		m_pCameraController->StaticCamera(
			m_pPlayer->GetPosition(),
			m_mouseDelta,
			m_mouseSense);
		for (auto& path : m_pSewerPathArray)
		{
			Pair pathCoords = path->GetMazeCoords();
			Pair playerCoords = WorldToMazeCoords(m_pPlayer->GetPosition());
			if ((pathCoords.x == playerCoords.x) && (pathCoords.y == playerCoords.y))		
			path->Update();
		}
		return;
	}
	// 通常カメラ
	m_pCameraController->FirstPersonCamera(
		m_mouseDelta,
		m_mouseSense);
	//D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
	//m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
	//m_pPlayer->SetRotation(playerRot);

}

void CTest::Draw()
{

	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	m_pGround->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	//m_pWomanMesh->Render(m_mView, m_mProj, m_GlobalLight, m_Camera.vPosition, m_Fog);
	m_pPlayer->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);

	//レイの描画
	m_pPlayerRayY->Render(m_mView, m_mProj, m_pPlayer->GetRayY());
	for (int dir = 0; dir < CROSSRAY::max; dir++) {
		m_pCrossRay[dir]->Render(
			m_mView, m_mProj, m_pPlayer->GetCrossRay().Ray[dir]);
	}

	// 迷路壁描画
	for (auto& path : m_pSewerPathArray)
	{
		path->UpdateCollider();
		path->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}
	// ゴースト描画
	for( int i = 0; i < ENEMY_COUNT; ++i)
	{
		m_pGhostList[i]->UpdateCollider();
		m_pGhostList[i]->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}
	// アイテム描画
	for (auto& item : m_ItemMeshArray)
	{
		item->UpdateCollider();
		item->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

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

		for( int i = 0; i < ENEMY_COUNT; ++i)
		{
			if (auto* col = m_pGhostList[i]->GetCollider())
			{
				m_pDbgCollider->DrawCollider(*m_pDx11, m_mView, m_mProj,
					CCollider::COLLIDER_SHAPE_BOX, *col);
			}
		}

		for (auto& item : m_ItemMeshArray)
		{
			if (auto* col = item->GetCollider())
			{
				m_pDbgCollider->DrawCollider(*m_pDx11, m_mView, m_mProj,
					CCollider::COLLIDER_SHAPE_SPHERE, *col);
			}
		}

		m_pDx11->SetDepth(true);
#pragma endregion

	}

	// テキスト描画
	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);      

	TCHAR text[64];
	
	//DrawTextMinimap();
	Pair playerRC = WorldToMazeCoords(m_pPlayer->GetPosition());
	Pair cameraRC = WorldToMazeCoords(m_pCamera->GetPosition());
	_stprintf_s(text, _T("PLAYER MAZE COORDS:%d,%d"), playerRC.x, playerRC.y);
	m_SDFText->Render(text, 50, 50, 30.f);
	_stprintf_s(text, _T("CAMERA MAZE COORDS:%d,%d"), cameraRC.x, cameraRC.y);
	m_SDFText->Render(text, 50, 80, 30.f);
	m_SDFText->Render(_T("PRESS R TO REGENERATE MAZE"), 50, 700, 25.f);
	
	if(ghostCamera)
		m_SDFText->Render(_T("GHOST CAMERA"), 800, 50, 30.f);
	else if (playerCamera)
		m_SDFText->Render(_T("PLAYER CAMERA"), 800, 80, 30.f);
	else if (staticCamera)
		m_SDFText->Render(_T("STATIC CAMERA"), 800, 110, 30.f);
	else
		m_SDFText->Render(_T("FIRST PERSON CAMERA"), 800, 140, 30.f);

	//for (int i = 0; i < CGlobal::debugText.size(); i++)
	//{
	//	auto text = CGlobal::debugText[i];
	//	m_SDFText->Render(text, 200, 110 + i * 30.f, 30.f);
	//}

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

void CTest::DrawTextMinimap()
{
	// --- MiniMap (文字) ---
		// レイアウト
	const int regionH = m_MazeCellH;	// 高さ
	const int regionW = m_MazeCellW;	// 幅

	TCHAR text[64];

	// 迷路描画
	for (int i = 0; i < regionH; ++i)
	{
		for (int j = 0; j < regionW; ++j)
		{
			float cx = m_miniMapStartX + j * m_miniMapCellSize;
			float cy = m_miniMapStartY + i * m_miniMapCellSize;

			// セールの真ん中に空白を描画
			_stprintf_s(text, _T(" "));
			m_SDFText->Render(text, cx, cy, m_miniMapFontSize);

			// 北壁 (セルの上に'_'を描画)
			if (!(m_pMazeData[i][j] & CMaze::North))
			{
				_stprintf_s(text, _T("_"));
				m_SDFText->Render(text, cx, cy - m_miniMapCellSize * 0.5f, m_miniMapFontSize);
			}
			// 西壁 (セルの左に'|'を描画)
			if (!(m_pMazeData[i][j] & CMaze::West))
			{
				_stprintf_s(text, _T("|"));
				m_SDFText->Render(text, cx - m_miniMapCellSize * 0.5f, cy, m_miniMapFontSize);
			}
			// 南壁を描画
			if (!(m_pMazeData[i][j] & CMaze::South))
			{
				_stprintf_s(text, _T("_"));
				m_SDFText->Render(text, cx, cy + m_miniMapCellSize * 0.5f, m_miniMapFontSize);
			}
			// 東壁 (セルの右に'|'を描画)
			if (!(m_pMazeData[i][j] & CMaze::East))
			{
				_stprintf_s(text, _T("|"));
				m_SDFText->Render(text, cx + m_miniMapCellSize * 0.5f, cy, m_miniMapFontSize);
			}
		}
	}


	//// プレイヤー位置描画
	//{
	//	Pair playerMazePos = WorldToMazeCoords(m_pPlayer->GetPosition());

	//	float pcx = m_miniMapStartX + playerMazePos.x * m_miniMapCellSize;
	//	float pcy = m_miniMapStartY + playerMazePos.y * m_miniMapCellSize;
	//	_stprintf_s(text, _T("*"));

	//	m_SDFText->SetColor(1.0f, 0.0f, 0.0f);
	//	m_SDFText->Render(text, pcx, pcy, m_miniMapFontSize);

	//	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);
	//}
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