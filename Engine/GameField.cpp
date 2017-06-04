#include "GameField.h"
#include "SpritesMinesweeper.h"

GameField::GameField(int nMines_in,int xfield,int yfield,std::mt19937 rng)
	:
	FieldRect(Vec2(xfield, yfield),dimension * width,dimension * height)
{
	nMines = nMines_in;
	x_field = xfield;
	y_field = yfield;
	spawnBombs(rng);
}

void GameField::spawnBombs(std::mt19937 rng)
{
	for (int i = 0; i < nMines; i++)
	{
		std::uniform_int_distribution<int> xDistr(0, width);
		std::uniform_int_distribution<int> yDistr(0, height);
		if (Field[yDistr(rng) * width + xDistr(rng)].HasBomb())
		{
			i--;
		}
		else
		{
			Field[yDistr(rng) * width + xDistr(rng)].SpawnBomb();
		}
	}
}

void GameField::Draw(Graphics& gfx)
{
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++)
		{
			if (!isGameOver)
			{
				switch (Field[y * width + x].statement)
				{
				case Tile::state::locked:
					Sprites::DrawLockedCell(gfx, x_field + x * dimension, y_field + y * dimension);
					break;
				case Tile::state::revealed:
					Sprites::DrawRevealedCell(gfx, x_field + x * dimension, y_field + y * dimension);
					break;
				case Tile::state::flagged:
					Sprites::DrawFlag(gfx, x_field + x * dimension, y_field + y * dimension);
					break;
				}
			}
			else
			{
				if (Field[y * width + x].HasBomb() && Field[y * width + x].statement == Tile::state::flagged)
				{
					Sprites::DrawBombDestructed(gfx, x_field + x * dimension, y_field + y * dimension);
				}
				else
				{
					if (Field[y * width + x].bombedYou)
					{
						Sprites::DrawBombBombedYou(gfx, x_field + x * dimension, y_field + y * dimension);
					}
					else
					{
						Sprites::DrawBombLive(gfx, x_field + x * dimension, y_field + y * dimension);
					}
				}
				if (Field[y * width + x].statement == Tile::state::locked && !Field[y * width + x].HasBomb())
				{
					Sprites::DrawLockedCell(gfx, x_field + x * dimension, y_field + y * dimension);
				}
				if (Field[y * width + x].statement == Tile::state::revealed && !Field[y * width + x].HasBomb())
				{
					//here must be a add condition for numbers
					Sprites::DrawRevealedCell(gfx, x_field + x * dimension, y_field + y * dimension);
				}
			}
		}
}

void GameField::Update(Mouse& mouse)
{
	if (!isGameOver)
	{
		Vec2 mousepos;
		if (mouse.LeftIsPressed() && FieldRect.isInsideRect(mouse))
		{
			mousepos = { mouse.GetPosX() - x_field,mouse.GetPosY() - y_field };

			mousepos.x /= dimension;
			mousepos.y /= dimension;
			if (!Field[mousepos.y * width + mousepos.x].HasBomb())
			{
				Field[mousepos.y * width + mousepos.x].statement = Tile::state::revealed;
			}
			else
			{
				Field[mousepos.y * width + mousepos.x].bombedYou = true;
				isGameOver = true;
			}
		}
	}
}

bool GameField::isOver() const
{
	return isGameOver;
}

void GameField::Tile::SpawnBomb()
{
	isHasBomb = true;
}

bool GameField::Tile::HasBomb() const
{
	return isHasBomb;
}

