#include "CMenu.h"
#include "CSoundManager.h"

CMenu::CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pMenuFont(nullptr)
	, m_pTitleUI(nullptr)
	, m_pStartButtonUI(nullptr)
	, m_pExitButtonUI(nullptr)
	, m_pTitleSprite(nullptr)
	, m_pStartButtonSprite(nullptr)
	, m_pExitButtonSprite(nullptr)
	, m_SelectedOption(MENU_OPTION_START)
{
	m_pDx11->SetDepth(false); // Disable depth for 2D menu
}

CMenu::~CMenu()
{
	SAFE_DELETE(m_pTitleUI);
	SAFE_DELETE(m_pStartButtonUI);
	SAFE_DELETE(m_pExitButtonUI);
	SAFE_DELETE(m_pTitleSprite);
	SAFE_DELETE(m_pStartButtonSprite);
	SAFE_DELETE(m_pExitButtonSprite);
	SAFE_DELETE(m_pMenuFont);
}

void CMenu::Create()
{
	// Create font for menu text
	m_pMenuFont = new CFont();

	// Create UI objects
	m_pTitleUI = new CUIObject();
	m_pStartButtonUI = new CUIObject();
	m_pExitButtonUI = new CUIObject();

	// Create sprites (will be simple colored rectangles)
	m_pTitleSprite = new CSprite2D();
	m_pStartButtonSprite = new CSprite2D();
	m_pExitButtonSprite = new CSprite2D();
}

HRESULT CMenu::LoadData()
{
	// Initialize font
	if (FAILED(m_pMenuFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

	// Initialize sprites for UI elements
	if (FAILED(m_pTitleSprite->Init(*m_pDx11, nullptr, 1.0f, 1.0f)))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pStartButtonSprite->Init(*m_pDx11, nullptr, 1.0f, 1.0f)))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pExitButtonSprite->Init(*m_pDx11, nullptr, 1.0f, 1.0f)))
	{
		return E_FAIL;
	}

	// Attach sprites to UI objects
	m_pTitleUI->AttachSprite(*m_pTitleSprite);
	m_pStartButtonUI->AttachSprite(*m_pStartButtonSprite);
	m_pExitButtonUI->AttachSprite(*m_pExitButtonSprite);

	// Set positions for UI elements
	// Title at top center
	m_pTitleUI->SetPosition(static_cast<float>(WND_W / 2), 100.0f, 0.0f);
	m_pTitleUI->SetScale(400.0f, 80.0f, 1.0f);

	// Start button in middle
	m_pStartButtonUI->SetPosition(static_cast<float>(WND_W / 2), static_cast<float>(WND_H / 2), 0.0f);
	m_pStartButtonUI->SetScale(300.0f, 60.0f, 1.0f);

	// Exit button below start
	m_pExitButtonUI->SetPosition(static_cast<float>(WND_W / 2), static_cast<float>(WND_H / 2 + 100), 0.0f);
	m_pExitButtonUI->SetScale(300.0f, 60.0f, 1.0f);

	return S_OK;
}

void CMenu::Release()
{
	// Clean up resources
}

void CMenu::Start()
{
	// Initialize menu state
	m_SelectedOption = MENU_OPTION_START;
}

void CMenu::Update()
{
	CScene::Update();

	// Handle keyboard input for menu navigation
	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		m_SelectedOption = MENU_OPTION_START;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		m_SelectedOption = MENU_OPTION_EXIT;
	}
	
	// Handle selection
	if (GetAsyncKeyState(VK_RETURN) & 0x0001)
	{
		if (m_SelectedOption == MENU_OPTION_START)
		{
			// Change to game scene
			m_pManager->ChangeScene("TEST");
		}
		else if (m_SelectedOption == MENU_OPTION_EXIT)
		{
			// Exit the game
			PostQuitMessage(0);
		}
	}

	// Update UI objects
	if (m_pTitleUI) m_pTitleUI->Update();
	if (m_pStartButtonUI) m_pStartButtonUI->Update();
	if (m_pExitButtonUI) m_pExitButtonUI->Update();
}

void CMenu::Draw()
{
	// Set clear color for menu background
	m_pDx11->GetContext()->ClearRenderTargetView(
		m_pDx11->GetRenderTargetView(),
		D3DXVECTOR4(0.1f, 0.1f, 0.2f, 1.0f)
	);

	// Draw UI elements
	if (m_pTitleUI) m_pTitleUI->Draw();
	if (m_pStartButtonUI) m_pStartButtonUI->Draw();
	if (m_pExitButtonUI) m_pExitButtonUI->Draw();

	// Draw text labels
	if (m_pMenuFont)
	{
		m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f);
		m_pMenuFont->SetAlpha(1.0f);

		// Draw title
		TCHAR titleText[64];
		_stprintf_s(titleText, _T("3D ACTION SHOOTING GAME"));
		m_pMenuFont->Render(titleText, static_cast<float>(WND_W / 2 - 200), 90.0f, 40.0f);

		// Draw start button text
		if (m_SelectedOption == MENU_OPTION_START)
		{
			m_pMenuFont->SetColor(1.0f, 1.0f, 0.0f); // Yellow for selected
		}
		else
		{
			m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f); // White
		}
		TCHAR startText[64];
		_stprintf_s(startText, _T("START GAME"));
		m_pMenuFont->Render(startText, static_cast<float>(WND_W / 2 - 100), static_cast<float>(WND_H / 2 - 20), 35.0f);

		// Draw exit button text
		if (m_SelectedOption == MENU_OPTION_EXIT)
		{
			m_pMenuFont->SetColor(1.0f, 1.0f, 0.0f); // Yellow for selected
		}
		else
		{
			m_pMenuFont->SetColor(1.0f, 1.0f, 1.0f); // White
		}
		TCHAR exitText[64];
		_stprintf_s(exitText, _T("EXIT"));
		m_pMenuFont->Render(exitText, static_cast<float>(WND_W / 2 - 50), static_cast<float>(WND_H / 2 + 80), 35.0f);
		
		// Draw instructions
		m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
		TCHAR instructText[128];
		_stprintf_s(instructText, _T("Use UP/DOWN arrows to navigate, ENTER to select"));
		m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 300), static_cast<float>(WND_H - 50), 20.0f);
	}
}
