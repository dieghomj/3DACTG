#pragma once
class CMaze
{

private:

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

public:

	CMaze();
	~CMaze();

	template <size_t W, size_t H>
	static void GenerateMaze(int(&mOut)[H][W], int width, int height)
	{
		// grid は連続領域なので先頭要素のポインタでフラットに渡す
		GenerateMaze(&mOut[0][0], width, height);
	}

	static void GenerateMaze(int* mOut, int width, int height);

private:

	static void CarvePassages(int cx, int cy, int* maze, int width, int height);
	static Direction GetOppositeDirection(Direction dir);
	static Pair GetMovementFromDirection(Direction dir);
	static bool IsInBounds(int x, int y, int width, int height);
	static void ShuffleDirections(Direction* directions, int size);
};

