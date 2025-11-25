#pragma once
#include "CScene.h"
#include "CFont.h"
#include "CStaticMeshObject.h"

class ScorpionScene
	: public CScene
{

	public:
	ScorpionScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager);
	virtual ~ScorpionScene() override;
	virtual void Create() override;
	virtual HRESULT LoadData() override;
	virtual void Release() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Draw() override;

private:
	CStaticMesh* m_pScorpionMesh = new CStaticMesh;
	CStaticMeshObject* m_pScorpion = new CStaticMeshObject;
	CFont* m_pText = nullptr;

};

