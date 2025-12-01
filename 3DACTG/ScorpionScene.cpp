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
	m_pDx11->SetDepth(true);
	m_GlobalLight.fIntensity = 1.0f;
	m_GlobalLight.vDirection = D3DXVECTOR3(0.0f, -1.0f, 1.0f);
	m_GlobalLight.Position = D3DXVECTOR3(0.0f, 10.0f, -5.0f);

	CSpotLight* m_pFlashLight = new CSpotLight;
	m_pFlashLight->SetPosition(D3DXVECTOR3(0.0f, 15.0f, 0.0f));
	m_pFlashLight->SetDirection(D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	m_pFlashLight->SetColor(D3DXCOLOR(1.0f, 1.0f, 0.8f, 1.0f));
	m_pFlashLight->SetRange(100.0f);
	m_pFlashLight->SetInnerAngle(D3DXToRadian(15.0f));
	m_pFlashLight->SetOuterAngle(D3DXToRadian(30.0f));
	m_pFlashLight->SetIntensity(1.0f);
	m_pFlashLight->SetSceneIndex(AddSpotLight(m_pFlashLight));


	m_pScorpion->SetPosition(0.0f, 0.0f, 0.0f);	
	m_pScorpion->SetScale(2.5f);
}

void ScorpionScene::Update()
{
	CScene::Update();
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
	m_pCamera->Draw(m_SceneInfo);
	m_pScorpion->Draw(m_SceneInfo);
	
	m_pText->Render(
		TEXT("ROLANDO"),
		WND_W/2,
		WND_H/2-50,
		32.0f);
}
