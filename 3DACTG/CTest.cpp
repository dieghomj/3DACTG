#include "CTest.h"
#include "CSoundManager.h"
#include "CMaze.h"
#include "CDebugColliderRender.h"
#include <stdio.h>

#define ENEMY_COUNT 7
#define SEWER_MESHWIDTH 10.5f 

int currX = 0;
int currY = 0;

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene				(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pGroundStaticMesh	(nullptr)
	, m_pGround				(nullptr)

	// プレイヤー関連
	, m_pPlayer				(nullptr)
	, m_pWomanMesh			(nullptr)

	// ゴースト関連
	, m_pGhostList			()
	, m_pGhostMesh			(nullptr)

	// 迷路関連
	, m_pMazeData			()
	, m_pMazeGen			(nullptr)
	, m_MazeCellH			(7)
	, m_MazeCellW			(7)
	, m_MazeStride			(64)
	, m_MazeCellSize		(1 + SEWER_MESHWIDTH)
	// 迷路の壁リスト

	, m_pMazeMeshObjArray	()
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

	, m_SDFText				(nullptr)
	, m_pDbgCollider		(nullptr)
	, m_ShowCollider		(false)
{
	m_pDx11->SetDepth(true);
}


CTest::~CTest()
{
	for (auto& wall : m_pMazeMeshObjArray)
	{
		delete wall;
	}
	m_pMazeMeshObjArray.clear();

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
	m_pPlayer->SetPosition(0.f,0.f,0.f);

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
	// 環境設定
	m_GlobalLight.fIntensity = 1.f;

	m_Fog.Color = D3DXVECTOR4(0.066f, 0.078f, 0.065f, 1.0f);
	m_Fog.Enable = m_bFog;
	m_Fog.Mode = D3DFOG_LINEAR;
	m_Fog.Start = 10.0f;
	m_Fog.End = 150.0f;
	m_Fog.Density = 0.08f;

	//GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);
}

void CTest::Update()
{

	m_pCamera->Update();
	m_pCameraController->Update(0);


	CScene::Update();

	m_Fog.Enable = m_bFog;

	if (GetAsyncKeyState('R') & 0x0001)
	{
		ClearMaze();
		m_pMazeGen->GenerateMaze(0, 0);
		GenerateMazeMeshObj(m_MazeCellH, m_MazeCellW, m_MazeStride);
		for( int i = 0; i < ENEMY_COUNT; ++i)
		{
			auto ghost = m_pGhostList[i];
			ghost->SetPath(m_pMazeGen->GeneratePath(ghost->GetCurrentCol(), ghost->GetCurrentRow()));
		}
	}
	if (GetAsyncKeyState('1') & 0x0001) // 表示切替
	{
		m_ShowCollider = !m_ShowCollider;
	}
	if (GetAsyncKeyState('2') & 0x0001) // 表示切替
	{
		m_bFog = !m_bFog;
	}
	if (GetAsyncKeyState('V') & 0x0001)
	{
		m_pPlayer->SetPosition(m_pGhostList[0]->GetPosition());
	}
	if (GetAsyncKeyState('C') & 0x0001)
	{
		Pair np = NextMazePosition();
		m_pPlayer->SetPosition(m_pMazeGen->CellToWorldRC(np.y, np.x, 2.f, m_MazeCellSize));
	}


	m_pGround->Update();

	m_pPlayer->Update();

	for (int i = 0; i < ENEMY_COUNT; ++i)
	{
		m_pGhostList[i]->Update();
	}

	for (auto& wall : m_pMazeMeshObjArray)
	{
		wall->Update();
	}

	D3DXVECTOR3 playerPos = m_pPlayer->GetPosition();
	m_pCameraController->ThirdPersonCamera(
		playerPos,
		m_mouseDelta,
		m_mouseSense);
	//D3DXVECTOR3 playerRot = m_pPlayer->GetRotation();
	//m_pCameraController->UpdateObjectRotationFromCamera(&playerRot);
	//m_pPlayer->SetRotation(playerRot);

}

void CTest::Draw()
{

	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	//m_pWomanMesh->Render(m_mView, m_mProj, m_GlobalLight, m_Camera.vPosition, m_Fog);
	m_pPlayer->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	m_pGround->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);


	for (auto& wall : m_pMazeMeshObjArray)
	{
		wall->UpdateCollider();
		wall->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

	for( int i = 0; i < ENEMY_COUNT; ++i)
	{
		m_pGhostList[i]->UpdateCollider();
		m_pGhostList[i]->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

	if (m_pDbgCollider && m_ShowCollider)
	{
		m_pDx11->SetDepth(false);
		for (auto& wall : m_pMazeMeshObjArray)
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

		m_pDx11->SetDepth(true);
	}

	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);      

	TCHAR text[64];
	
	DrawTextMinimap();

	// --- マウス移動量表示 ---
	//_stprintf_s(text, _T(" DELTA: (%d,%d)"), m_mouseDelta.x, m_mouseDelta.y);
	//m_SDFText->Render(text, 50, 50, 30.f);
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

			unsigned bitCount = 4 - __popcnt(m_pMazeData[i][j]);
			int mazeData = m_pMazeData[i][j];

			switch (bitCount)
			{
				case 0: // 十字路
				{
					CStaticMeshObject* wall = new CStaticMeshObject();
					wall->AttachMesh(*m_pSewerCrossMesh);
					wall->SetPosition(x, wallHeight, z);
					m_pMazeMeshObjArray.push_back(wall);
					break;
				}

				case 1: // T字路
				{
					CStaticMeshObject* wall = new CStaticMeshObject();
					wall->AttachMesh(*m_pSewerTJunctionMesh);
					wall->SetPosition(x, wallHeight, z);
					if (!(mazeData & CMaze::North))
					{
						wall->SetRotation(0, -D3DX_PI / 2.0f, 0);
					}
					else if (!(mazeData & CMaze::South))
					{
						wall->SetRotation(0, D3DX_PI / 2.0f, 0);
					}
					else if (!(mazeData & CMaze::West))
					{
						wall->SetRotation(0, D3DX_PI, 0);
					}
					m_pMazeMeshObjArray.push_back(wall);
					break;
				}

				case 2:
					// 直線 or 曲がり角
				{
					if ((mazeData & CMaze::North && mazeData & CMaze::South) ||
						(mazeData & CMaze::East && mazeData & CMaze::West))
					{
						// 直線
						CStaticMeshObject* wall = new CStaticMeshObject();
						wall->AttachMesh(*m_pSewerLineMesh);
						wall->SetPosition(x, wallHeight, z);
						if (mazeData & CMaze::East && mazeData & CMaze::West)
						{
							wall->SetRotation(0, D3DX_PI / 2.0f, 0);
						}
						m_pMazeMeshObjArray.push_back(wall);
					}
					else
					{
						// 曲がり角
						CStaticMeshObject* wall = new CStaticMeshObject();
						wall->AttachMesh(*m_pSewerTurnMesh);
						wall->SetPosition(x, wallHeight, z);
						if (mazeData & CMaze::North && mazeData & CMaze::East)
						{
							wall->SetRotation(0, D3DX_PI, 0);
						}
						else if (mazeData & CMaze::East && mazeData & CMaze::South)
						{
							wall->SetRotation(0, -D3DX_PI / 2.0f, 0);
						}
						else if (mazeData & CMaze::North && mazeData & CMaze::West)
						{
							wall->SetRotation(0, D3DX_PI / 2.0f, 0);
						}
						m_pMazeMeshObjArray.push_back(wall);
					}
					break;
				}

				case 3: // 行き止まり
				{
					CStaticMeshObject* wall = new CStaticMeshObject();
					wall->AttachMesh(*m_pSewerEndMesh);
					wall->SetPosition(x, wallHeight, z);
					if (mazeData & CMaze::North)
					{
						wall->SetRotation(0, D3DX_PI, 0);
					}
					else if (mazeData & CMaze::West)
					{
						wall->SetRotation(0, D3DX_PI / 2.0f, 0);
					}
					else if (mazeData & CMaze::South)
					{
						wall->SetRotation(0, 0, 0);
					}
					else if (mazeData & CMaze::East)
					{
						wall->SetRotation(0, -D3DX_PI / 2.0f, 0);
					}
					m_pMazeMeshObjArray.push_back(wall);
					break;
				}
			}
		}
	}

	for (int i = 0; i < m_pMazeMeshObjArray.size(); ++i)
	{
		m_pMazeMeshObjArray[i]->CreateCollider(CCollider::COLLIDER_SHAPE_BOX);
	}
}


void CTest::ClearMaze()
{
	for (auto& wall : m_pMazeMeshObjArray)
	{
		delete wall;
	}
	m_pMazeMeshObjArray.clear();
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
	// Reverses the calculation in GenerateMazeMeshObj to convert world coordinates back to maze grid coordinates.
	float worldOffsetX = (static_cast<float>(m_MazeCellW)) * (SEWER_MESHWIDTH);

	int mazeX = static_cast<int>((worldPos.x + worldOffsetX) / SEWER_MESHWIDTH);
	int mazeY = static_cast<int>(((-worldPos.z / SEWER_MESHWIDTH) + (static_cast<float>(SEWER_MESHWIDTH))));

	// Clamp values to be within maze bounds
	mazeX = max(0, min(m_MazeCellW - 1, mazeX));
	mazeY = max(0, min(m_MazeCellH - 1, mazeY));

	return { mazeX, mazeY };
}