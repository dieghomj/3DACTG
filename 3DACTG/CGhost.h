#pragma once
#include "CCharacter.h"

class CGhost :
	public CCharacter
{

public:
	CGhost();
	CGhost(int* pMaze, int stride, int regionWidth, int regionHeight);
	~CGhost();

	void Update();
	
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog);

private:

	int* m_pMaze;
	int m_Stride;
	int m_RegionWidth;
	int m_RegionHeight;

};

