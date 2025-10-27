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



void CMaze::GenerateMaze(int* out, int stride, int regionWidth, int regionHeight, int startX, int startY)
{

	for (int y = 0; y < regionHeight; ++y)
		for (int x = 0; x < regionWidth; ++x)
			out[y * stride + x] = 0;

	// Clamp start inside region
	ClampStart(startX, startY, regionWidth, regionHeight);


	CarvePassages(startX, startY, out, stride, regionWidth, regionHeight);
}

void CMaze::CarvePassages(int cx, int cy, int* maze, int stride, int regionWidth, int regionHeight)
{

	Direction directions[4] = { North, South, East, West };
	ShuffleDirections(directions, 4);

	const int curIdx = cy * stride + cx;

	for (int i = 0; i < 4; ++i)
	{
		const Direction dir = directions[i];
		const Pair d = GetMovementFromDirection(dir);
		const int nx = cx + d.x;
		const int ny = cy + d.y;

		if (!IsInBounds(nx, ny, regionWidth, regionHeight))
			continue;

		const int nextIdx = ny * stride + nx;
		if (maze[nextIdx] != 0)
			continue;

		// Carve both ways
		maze[curIdx] |= dir;
		maze[nextIdx] |= GetOppositeDirection(dir);

		CarvePassages(nx, ny, maze, stride, regionWidth, regionHeight);
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
		return { 0, -1 };
	case South:
		return { 0,  1 };
	case East:
		return { 1, 0};
	case West:
		return { -1, 0};
	}
}

bool CMaze::IsInBounds(int x, int y, int width, int height)
{
	return (x >= 0 && x < (width) && y >= 0 && y < (height));
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

void CMaze::ClampStart(int& startX, int& startY, int regionWidth, int regionHeight)
{
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (startX >= regionWidth)  startX = regionWidth - 1;
	if (startY >= regionHeight) startY = regionHeight - 1;
}

