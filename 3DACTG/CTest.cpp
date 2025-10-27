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
	//SAFE_DELETE(m_pPlayer);
	//SAFE_DELETE(m_pGround);
	//SAFE_DELETE(m_pGroundStaticMesh);
	//SAFE_DELETE(m_SDFText);

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

	m_pGround->AttachMesh(*m_pGroundStaticMesh);
	m_pGround->SetPosition(0.f, 0.f, 0.f);

	m_pPlayer->SetPosition(0.f, 1.f, 0.f);

	return S_OK;

}

void CTest::Release()
{
	for (auto& wall : m_pWalls)
	{
		delete wall;
	}
	m_pWalls.clear();

	delete m_pWallStaticMesh;
	m_pWallStaticMesh = nullptr;
}

void CTest::Start()
{
	const int regionWidth = 8;
	const int regionHeight = 8;
	const int stride = 64;

	CMaze::GenerateMaze(&grid[0][0], stride, regionWidth, regionHeight);
	const float wallSize = 4.0f;
	const float wallHeight = 1.0f;

	for (int i = 0; i < regionHeight; ++i)
	{
		for (int j = 0; j < regionWidth; ++j)
		{
			float x = (j - regionWidth / 2.0f) * wallSize + (wallSize / 2.0f);
			float z = (i - regionHeight / 2.0f) * wallSize + (wallSize / 2.0f);

			if (grid[i][j] & CMaze::North)
			{
				CStaticMeshObject* wall = new CStaticMeshObject();
				wall->AttachMesh(*m_pWallStaticMesh);
				wall->SetPosition(x, wallHeight, z + wallSize / 2.0f);
				m_pWalls.push_back(wall);
			}
			if (grid[i][j] & CMaze::West)
			{
				CStaticMeshObject* wall = new CStaticMeshObject();
				wall->AttachMesh(*m_pWallStaticMesh);
				wall->SetPosition(x - wallSize / 2.0f, wallHeight, z);
				wall->SetRotation(0, D3DX_PI / 2.0f, 0);
				m_pWalls.push_back(wall);
			}

			/*if (i == regionHeight - 1) {
				if (grid[i][j] & CMaze::South)
				{
					CStaticMeshObject* wall = new CStaticMeshObject();
					wall->AttachMesh(*m_pWallStaticMesh);
					wall->SetPosition(x, wallHeight, z + wallSize / 2.0f);
					m_pWalls.push_back(wall);
				}
			}
			if (j == regionWidth - 1) {
				if (grid[i][j] & CMaze::East)
				{
					CStaticMeshObject* wall = new CStaticMeshObject();
					wall->AttachMesh(*m_pWallStaticMesh);
					wall->SetPosition(x + wallSize / 2.0f, wallHeight, z);
					wall->SetRotation(0, D3DX_PI / 2.0f, 0);
					m_pWalls.push_back(wall);
				}
			}*/
		}
	}
}

void CTest::Update()
{
	
	CScene::Update();

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
	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera);

	m_pGround->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera);

	for (auto& wall : m_pWalls)
	{
		wall->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera);
	}

	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);               // Fully opaque
	
	TCHAR text[64];

	for( int i = 0; i < 8; i++ )
	{
		for( int j = 0; j < 8; j++ )
		{
			_stprintf_s(text, _T("%d"),grid[i][j]);

			m_SDFText->Render( text, 20 + j * (44.f), 100 + i * 30, 24.f);
		}
	}


	_stprintf_s(text, _T(" DELTA: (%d,%d)"), m_mouseDelta.x, m_mouseDelta.y);
	m_SDFText->Render(text, 50, 50, 30.f);
}	
