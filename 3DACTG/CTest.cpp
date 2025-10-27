#include "CTest.h"
#include "CSoundManager.h"
#include <stdio.h>

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene				(pDx9, pDx11, hWnd, pTime, pManager)
	, m_SDFText				(nullptr)
	, m_pGroundStaticMesh	(nullptr)
	, m_pGround				(nullptr)
{
	m_pDx11->SetDepth(true);
}

CTest::~CTest()
{
}

void CTest::Create()
{
	m_SDFText = new CFont();
	m_pCamera = new CCamera();
	m_pCamera->SetPerspective(D3DX_PI / 4.0f,
		static_cast<float>(WND_W) / static_cast<float>(WND_H),
		0.1f, 1000.0f);
	m_pCamera->SetPosition(0.f, 5.f, -15.f);

	m_pGroundStaticMesh = new CStaticMesh();
	m_pGround = new CStaticMeshObject();

}

HRESULT CTest::LoadData()
{
	if (FAILED(m_SDFText->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	//if (CSoundManager::GetInstance()->Load(m_hWnd) == false) {
	//	_ASSERT_EXPR(false, _T("サウンドマネージャーインスタンス生成失敗"));
	//	return E_FAIL;
	//}

	if (FAILED(m_pGroundStaticMesh->Init(
		*m_pDx9, *m_pDx11,
		_T("Data\\Mesh\\Static\\Ground\\Ground.x"))))
	{
		return E_FAIL;
	}

	m_pGround->AttachMesh(*m_pGroundStaticMesh);
	m_pGround->SetPosition(0.f, 0.f, 0.f);

	return S_OK;

}

void CTest::Release()
{
}

void CTest::Start()
{
}

void CTest::Update()
{
	
	CScene::Update();

	m_pGround->Update();

	m_pCamera->FirstPersonCamera(
		D3DXVECTOR3(0.f,1.f,0.f),
		m_mouseDelta, m_mouseSense);
}

void CTest::Draw()
{
	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera);

	m_pGround->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera);

	m_SDFText->SetColor(1.0f, 1.0f, 1.0f);  
	m_SDFText->SetAlpha(1.0f);               // Fully opaque
	
	TCHAR text[64];
	_stprintf_s(text, _T(" DELTA: (%d,%d)"), m_mouseDelta.x, m_mouseDelta.y);
	m_SDFText->Render(text, 50, 50, 30.f);
}	

