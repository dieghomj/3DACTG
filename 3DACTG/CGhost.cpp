#include "CGhost.h"



CGhost::CGhost()
{
}

CGhost::CGhost(int* pMaze, int stride, int regionWidth, int regionHeight)
	: CCharacter()
	, m_pMaze	(pMaze)
	, m_Stride	(stride)
	, m_RegionWidth	(regionWidth)
	, m_RegionHeight(regionHeight)
{
}

CGhost::~CGhost()
{

}

void CGhost::Update()
{
	CCharacter::Update();
}

void CGhost::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog)
{
	CCharacter::Draw(View, Proj, Light, Camera, Fog);
}

