#pragma once
#include "Vec2.h"
#include "RectF.h"
#include "Mouse.h"
#include <random>
#include "Graphics.h"


class GameField
{
private:
	class Tile
	{
	public:
		enum class state
		{
			locked,
			revealed,
			flagged
		};
	public:
		void SpawnBomb();
		bool HasBomb() const;
		bool bombedYou = false;
		state statement = { state::locked };
	private:
		bool isHasBomb = false;
	};
public:
	GameField(int nMines_in, int xfield, int yfield, std::mt19937 rng);
	void spawnBombs(std::mt19937 rng);
	void Draw(Graphics& gfx);
	void Update(Mouse& mouse);
	bool isOver() const;
private:
	bool isGameOver = false;
	RectF FieldRect;
	static constexpr int dimension = 16;
	static constexpr int width = 16;
	static constexpr int height = 16;
	int x_field;
	int y_field;
	int nMines;
	Tile Field[width * height];
};