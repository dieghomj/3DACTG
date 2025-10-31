#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CCamera.h"
#include "CStaticMeshObject.h"
#include "CPlayer.h"
#include "CMinimap.h"
#include "CUIObject.h"
#include "CSprite2D.h"

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
	int grid[64][64] = { 0 };

	//
	const int m_MazeCellW	= 32;
	const int m_MazeCellH	= 32;
	const int m_MazeStride	= 64;
};