#include "stdafx.h"
#include "ScorpionScene.h"

ScorpionScene::ScorpionScene(CDirectX9& pDx9, CDirectX11& pDx11, HWND hWnd, CTime& pTime, CSceneManager& pManager)
	: CScene(pDx9, pDx11, hWnd, pTime, pManager)
	, m_pScorpionMesh	(nullptr)
	, m_pScorpion		(nullptr)
	, m_pText			(nullptr)
{
}

ScorpionScene::~ScorpionScene()
{
}

void ScorpionScene::Create()
{
	m_pCamera = new CCamera;
	m_pCameraController = new CCameraController(m_pCamera);
	m_pScorpionMesh = new CStaticMesh;
	m_pScorpion = new CStaticMeshObject;
	m_pText = new CFont();
}

HRESULT ScorpionScene::LoadData()
{
	m_pScorpionMesh->Init(*m_pDx9, *m_pDx11, L"Data\\Mesh\\Static\\Bug\\bug.x");
	m_pScorpion->AttachMesh(*m_pScorpionMesh);
	m_pText->Init(*m_pDx11);
	return S_OK;
}

void ScorpionScene::Release()
{
}

void ScorpionScene::Start()
{
	m_pScorpion->SetPosition(0.0f, 5.0f, 5.0f);	
	m_pScorpion->SetScale(1.5f);
}

void ScorpionScene::Update()
{
	m_pCamera->Update();
	m_pCameraController->Update(0);

	m_pScorpion->Update();

	m_pCameraController->ThirdPersonCamera(
		m_pScorpion->GetPosition(),
		15.0f,
		POINT(5.f,0.f),
		m_mouseSense);
}

void ScorpionScene::Draw()
{
	m_pCamera->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	m_pScorpion->Draw(m_mView, m_mProj, m_GlobalLight, m_Camera, m_Fog);
	
	m_pText->Render(
		TEXT("ROLANDO"),
		WND_W/2,
		WND_H/2-50,
		32.0f);
}
