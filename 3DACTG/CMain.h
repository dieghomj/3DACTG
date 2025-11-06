#pragma once
#include <Windows.h>
#include "CScene.h"
#include "CSceneManager.h"

//�N���X�̑O���錾.
class CDirectX9;
class CDirectX11;
class CTest;
class CMenu;

/**************************************************
*	���C���N���X.
**/
class CMain
{
public:
	CMain();	//�R���X�g���N�^.
	~CMain();	//�f�X�g���N�^.

	void Update();		//�X�V����.
	HRESULT Create();	//�\�z����.
	HRESULT LoadData();	//�f�[�^���[�h����.
	void Release();		//�������.
	void Loop();		//���C�����[�v.

	//�E�B���h�E�������֐�.
	HRESULT InitWindow(
		HINSTANCE hInstance,
		INT x, INT y,
		INT width, INT height );

private:
	static void LockCursorToWindow(HWND hwnd);
	static void UnlockCursor();
	//�E�B���h�E�֐��i���b�Z�[�W���̏����j.
	static LRESULT CALLBACK MsgProc(
		HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam );

private:
	HWND			m_hWnd;	//�E�B���h�E�n���h��.
	CDirectX9*		m_pDx9;	//DirectX9�Z�b�g�A�b�v�N���X
	CDirectX11*		m_pDx11;//DirectX11�Z�b�g�A�b�v�N���X.

	//�V�[���Ǘ��N���X.
	CSceneManager*	m_pSceneManager;
	
	//�V�[��
	CScene*			m_pGame;
	CScene*			m_pTitle;
	CScene*			m_pTest;
	CScene*			m_pResult;
	CScene*			m_pMenu;
	
	CTime*			m_pTime;
};