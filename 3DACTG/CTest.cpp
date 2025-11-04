#include "CTest.h"
#include "CSoundManager.h"
#include "CMaze.h"
#include "CDebugColliderRender.h"
#include <stdio.h>

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene				(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pGroundStaticMesh	(nullptr)
	, m_pGround				(nullptr)

	// プレイヤー関連
	, m_pPlayer				(nullptr)

	// ゴースト関連
	, m_pGhostList			()
	, m_pGhostMesh			(nullptr)

	// 迷路関連
	, m_MazeCellH			(32)
	, m_MazeCellW			(32)
	, m_MazeStride			(64)
	// 迷路の壁リスト
	, m_pWalls				()
	, m_pWallStaticMesh		(nullptr)
	, m_pMiniMapUI			(nullptr)
	, m_pMiniMapSprite		(nullptr)
	, m_pMiniMap			(nullptr)

	, m_SDFText				(nullptr)
	, m_pDbgCollider		(nullptr)
	, m_ShowCollider		(false)
{
	m_pDx11->SetDepth(true);
}

CTest::~CTest()
{
	for (auto& wall : m_pWalls)
	{
		delete wall;
	}
	m_pWalls.clear();

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

	// 地面メッシュ作成
	m_pGroundStaticMesh = new CStaticMesh();
	// 地面オブジェクト作成
	m_pGround = new CStaticMeshObject();		
	
	m_pMiniMap = new CMiniMapTexture();
	m_pMiniMapSprite = new CSprite2D();
	m_pMiniMapUI = new CUIObject();

	// プレイヤー作成
	m_pPlayer = new CPlayer();


	m_pGhostMesh = new CStaticMesh();
	// ゴースト作成
	for (int i = 0; i < 4; ++i)
	{
		m_pGhostList[i] = new CGhost;
	}
	
	// 壁メッシュ作成
	m_pWallStaticMesh = new CStaticMesh();

	m_pDbgCollider = new CDebugColliderRender();

}

HRESULT CTest::LoadData()
{
	if (FAILED(m_SDFText->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGroundStaticMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Ground\\Ground.x"))))
	{
		return E_FAIL;
	}

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

	m_pPlayer->SetPosition(0.f, 1.f, 0.f);

	for(int i = 0; i < 4; ++i)
	{
		m_pGhostList[i]->AttachMesh(*m_pGhostMesh);
		m_pGhostList[i]->SetPosition(CellToWorldRC((m_MazeCellH - 1) - i, i, 1.f));
		m_pGhostList[i]->SetScale(0.09f);
		m_pGhostList[i]->CreateCollider(CCollider::COLLIDER_SHAPE_SPHERE);
	}

	return S_OK;

}

void CTest::Release()
{
	
}

void CTest::Start()
{
	// 環境設定
	m_GlobalLight.fIntensity = 0.3f;

	m_Fog.Color = D3DXVECTOR4(0.2f, 0.01f, 0.01f, 1.0f);
	m_Fog.Enable = m_bFog;
	m_Fog.Mode = D3DFOG_EXP;
	m_Fog.Start = 20.0f;
	m_Fog.End = 150.0f;
	m_Fog.Density = 0.08f;

	GenerateMaze(m_MazeCellH, m_MazeCellW, m_MazeStride);

}

void CTest::Update()
{
	
	CScene::Update();

	m_Fog.Enable = m_bFog;

	if (GetAsyncKeyState('R') & 0x0001)
	{
		ClearMaze();
		GenerateMaze(m_MazeCellH, m_MazeCellW, m_MazeStride);
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

	m_pGround->Update();

	m_pPlayer->Update();

	for (int i = 0; i < 4; ++i)
	{
		m_pGhostList[i]->Update();
	}

	for (auto& wall : m_pWalls)
	{
		wall->Update();
	}

	m_pCamera->FirstPersonCamera(
		m_pPlayer->GetPosition(),
		m_mouseDelta, m_mouseSense);
	float rotY = m_pCamera->GetYaw();
	m_pPlayer->SetRotation(0, rotY, 0);

}

void CTest::Draw()
{

	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);

	m_pGround->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);

	for (auto& wall : m_pWalls)
	{
		wall->UpdateCollider();
		wall->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

	for( int i = 0; i < 4; ++i)
	{
		m_pGhostList[i]->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

	if (m_pDbgCollider && m_ShowCollider)
	{
		m_pDx11->SetDepth(false);
		for (auto& wall : m_pWalls)
		{
			if (auto* col = wall->GetCollider())
			{
				m_pDbgCollider->DrawCollider(*m_pDx11, m_mView, m_mProj,
					CCollider::COLLIDER_SHAPE_BOX, *col);
			}
		}

		for( int i = 0; i < 4; ++i)
		{
			if (auto* col = m_pGhostList[i]->GetCollider())
			{
				m_pDbgCollider->DrawCollider(*m_pDx11, m_mView, m_mProj,
					CCollider::COLLIDER_SHAPE_SPHERE, *col);
			}
		}

		m_pDx11->SetDepth(true);
	}

	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);      

	TCHAR text[64];
	
	DrawTextMinimap(20, 20, 9, 11, 4);

	// --- マウス移動量表示 ---
	//_stprintf_s(text, _T(" DELTA: (%d,%d)"), m_mouseDelta.x, m_mouseDelta.y);
	//m_SDFText->Render(text, 50, 50, 30.f);
}	

void CTest::GenerateMaze(int regionHeight, int regionWidth, int stride)
{

	CMaze::GenerateMaze(&grid[0][0], stride, regionWidth, regionHeight); // 迷路生成
	const float wallSize = 4.0f;	// 壁のサイズ
	const float wallHeight = 0.0f;	// 壁の高さ

	// 迷路の壁を配置
	for (int i = 0; i < regionHeight; ++i)
	{
		for (int j = 0; j < regionWidth; ++j)
		{
			// 壁の位置計算
			float x = (j - regionWidth / 2.0f) * wallSize + (wallSize / 2.0f);	// X座標
			float z = (i - regionHeight / 2.0f) * wallSize + (wallSize / 2.0f); // Z座標

			// 壁を追加
			if (!(grid[i][j] & CMaze::North) )
			{
				CStaticMeshObject* wall = new CStaticMeshObject();
				wall->AttachMesh(*m_pWallStaticMesh);
				wall->SetPosition(x, wallHeight, z - wallSize / 2.0f); // 北の壁
				m_pWalls.push_back(wall);
			}
			if (!(grid[i][j] & CMaze::West))
			{
				CStaticMeshObject* wall = new CStaticMeshObject();
				wall->AttachMesh(*m_pWallStaticMesh);
				wall->SetPosition(x - wallSize / 2.0f, wallHeight, z);// 西の壁
				wall->SetRotation(0, D3DX_PI / 2.0f, 0);
				m_pWalls.push_back(wall);
			}

			// 端の壁を追加
			if (i == (regionHeight - 1) && !(grid[i][j] & CMaze::South))
			{
				CStaticMeshObject* wall = new CStaticMeshObject();
				wall->AttachMesh(*m_pWallStaticMesh);
				wall->SetPosition(x, wallHeight, z + wallSize / 2.0f); // 南の壁
				m_pWalls.push_back(wall);
			}
			if (j == (regionWidth - 1) && !(grid[i][j] & CMaze::East))
			{
				CStaticMeshObject* wall = new CStaticMeshObject();
				wall->AttachMesh(*m_pWallStaticMesh);
				wall->SetPosition(x + wallSize / 2.0f, wallHeight, z); // 東の壁
				wall->SetRotation(0, D3DX_PI / 2.0f, 0);
				m_pWalls.push_back(wall);
			}
		}
	}
	
	for(int i = 0; i < m_pWalls.size(); ++i)
	{
		m_pWalls[i]->CreateCollider(CCollider::COLLIDER_SHAPE_BOX);
	}
}

void CTest::ClearMaze()
{
	for (auto& wall : m_pWalls)
	{
		delete wall;
	}
	m_pWalls.clear();
}

void CTest::DrawTextMinimap(int startX, int startY, int cell, int font, int wallSize)
{
	// --- MiniMap (文字) ---
		// レイアウト
	const int regionH = m_MazeCellH;	// 高さ
	const int regionW = m_MazeCellW;	// 幅
	const float mmCell = cell;			// セールサイズ
	const float mmFont = font;			// フォントサイズ
	const float mmStartX = startX;		//上左ｘ座標
	const float mmStartY = startY;		//上左ｙ座標
	
	TCHAR text[64];
	
	// 迷路描画
	for (int i = 0; i < regionH; ++i)
	{
		for (int j = 0; j < regionW; ++j)
		{
			float cx = mmStartX + j * mmCell;
			float cy = mmStartY + i * mmCell;

			// セールの真ん中に空白を描画
			_stprintf_s(text, _T(" "));
			m_SDFText->Render(text, cx, cy, mmFont);

			// 北壁 (セルの上に'_'を描画)
			if (!(grid[i][j] & CMaze::North))
			{
				_stprintf_s(text, _T("_"));
				m_SDFText->Render(text, cx, cy - mmCell * 0.5f, mmFont);
			}
			// 西壁 (セルの左に'|'を描画)
			if (!(grid[i][j] & CMaze::West))
			{
				_stprintf_s(text, _T("|"));
				m_SDFText->Render(text, cx - mmCell * 0.5f, cy, mmFont);
			}

			// 南壁を描画
			if (i == (regionH - 1) && !(grid[i][j] & CMaze::South))
			{
				_stprintf_s(text, _T("_"));
				m_SDFText->Render(text, cx, cy + mmCell * 0.5f, mmFont);
			}
			// 東壁 (セルの右に'|'を描画)
			if (j == (regionW - 1) && !(grid[i][j] & CMaze::East))
			{
				_stprintf_s(text, _T("|"));
				m_SDFText->Render(text, cx + mmCell * 0.5f, cy, mmFont);
			}
		}
	}

	// プレイヤー位置描画
	{
		D3DXVECTOR3 ppos = m_pPlayer->GetPosition();
		// ワールド座標 -> グリッド座標変換
		float worldOffsetX = (regionW * 0.5f) * wallSize;
		float worldOffsetZ = (regionH * 0.5f) * wallSize;
		int px = static_cast<int>((ppos.x + worldOffsetX) / wallSize);
		int pz = static_cast<int>((ppos.z + worldOffsetZ) / wallSize);
		px = max(0, min(regionW - 1, px));
		pz = max(0, min(regionH - 1, pz));

		float pcx = mmStartX + px * mmCell;
		float pcy = mmStartY + pz * mmCell;
		_stprintf_s(text, _T("*"));

		m_SDFText->SetColor(1.0f, 0.0f, 0.0f);
		m_SDFText->Render(text, pcx, pcy, mmFont);

		m_SDFText->SetColor(1.0f, 1.0f, 1.0f);
	}
}

D3DXVECTOR3 CTest::CellToWorld(int cellIndex, float y, float cellSize) const
{
	const int total = m_MazeCellW * m_MazeCellH;
	if (cellIndex < 0) cellIndex = 0;
	if (cellIndex >= total) cellIndex = total - 1;

	const int row = cellIndex / m_MazeCellW;
	const int col = cellIndex % m_MazeCellW;

	return CellToWorldRC(row, col, y, cellSize);
}

D3DXVECTOR3 CTest::CellToWorldRC(int row, int col, float y, float cellSize) const
{
	if (row < 0) row = 0;
	if (row >= m_MazeCellH) row = m_MazeCellH - 1;
	if (col < 0) col = 0;
	if (col >= m_MazeCellW) col = m_MazeCellW - 1;

	const float x = (col - m_MazeCellW / 2.0f) * cellSize + (cellSize * 0.5f);
	const float z = (row - m_MazeCellH / 2.0f) * cellSize + (cellSize * 0.5f);
	return D3DXVECTOR3(x, y, z);
}