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
	{
		Sprites::DrawBottomBorder(gfx, 0, 308);
		Sprites::DrawLeftBorder(gfx, 0, 52);
		Sprites::DrawLeftBorder(gfx, 0, 180);
		Sprites::DrawRightBorder(gfx, 266, 52);
		Sprites::DrawRightBorder(gfx, 266, 180);
		Sprites::DrawTopBoard(gfx, 0, 0);
	}
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
				case Tile::state::numbered:
					Field[y * width + x].DrawMinesNumber(gfx, x_field + x * dimension, y_field + y * dimension);
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
						if (Field[y * width + x].HasBomb())
						{
							Sprites::DrawBombLive(gfx, x_field + x * dimension, y_field + y * dimension);
						}
					}
				}
				if (Field[y * width + x].statement == Tile::state::locked && !Field[y * width + x].HasBomb())
				{
					Sprites::DrawLockedCell(gfx, x_field + x * dimension, y_field + y * dimension);
				}
				if (Field[y * width + x].statement == Tile::state::flagged && !Field[y * width + x].HasBomb())
				{
					Sprites::DrawFlag(gfx, x_field + x * dimension, y_field + y * dimension);
				}
				if (Field[y * width + x].statement == Tile::state::revealed && !Field[y * width + x].HasBomb())
				{
					//here must be a add condition for numbers
					Sprites::DrawRevealedCell(gfx, x_field + x * dimension, y_field + y * dimension);
				}
				if (Field[y * width + x].statement == Tile::state::numbered)
				{
					Field[y * width + x].DrawMinesNumber(gfx, x_field + x * dimension, y_field + y * dimension);
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
			if (!Field[mousepos.y * width + mousepos.x].HasBomb() && Field[mousepos.y * width + mousepos.x].statement == Tile::state::locked)
			{
				int bombsNumber = 0;
				bool hasAnyBomb = false;
				for (int x = mousepos.x - 1; x <= mousepos.x + 1; x++)
					for (int y = mousepos.y - 1; y <= mousepos.y + 1; y++)
					{
						if (x > width - 1 || y > height - 1 || x < 0 || y < 0)
							continue;
						if (Field[y * width + x].HasBomb())
						{
							bombsNumber++;
							hasAnyBomb = hasAnyBomb || Field[y * width + x].HasBomb();
						}
					}
				if (hasAnyBomb)
				{
					Field[mousepos.y * width + mousepos.x].statement = { Tile::state::numbered };
					Field[mousepos.y * width + mousepos.x].bombsAround = bombsNumber;
				}
				if (!hasAnyBomb)
				{
					for (int x = mousepos.x - 1; x <= mousepos.x + 1; x++)
						for (int y = mousepos.y - 1; y <= mousepos.y + 1; y++)
						{
							if (x > width - 1 || y > height - 1 || x < 0 || y < 0)
								continue;
							if (Field[y * width + x].statement != Tile::state::numbered)
							{
								Field[y * width + x].statement = { Tile::state::revealed };
							}
							
						}
				}
			}
			else
			{
				if (Field[mousepos.y * width + mousepos.x].HasBomb())
				{
					Field[mousepos.y * width + mousepos.x].bombedYou = true;
					isGameOver = true;
				}
				
			}
		}
		if (mouse.RightIsPressed() && FieldRect.isInsideRect(mouse))
		{
			mousepos = { mouse.GetPosX() - x_field,mouse.GetPosY() - y_field };

			mousepos.x /= dimension;
			mousepos.y /= dimension;
			if (Field[mousepos.y * width + mousepos.x].statement != Tile::state::flagged && flagsNumber > 0 )
			{
				
				flagsNumber--;
				Field[mousepos.y * width + mousepos.x].statement = Tile::state::flagged;
			}
			else
			{
				if (Field[mousepos.y * width + mousepos.x].statement == Tile::state::flagged)
				{
					flagsNumber++;
					Field[mousepos.y * width + mousepos.x].statement = Tile::state::locked;
				}
			}
		}
	}
}

bool GameField::isOver() const
{
	return isGameOver;
}

void GameField::DrawNumberTime(int x_in,Graphics& gfx,int num) const
{
	int x = x_in;
	int num_off = (num - 1) * 13;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx,259 - num_off,8);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 259 - num_off, 8);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 259 - num_off, 8);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 259 - num_off, 8);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 259 - num_off, 8);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 259 - num_off, 8);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 259 - num_off, 8);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 259 - num_off, 8);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 259 - num_off, 8);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 259 - num_off, 8);
		break;
	}
}

void GameField::Tile::SpawnBomb()
{
	isHasBomb = true;
}

bool GameField::Tile::HasBomb() const
{
	return isHasBomb;
}

void GameField::Tile::DrawMinesNumber(Graphics & gfx, int x_in, int y_in) const
{
	switch (bombsAround)
	{
	case 1: Sprites::DrawQuantOne(gfx, x_in , y_in);
		break;
	case 2: Sprites::DrawQuantTwo(gfx, x_in, y_in);
		break;
	case 3: Sprites::DrawQuantThree(gfx, x_in, y_in);
		break;
	case 4: Sprites::DrawQuantFour(gfx, x_in, y_in);
		break;
	case 5: Sprites::DrawQuantFive(gfx, x_in, y_in);
		break;
	case 6: Sprites::DrawQuantSix(gfx, x_in, y_in);
		break;
	case 7: Sprites::DrawQuantSeven(gfx, x_in, y_in);
		break;
	case 8: Sprites::DrawQuantEight(gfx, x_in, y_in);
		break;
	}
}

