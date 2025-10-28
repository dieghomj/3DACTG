#include "CScene.h"
#include "CSoundManager.h"

CScene::CScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: m_pDx9			(&pDx9)
	, m_pDx11			(&pDx11)
	, m_hWnd			(hWnd)
	, m_pTime			(&pTime)
	, m_pManager		(&pManager)

	, m_mView			()
	, m_mProj			()

	, m_GlobalLight		()
	, m_Fog				()
	, m_Camera			()
	, m_pCamera			(nullptr)

	, m_mousePos		({ 0, 0 })
	, m_mouseSeudoPos	({ WND_W / 2,WND_H / 2 })
	, m_mouseBeforePos	({ 0, 0 })
	, m_mouseDelta		({ 0, 0 })
	, m_mouseSense		( 0.01f )

{
	//サウンドデータの読み込み
	CSoundManager::GetInstance()->Load(m_hWnd);
	m_GlobalLight.vDirection = D3DXVECTOR3(1.5f, 1.f, -1.f);	//ライト方向

}

CScene::~CScene()
{

	SAFE_DELETE(m_pCamera);

	//外部で作成しているので、ここでは破棄しない
	m_pManager = nullptr;
	m_hWnd = nullptr;
	m_pDx11 = nullptr;
	m_pDx9 = nullptr;
}

void CScene::Update()
{
	UpdateMousePos();

	if (GetAsyncKeyState('P') & 0x0001)
	{
		m_IsPause = !m_IsPause;
	}

}

void CScene::UpdateMousePos()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(m_hWnd, &mousePos);

	// Calculate delta from center
	POINT center = { WND_W / 2, WND_H / 2 };
	m_mouseDelta.x = mousePos.x - center.x;
	m_mouseDelta.y = mousePos.y - center.y;
	m_mouseSeudoPos.x += m_mouseDelta.x;
	m_mouseSeudoPos.y += m_mouseDelta.y;
	//// Reset cursor to center
	ClientToScreen(m_hWnd, &center);
	SetCursorPos(center.x, center.y);

}

POINT CScene::GetMouseSeudoPos()
{
	return m_mouseSeudoPos;
}