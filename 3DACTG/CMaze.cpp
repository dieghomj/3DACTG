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


// 迷路を生成
void CMaze::GenerateMaze(int* out, int stride, int regionWidth, int regionHeight, int startX, int startY)
{

	for (int y = 0; y < regionHeight; ++y)
		for (int x = 0; x < regionWidth; ++x)
			out[y * stride + x] = 0;

	//範囲内に開始位置をクランプ
	ClampStart(startX, startY, regionWidth, regionHeight);
	// 通路を掘る
	CarvePassages(startX, startY, out, stride, regionWidth, regionHeight);

	BuildMazeBorders(out, stride, regionWidth, regionHeight);

	CarveEntrances(out,stride,regionWidth,regionHeight);

}

void CMaze::CarveEntrances(int* maze, int stride, int regionWidth, int regionHeight)
{
		
	int entranceX = 0;
	int entranceY = 0;

	int exitX = regionWidth - 1;
	int exitY = regionHeight - 1;

	maze[entranceY * stride + entranceX] |= West;
	maze[exitY * stride + exitX]		 |= East;

}

void CMaze::BuildMazeBorders(int* maze, int stride, int regionWidth, int regionHeight)
{
	for (int y = 0; y < regionHeight; ++y)
	{
		for (int x = 0; x < regionWidth; ++x)
		{
			const int idx = y * stride + x;
			if (y == 0)
			{
				maze[idx] &= ~North;
			}
			if (y == regionHeight - 1)
			{
				maze[idx] &= ~South;
			}
			if (x == 0)
			{
				maze[idx] &= ~West;
			}
			if (x == regionWidth - 1)
			{
				maze[idx] &= ~East;
			}
		}
	}
}

// 通路を掘る再帰関数
void CMaze::CarvePassages(int cx, int cy, int* maze, int stride, int regionWidth, int regionHeight)
{
	// ランダムな方向の配列を作成
	Direction directions[4] = { North, South, East, West };
	ShuffleDirections(directions, 4);

	// 現在のセルのインデックスを計算
	// 説明: strideは1行あたりのセル数を示す
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

		// 現在のセルと隣接セルの間の壁を取り除く
		maze[curIdx] |= dir;
		maze[nextIdx] |= GetOppositeDirection(dir);

		CarvePassages(nx, ny, maze, stride, regionWidth, regionHeight);
	}

}

// 指定された方向の反対方向を取得
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
// 指定された方向に対応する移動量を取得
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
// 指定された座標が迷路の範囲内にあるか確認
bool CMaze::IsInBounds(int x, int y, int width, int height)
{
	return (x >= 0 && x < (width) && y >= 0 && y < (height));
}
// 配列内の方向をランダムにシャッフル
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
// 開始位置を迷路の範囲内にクランプ
void CMaze::ClampStart(int& startX, int& startY, int regionWidth, int regionHeight)
{
	if (startX < 0) startX = 0;
	if (startY < 0) startY = 0;
	if (startX >= regionWidth)  startX = regionWidth - 1;
	if (startY >= regionHeight) startY = regionHeight - 1;
}

