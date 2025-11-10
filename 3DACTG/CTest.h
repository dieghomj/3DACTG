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
#include "CBaseEnemy.h"
#include "CMaze.h"

#define MAZE_H 64
#define MAZE_W 64

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

	void GenerateMazeMeshObj(int regionHeight, int regionWidth, int stride);
	void ClearMaze();
	Pair NextMazePosition();
	Pair WorldToMazeCoords(const D3DXVECTOR3& worldPos);
	void DrawTextMinimap();

private:

	CFont* m_SDFText;
	CStaticMesh* m_pGroundStaticMesh;
	CStaticMeshObject* m_pGround;
	

	CStaticMesh* m_pWallStaticMesh;

	CStaticMesh* m_pSewerLineMesh;
	CStaticMesh* m_pSewerTurnMesh;
	CStaticMesh* m_pSewerTJunctionMesh;
	CStaticMesh* m_pSewerCrossMesh;
	CStaticMesh* m_pSewerEndMesh;

	std::vector<CStaticMeshObject*> m_pMazeMeshObjArray;

	CPlayer* m_pPlayer;
	CStaticMesh* m_pWomanMesh;
	
	CStaticMesh* m_pGhostMesh;
	CBaseEnemy* m_pGhostList[16];

	CDebugColliderRender* m_pDbgCollider = nullptr;
	bool m_ShowCollider = true;
	bool m_bFog = false;

	//–À˜H
	int					m_pMazeData[MAZE_H][MAZE_W];
	CMaze* m_pMazeGen;
	int					m_MazeCellH;
	int					m_MazeCellW;
	int					m_MazeStride;
	float				m_MazeCellSize;

	CUIObject*			m_pMiniMapUI;
	CSprite2D*			m_pMiniMapSprite;
	CMiniMapTexture*	m_pMiniMap;
	int					m_miniMapStartX;
	int					m_miniMapStartY;
	int					m_miniMapCellSize;
	int					m_miniMapFontSize;
};