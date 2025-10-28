#include "CTest.h"
#include "CSoundManager.h"
#include "CMaze.h"
#include <stdio.h>

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene				(pDx9, pDx11, hWnd, pTime, pManager)
	, m_SDFText				(nullptr)
	, m_pGroundStaticMesh	(nullptr)
	, m_pGround				(nullptr)
	, m_pPlayer				(nullptr)
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
	
	// 壁メッシュ作成
	m_pWallStaticMesh = new CStaticMesh();

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

	m_pMiniMap->Init(*m_pDx11->GetDevice(), *m_pDx11->GetContext(), m_MazeCellW, m_MazeCellH);

	m_pGround->AttachMesh(*m_pGroundStaticMesh);
	m_pGround->SetPosition(0.f, 0.f, 0.f);

	m_pPlayer->SetPosition(0.f, 1.f, 0.f);

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
	m_Fog.Enable = true;
	m_Fog.Mode = D3DFOG_LINEAR;
	m_Fog.Start = 20.0f;
	m_Fog.End = 150.0f;
	m_Fog.Density = 0.08f;

	GenerateMaze(m_MazeCellH, m_MazeCellW, m_MazeStride);



}

void CTest::Update()
{
	
	CScene::Update();

	if (GetAsyncKeyState('R') & 0x0001)
	{
		ClearMaze();
		GenerateMaze(m_MazeCellH, m_MazeCellW, m_MazeStride);
	}

	m_pGround->Update();

	m_pPlayer->Update();

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
		wall->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	}

	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);               // Fully opaque
	
	TCHAR text[64];
	
	// --- MiniMap (character based) ---
		// layout
	const int regionH = m_MazeCellH;
	const int regionW = m_MazeCellW;
	const float mmCell = 9.f;    // pixels per cell
	const float mmFont = 11.f;    // font size for minimap chars
	const float mmStartX = 20.0f; // minimap top-left
	const float mmStartY = 20.0f;

	// draw cells and walls
	for (int i = 0; i < regionH; ++i)
	{
		for (int j = 0; j < regionW; ++j)
		{
			float cx = mmStartX + j * mmCell;
			float cy = mmStartY + i * mmCell;

			// cell center marker
			_stprintf_s(text, _T(" "));
			m_SDFText->Render(text, cx, cy, mmFont);

			// North wall (draw a horizontal '-' above cell)
			if (!(grid[i][j] & CMaze::North))
			{
				_stprintf_s(text, _T("_"));
				m_SDFText->Render(text, cx, cy - mmCell * 0.5f, mmFont);
			}
			// West wall (draw a vertical '|' left of cell)
			if (!(grid[i][j] & CMaze::West))
			{
				_stprintf_s(text, _T("|"));
				m_SDFText->Render(text, cx - mmCell * 0.5f, cy, mmFont);
			}

			// Border walls for southern and eastern edges
			if (i == (regionH - 1) && !(grid[i][j] & CMaze::South))
			{
				_stprintf_s(text, _T("_"));
				m_SDFText->Render(text, cx, cy + mmCell * 0.5f, mmFont);
			}
			if (j == (regionW - 1) && !(grid[i][j] & CMaze::East))
			{
				_stprintf_s(text, _T("|"));
				m_SDFText->Render(text, cx + mmCell * 0.5f, cy, mmFont);
			}
		}
	}

	// draw player on minimap
	{
		D3DXVECTOR3 ppos = m_pPlayer->GetPosition();
		const float wallSize = 4.0f; // must match GenerateMaze usage
		// convert world position to grid indices (same origin used for walls in GenerateMaze)
		float worldOffsetX = (regionW / 2.0f) * wallSize;
		float worldOffsetZ = (regionH / 2.0f) * wallSize;
		int px = static_cast<int>((ppos.x + worldOffsetX) / wallSize);
		int pz = static_cast<int>((ppos.z + worldOffsetZ) / wallSize);
		px = max(0, min(regionW - 1, px));
		pz = max(0, min(regionH - 1, pz));

		float pcx = mmStartX + px * mmCell;
		float pcy = mmStartY + pz * mmCell;
		_stprintf_s(text, _T("*"));
		m_SDFText->SetColor(1.0f, 0.0f, 0.0f); // player red
		m_SDFText->Render(text, pcx, pcy, mmFont);
		m_SDFText->SetColor(1.0f, 1.0f, 1.0f);
	}

	// existing mouse delta debug
	_stprintf_s(text, _T(" DELTA: (%d,%d)"), m_mouseDelta.x, m_mouseDelta.y);
	m_SDFText->Render(text, 50, 50, 30.f);
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
}

void CTest::ClearMaze()
{
	for (auto& wall : m_pWalls)
	{
		delete wall;
	}
	m_pWalls.clear();
}
