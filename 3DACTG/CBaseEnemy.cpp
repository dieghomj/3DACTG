#include "CBaseEnemy.h"
#include "CMaze.h"

CBaseEnemy::CBaseEnemy()
	: CAnimCharacter()
	, moveTime		(0.f)
	, moveSpeed		(0.05f)
	, m_pathStep		(0)
	, m_MazeWidth		(0)
	, m_MazeHeight		(0)
	, m_CurrentRow		(0)
	, m_CurrentCol		(0)
	, m_StepList		()
{
}

CBaseEnemy::CBaseEnemy(std::vector<Pair> path, int width, int height)
	: CAnimCharacter()
	, moveTime		(0.f)
	, moveSpeed		(0.05f)
	, m_pathStep	(0)
	, m_MazeWidth	(width)
	, m_MazeHeight	(height)
	, m_StepList	(path)
	, m_CurrentCol	(path[0].x)
	, m_CurrentRow	(path[0].y)
	, m_State		(Move)
{
}


CBaseEnemy::~CBaseEnemy()
{
	m_StepList.clear();
}

void CBaseEnemy::Update()
{
	if(m_StepList.empty())
		return;

	Pair prevCell = { m_CurrentCol, m_CurrentRow};
	
	m_CurrentCol = m_StepList[m_pathStep].x;
	m_CurrentRow = m_StepList[m_pathStep].y;

	D3DXVECTOR3 vNextStep = CMaze::CellToWorldRC(m_CurrentRow, m_CurrentCol, m_MazeHeight, m_MazeWidth, 5.f, 12) ;
	
	if (m_pathStep == 0)
	{
		m_vPosition = vNextStep;
		GetNextStep();
		return;
	}

	D3DXVECTOR3 vDirection = vNextStep - m_vPosition;
	float distance = D3DXVec3Length(&vDirection);
	
	if (distance > 0.5f)
	{
		D3DXVec3Normalize(&vDirection, &vDirection);
		m_vPosition += vDirection * moveSpeed;
	}
	else
	{
		m_vPosition = vNextStep;
		GetNextStep();
	}

	CAnimCharacter::Update();


}

void CBaseEnemy::Draw(SCENE_DATA& sceneData)
{
	CAnimCharacter::Draw(sceneData);
}

void CBaseEnemy::SetPath(std::vector<Pair> path)
{
	m_StepList = path;
	m_pathStep = 0;
}

void CBaseEnemy::GetNextStep()
{
	m_pathStep++;
	if (m_pathStep >= m_StepList.size()) {
		m_pathStep = 0;
	}
}
