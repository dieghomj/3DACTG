#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CStaticMeshObject.h"
#include "CPlayer.h"
#include "CMinimap.h"
#include "CUIObject.h"
#include "CSprite2D.h"
#include "CDebugBSphere.h"
#include "CBaseEnemy.h"
#include "CMaze.h"
#include "CSewerPath.h"
#include "CZako.h"

//-----------------
// 定数定義
//-----------------
const float SEWER_MESHWIDTH = 10.5f;
const int MAZE_H = 64;
const int MAZE_W = 64;
const int CELL_SIZE = 10;
const int MAX_ENEMY_COUNT = 16;

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
	void EnemyCamera();
	void Draw() override;

private:

	void OnEnemyKilled()
	{
		++m_EnemiesKilled;
	}
	void UpdatePlayerCamera();
	void UpdateFPCamera();
	void UpdateStaticCamera();

	void GenerateMazeMeshObj(int regionHeight, int regionWidth, int stride);
	void ClearMaze();
	Pair NextMazePosition();	
	Pair WorldToMazeCoords(const D3DXVECTOR3& worldPos);

private:
	
	// 地面
	CStaticMesh* m_pGroundStaticMesh;
	CStaticMeshObject* m_pGround;
	
	// プレイヤー
	CPlayer* m_pPlayer;
	// 懐中電灯
	CSpotLight* m_pPlayerLight;
	// 懐中電灯UI
	CSprite2D* m_pLightBarSprite;
	CUIObject* m_pLightBar;
	
	CSprite2D* m_pHealthBarSprite;
	CUIObject* m_pHealthBar;

	CSprite2D* m_pTensionSprite;
	CUIObject* m_pTensionUI;

	// プレイヤーレイ
	CRay* m_pPlayerRayY;
	CRay* m_pCrossRay[4];

	// 女性メッシュ
	CSkinMesh* m_pWomanMesh;

	// アイテムメッシュ
	CStaticMesh* m_TMPItemMesh;
	std::vector<CStaticMeshObject*> m_ItemMeshArray;

	// 壁メッシュ
	CStaticMesh* m_pWallStaticMesh;

	// 下水道メッシュ
	CStaticMesh* m_pSewerLineMesh;
	CStaticMesh* m_pSewerTurnMesh;
	CStaticMesh* m_pSewerTJunctionMesh;
	CStaticMesh* m_pSewerCrossMesh;
	CStaticMesh* m_pSewerEndMesh;

	// 下水道パスオブジェクト
	std::vector<CSewerPath*> m_pSewerPathArray;

	// ゴースト/敵
	CStaticMesh* m_pGhostMesh;
	CBaseEnemy* m_pGhostList[128];
	// ザコ
	CSkinMesh* m_pZakoMesh;
	CStaticMesh* m_pZakoStaticMesh;
	CZako* m_pZakoList[128];

	// デバッグ用コライダー描画
	CDebugColliderRender* m_pDbgCollider;
	// デバッグ用テキスト描画
	CFont* m_SDFText;

	// コライダー表示フラグ
	bool m_ShowCollider = true;
	// フォグ有無
	bool m_bFog = true;

	int m_EnemiesKilled = 0;

	//迷路
	int					m_pMazeData[MAZE_H][MAZE_W];
	CMaze* m_pMazeGen;
	int					m_MazeCellH;
	int					m_MazeCellW;
	int					m_MazeStride;
	float				m_MazeCellSize;
	int					m_EnemyCount;

	float				m_GameTimer;

	CSpotLight* m_EndSpotLight;

};