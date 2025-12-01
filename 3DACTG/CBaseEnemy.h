#pragma once
#include "CAnimCharacter.h"
#include <vector>

class CBaseEnemy :
	public CAnimCharacter
{

public:

	enum enState {
		Move,
		Attack,
		Dead
	};

	CBaseEnemy();
	CBaseEnemy(std::vector<Pair> path, int width, int height);

	~CBaseEnemy();

	void Update();
	void Draw(SCENE_DATA& sceneData);

	void SetPath(std::vector<Pair> path);
	int GetCurrentRow() const { return m_CurrentRow; }
	int GetCurrentCol() const { return m_CurrentCol; }

private:
	void GetNextStep();

protected:

	int m_State;

	int m_MazeWidth;
	int m_MazeHeight;
	int m_CurrentRow;
	int m_CurrentCol;
	
	std::vector<Pair> m_StepList;
	int m_pathStep;

	float moveSpeed;
	float moveTime;

};

