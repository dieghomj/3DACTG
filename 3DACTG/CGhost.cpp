#include "CGhost.h"

CGhost::CGhost()
	: CCharacter() 
	, m_CurrentRow		(0)
	, m_CurrentCol		(0)
{
}

CGhost::CGhost(std::vector<Pair> path)
	: CCharacter	()
	, m_pathStep	(0)
	, m_StepList	(path)
	, m_CurrentCol	(path[0].x)
	, m_CurrentRow	(path[0].y)
{
}


CGhost::~CGhost()
{
	m_StepList.clear();
}

void CGhost::Update()
{
	CCharacter::Update();

	Pair prevCell = { m_CurrentCol, m_CurrentRow};

 	if (m_StepList.empty())
		return;
	
	if(moveTime >= moveSpeed)
		m_pathStep++;
	
	if (m_pathStep >= m_StepList.size()) {
		m_pathStep = 0;
	}

	m_CurrentCol = m_StepList[m_pathStep].x;
	m_CurrentRow = m_StepList[m_pathStep].y;

	moveTime += 1000.0f / FPS;

	if (prevCell.x != m_CurrentCol || prevCell.y != m_CurrentRow)
		moveTime = 0.f;

}

void CGhost::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog)
{
	CCharacter::Draw(View, Proj, Light, Camera, Fog);
}

void CGhost::SetPath(std::vector<Pair> path)
{
	m_StepList = path;
	m_pathStep = 0;
}

