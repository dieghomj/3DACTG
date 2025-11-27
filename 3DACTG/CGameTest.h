#pragma once
#include "CScene.h"
#include "CPlayer.h"
#include "CFont.h"
#include "CZako.h"

class CGameTest 
	: public CScene
{

	public:
	CGameTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	~CGameTest();

	void Create();
	void Release();
	HRESULT LoadData();
	void Start();
	void Update();
	void Draw();

private:

	//-----Ground------
	CStaticMesh* m_pGroundMesh;
	CStaticMeshObject* m_pGroundMeshObject;
	CStaticMeshObject* m_pMeshObject;

	//-----Player-------
	CStaticMesh* m_pPlayerMesh;
	CSkinMesh* m_pPlayerSkinMesh;
	CPlayer* m_pPlayer;

	//-----Enemy--------
	std::vector<CZako*> m_pEnemyList;
	CStaticMesh* m_pEnemyMesh;

	//-----------------
	CFont* m_pText = nullptr;

};

