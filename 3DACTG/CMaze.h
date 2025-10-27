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
	~CMaze();

	static void GenerateMaze(int* out, int stride, int regionWidth, int regionHeight, int startX = 0, int startY = 0);

private:

	static void CarvePassages(int cx, int cy, int* maze, int stride, int regionWidth, int regionHeight);
	static Direction GetOppositeDirection(Direction dir);
	static Pair GetMovementFromDirection(Direction dir);
	static bool IsInBounds(int x, int y, int width, int height);
	static void ShuffleDirections(Direction* directions, int size);

};

