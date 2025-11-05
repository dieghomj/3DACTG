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

private:
	void GenerateMaze(int regionHeight, int regionWidth, int stride);
	void ClearMaze();
	void DrawTextMinimap(int startX, int startY, int cell, int font, int wallSize);

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


	CMiniMapTexture* m_pMiniMap;
	CSprite2D* m_pMiniMapSprite;
	CUIObject* m_pMiniMapUI;


	CPlayer* m_pPlayer;
	
	CStaticMesh* m_pGhostMesh;
	CGhost* m_pGhostList[4];

	CDebugColliderRender* m_pDbgCollider = nullptr;
	bool m_ShowCollider = true;
	bool m_bFog = false;

	//–À˜H
	CMaze* m_pMazeGen;
	int m_pMazeData[64][64] = { 0 };
	const int m_MazeCellW;
	const int m_MazeCellH;
	const int m_MazeStride;
};