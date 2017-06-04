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
			flagged,
			numbered
		};
		enum class MinesNumber
		{
			zero,
			one,
			two,
			three,
			four,
			five,
			six,
			seven,
			eight
		};
	public:
		int bombsAround;
		void SpawnBomb();
		bool HasBomb() const;
		bool bombedYou = false;
		state statement = { state::locked };
		MinesNumber mines_number = { MinesNumber::zero };
		void DrawMinesNumber(Graphics& gfx,int x_in,int y_in) const;
	private:
		bool isHasBomb = false;
	};
public:
	enum class Number
	{
		zero,
		one,
		two,
		three,
		four,
		five,
		six,
		seven,
		eight,
		nine
	};
	GameField(int nMines_in, int xfield, int yfield, std::mt19937 rng);
	void spawnBombs(std::mt19937 rng);
	void Draw(Graphics& gfx);
	void Update(Mouse& mouse);
	bool isOver() const;
	void DrawNumberTime(int x_in,Graphics& gfx, int num) const;
	Number number = {Number::zero};
private:
	int flagsNumber = 10;
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