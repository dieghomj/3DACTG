#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CCamera.h"
#include "CStaticMeshObject.h"
#include "CPlayer.h"

class CTest : public CScene
{
public:
	CTest(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	~CTest();

	void Create() override;	
	void Release() override;
	HRESULT LoadData() override;
	void Start() override;
	void Update() override;
	void Draw() override;

private:

	CFont* m_SDFText;
	CStaticMesh* m_pGroundStaticMesh;
	CStaticMeshObject* m_pGround;
	
	CStaticMesh* m_pWallStaticMesh;
	std::vector<CStaticMeshObject*> m_pWalls;

	CPlayer* m_pPlayer;
	int grid[64][64] = { 0 };
};