#include "stdafx.h"
#include "CMaze.h"
#include <random>
#include <vector>

CMaze::CMaze()
{
}

CMaze::~CMaze()
{
}

void CMaze::GenerateMaze(int* mOut, int width, int height)
{

	for (int i = 0; i < width; i++)
	{
		for( int j = 0; j < height; j++)
		{
			mOut[i * width + j] = 0;
		}
	}

	CarvePassages(1, 1, mOut, 32, 32);
}

void CMaze::CarvePassages(int cx, int cy, int* maze, int width, int height)
{

	Direction directions[4] = { North, South, East, West };
	ShuffleDirections(directions, 4);

	for (int i = 0; i < 4; ++i)
	{
		const Direction dir = directions[i];
		const Pair d = GetMovementFromDirection(dir);
		const int nx = cx + d.x;
		const int ny = cy + d.y;

		if (!IsInBounds(nx, ny, width, height))
			continue;

		const int nextIdx = ny * width + nx;
		if (maze[nextIdx] != 0)
			continue;

		const int curIdx = cy * width + cx;

		// Ú‘±ƒtƒ‰ƒO‚ð‘o•ûŒü‚É—§‚Ä‚é
		maze[curIdx] |= dir;
		maze[nextIdx] |= GetOppositeDirection(dir);

		CarvePassages(nx, ny, maze, width, height);
	}

}

CMaze::Direction CMaze::GetOppositeDirection(Direction dir)
{
	switch (dir)
	{
		case North:
			return South;
		case South:
			return North;
		case East:
			return West;
		case West:
			return East;
	}
}

CMaze::Pair CMaze::GetMovementFromDirection(Direction dir)
{
	switch (dir)
	{
	case North:
		return { 0, 1 };
	case South:
		return { 0, -1};
	case East:
		return { 1, 0};
	case West:
		return { -1, 0};
	}
}

bool CMaze::IsInBounds(int x, int y, int width, int height)
{
	return (x > 0 && x < (width) && y > 0 && y < (height));
}

void CMaze::ShuffleDirections(Direction* directions, int size)
{
	if (directions == nullptr || size <= 1)
		return;
	
	static thread_local std::mt19937 rng{ std::random_device{}() };
	for (int i = size - 1; i > 0; --i)
	{
		std::uniform_int_distribution<int> dist(0, i);
		const int j = dist(rng);
		const Direction tmp = directions[i];
		directions[i] = directions[j];
		directions[j] = tmp;
	}
}

