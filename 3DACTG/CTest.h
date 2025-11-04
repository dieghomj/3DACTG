#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CCamera.h"
#include "CStaticMeshObject.h"
#include "CPlayer.h"
#include "CMinimap.h"
#include "CUIObject.h"
#include "CSprite2D.h"
#include "CDebugBSphere.h"
#include "CGhost.h"

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


	D3DXVECTOR3 CellToWorld(int cellIndex, float y = 0.0f, float cellSize = 4.0f) const;
	D3DXVECTOR3 CellToWorldRC(int row, int col, float y = 0.0f, float cellSize = 4.0f) const;


private:
	void GenerateMaze(int regionHeight, int regionWidth, int stride);
	void ClearMaze();
	void DrawTextMinimap(int startX, int startY, int cell, int font, int wallSize);

private:

	CFont* m_SDFText;
	CStaticMesh* m_pGroundStaticMesh;
	CStaticMeshObject* m_pGround;
	

	CStaticMesh* m_pWallStaticMesh;
	std::vector<CStaticMeshObject*> m_pWalls;
	CMiniMapTexture* m_pMiniMap;
	CSprite2D* m_pMiniMapSprite;
	CUIObject* m_pMiniMapUI;


	CPlayer* m_pPlayer;
	
	CStaticMesh* m_pGhostMesh;
	CGhost* m_pGhostList[4];

	CDebugColliderRender* m_pDbgCollider = nullptr;
	bool m_ShowCollider = true;
	bool m_bFog = false;

	//
	int grid[64][64] = { 0 };
	const int m_MazeCellW	= 8;
	const int m_MazeCellH	= 8;
	const int m_MazeStride	= 64;
};