#include "CTest.h"

CTest::CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_SDFText (nullptr)
{
}

CTest::~CTest()
{
}

void CTest::Create()
{
	m_SDFText = new CFont();
}

HRESULT CTest::LoadData()
{
	if (FAILED(m_SDFText->Init(*m_pDx11)))
	{
		return E_FAIL;
	}
}

void CTest::Release()
{
}

void CTest::Update()
{
	CScene::Update();
}

void CTest::Draw()
{
	m_SDFText->SetColor(1.0f, 0.0f, 0.0f);  // White
	m_SDFText->SetAlpha(1.0f);               // Fully opaque
	m_SDFText->Render(_T(" SCORE: 1234567890"), 50, 50, 30.f);
}	

void CTest::Start()
{
}