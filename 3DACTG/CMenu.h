#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CUIObject.h"
#include "CSprite2D.h"

/********************************************************************************
*	Menu Scene Class.
**/
class CMenu : public CScene
{
public:
	CMenu(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	~CMenu();

	void Create() override;
	void Release() override;
	HRESULT LoadData() override;
	void Start() override;
	void Update() override;
	void Draw() override;

private:
	// Font for menu text
	CFont* m_pMenuFont;
	
	// UI Objects for menu buttons
	CUIObject* m_pTitleUI;
	CUIObject* m_pStartButtonUI;
	CUIObject* m_pExitButtonUI;
	
	// Sprites for UI elements
	CSprite2D* m_pTitleSprite;
	CSprite2D* m_pStartButtonSprite;
	CSprite2D* m_pExitButtonSprite;
	
	// Menu state
	int m_SelectedOption;
	const int MENU_OPTION_START = 0;
	const int MENU_OPTION_EXIT = 1;
};
