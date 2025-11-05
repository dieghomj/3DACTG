#pragma once
#include "CCharacter.h"
#include <vector>

class CGhost :
	public CCharacter
{

public:
	CGhost();
	CGhost(std::vector<Pair> path);

	~CGhost();

	void Update();
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog);

	int GetCurrentRow() const { return m_CurrentRow; }
	int GetCurrentCol() const { return m_CurrentCol; }

private:


private:

	int m_CurrentRow;
	int m_CurrentCol;
	
	std::vector<Pair> m_pPath;
	int m_pathStep;

	float moveSpeed = 450.0f;
	float moveTime = 0.f;

};

