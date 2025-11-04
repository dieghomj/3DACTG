#include "CGhost.h"



CGhost::CGhost()
	: CCharacter()
	, m_CurrentCell		(0)
	, m_CurrentRow		(0)
	, m_CurrentCol		(0)
{
}

CGhost::CGhost(CMaze pMazeGen)
	: CCharacter()
	, m_pMaze			(pMazeGen.GetMazeData())
	, m_Stride			(pMazeGen.GetStride())
	, m_RegionWidth		(pMazeGen.GetRegionWidth())
	, m_RegionHeight	(pMazeGen.GetRegionHeight())
	, m_CurrentCell		(0)
	, m_CurrentRow		(0)
	, m_CurrentCol		(0)
{
}

CGhost::CGhost(int* pMaze, int stride, int regionWidth, int regionHeight)
	: CCharacter()
	, m_pMaze			(pMaze)
	, m_Stride			(stride)
	, m_RegionWidth		(regionWidth)
	, m_RegionHeight	(regionHeight)
{
}

CGhost::~CGhost()
{

}

void CGhost::Update()
{
	CCharacter::Update();

	CMaze::Pair directions[4] = {
		{0, -1},	// North
		{0, 1},		// South
		{1, 0},		// East
		{-1, 0}		// West
	};

	for ( int i = 0; i < 4; ++i )
	{
		const int nextRow = m_CurrentRow + directions[i].y;
		const int nextCol = m_CurrentCol + directions[i].x;
		if ( nextRow < 0 || nextRow >= m_RegionHeight ||
			 nextCol < 0 || nextCol >= m_RegionWidth )
		{
			continue;
		}
		const int curIdx = m_CurrentRow * m_Stride + m_CurrentCol;
		const int nextIdx = nextRow * m_Stride + nextCol;
		if ( directions[i].y == -1 && (m_pMaze[curIdx] & CMaze::North) ) // North
		{
			m_CurrentRow = nextRow;
			m_CurrentCol = nextCol;
			break;
		}
		else if ( directions[i].y == 1 && (m_pMaze[curIdx] & CMaze::South) ) // South
		{
			m_CurrentRow = nextRow;
			m_CurrentCol = nextCol;
			break;
		}
		else if ( directions[i].x == 1 && (m_pMaze[curIdx] & CMaze::East) ) // East
		{
			m_CurrentRow = nextRow;
			m_CurrentCol = nextCol;
			break;
		}
		else if ( directions[i].x == -1 && (m_pMaze[curIdx] & CMaze::West) ) // West
		{
			m_CurrentRow = nextRow;
			m_CurrentCol = nextCol;
			break;
		}
	}

}

void CGhost::Draw(D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera, FOG& Fog)
{
	CCharacter::Draw(View, Proj, Light, Camera, Fog);
}

