#include "CMenu.h"

CMenu::CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pMenuFont(nullptr)
	, m_SelectedOption(MENU_OPTION_START)
{
	m_pDx11->SetDepth(false); // Disable depth for 2D menu
}

CMenu::~CMenu()
{
	SAFE_DELETE(m_pMenuFont);
}

void CMenu::Create()
{
	// Create font for menu text
	m_pMenuFont = new CFont();
}

HRESULT CMenu::LoadData()
{
	// Initialize font
	if (FAILED(m_pMenuFont->Init(*m_pDx11)))
	{
		return E_FAIL;
	}

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
}

void CMenu::Draw()
{
	// Draw text labels for the menu
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
		_stprintf_s(startText, _T("> START GAME"));
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
		_stprintf_s(exitText, _T("> EXIT"));
		m_pMenuFont->Render(exitText, static_cast<float>(WND_W / 2 - 100), static_cast<float>(WND_H / 2 + 80), 35.0f);
		
		// Draw instructions
		m_pMenuFont->SetColor(0.7f, 0.7f, 0.7f);
		TCHAR instructText[128];
		_stprintf_s(instructText, _T("Use UP/DOWN arrows to navigate, ENTER to select"));
		m_pMenuFont->Render(instructText, static_cast<float>(WND_W / 2 - 300), static_cast<float>(WND_H - 50), 20.0f);
	}
}
