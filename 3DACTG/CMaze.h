#pragma once
class CMaze
{

public:

	enum Direction
	{
		North = 1,
		South = 2,
		East = 4,
		West = 8
	};

	struct Pair
	{
		int x;
		int y;
	};


	static void ClampStart(int& startX, int& startY, int regionWidth, int regionHeight);

public:

	CMaze();
	CMaze(int* pMaze, int stride, int regionWidth, int regionHeight, int startX = 0, int startY = 0);
	~CMaze();

	int* GetMazeData() const { return m_pMazeData; }
	int GetRegionWidth() const { return m_RegionWidth; }
	int GetRegionHeight() const { return m_RegionHeight; }
	int GetStride() const { return m_Stride; }

	D3DXVECTOR3 CellToWorld(int cellIndex, float y = 0.f, float cellSize = 4.0f) const;
	D3DXVECTOR3 CellToWorldRC(int row, int col, float y = 0.f, float cellSize = 4.0f) const;

	static void GenerateMaze(int* out, int stride, int regionWidth, int regionHeight, int startX = 0, int startY = 0);

private:

	static void CarvePassages(int cx, int cy, int* maze, int stride, int regionWidth, int regionHeight);
	static void CarveEntrances(int* maze, int stride, int regionWidth, int regionHeight);
	//static void FindTwoFurthestPoints(int* maze, int stride, int regionWidth, int regionHeight, Pair& outPointA, Pair& outPointB);
	static void BuildMazeBorders(int* maze, int stride, int regionWidth, int regionHeight);
	static Direction GetOppositeDirection(Direction dir);
	static Pair GetMovementFromDirection(Direction dir);
	static bool IsInBounds(int x, int y, int width, int height);
	static void ShuffleDirections(Direction* directions, int size);

private: 

	int*	m_pMazeData;
	int		m_RegionWidth;
	int		m_RegionHeight;
	int		m_Stride;

};

