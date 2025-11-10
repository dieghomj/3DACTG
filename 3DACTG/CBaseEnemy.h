#pragma once
#include "CCharacter.h"
#include <vector>

class CBaseEnemy :
	public CCharacter
{

public:
	CBaseEnemy();
	CBaseEnemy(std::vector<Pair> path, int width, int height);

	~CBaseEnemy();

	void Update();
	void Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog);

	void SetPath(std::vector<Pair> path);
	int GetCurrentRow() const { return m_CurrentRow; }
	int GetCurrentCol() const { return m_CurrentCol; }

private:
	void GetNextStep();

private:

	int m_MazeWidth;
	int m_MazeHeight;
	int m_CurrentRow;
	int m_CurrentCol;
	
	std::vector<Pair> m_StepList;
	int m_pathStep;

	float moveSpeed;
	float moveTime;

};

