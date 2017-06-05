#include "GameField.h"
#include "SpritesMinesweeper.h"
#include "Game.h"

GameField::GameField(int nMines_in,int xfield,int yfield,std::mt19937 rng)
	:
	FieldRect(Vec2(xfield, yfield),dimension * width,dimension * height)
{
	nMines = nMines_in;
	x_field = xfield;
	y_field = yfield;
	spawnBombs(rng);
	flagsNumber = nMines;
}

void GameField::spawnBombs(std::mt19937 rng)
{
	for (int i = 0; i < nMines; i++)
	{
		std::uniform_int_distribution<int> xDistr(0, width - 1);
		std::uniform_int_distribution<int> yDistr(0, height - 1);
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
	DrawNumberFlags(flagsNumber, gfx, 1);
	DrawTimer(secundomer, gfx, 1);
	if (gameState != GameStates::Victory)
	{

		if (gameState != GameStates::IsOver)
		{
			
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
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
		}
		else
		{
			
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
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
						Sprites::DrawRevealedCell(gfx, x_field + x * dimension, y_field + y * dimension);
					}
					if (Field[y * width + x].statement == Tile::state::numbered)
					{
						Field[y * width + x].DrawMinesNumber(gfx, x_field + x * dimension, y_field + y * dimension);
					}
				}
		}

	}
	else
	{

	}
}

void GameField::Update(Mouse& mouse,Keyboard& kbd,float second)
{
	if (gameState != GameStates::IsOver)
	{
		if (gameState == GameStates::Started)
		{
			if (second >= 1.f && second < 1.5f)
				secundomer += static_cast<int>(second);
			isTimeAllowedToDraw = true;
			Vec2 mousepos;
			if (mouse.LeftIsPressed() && FieldRect.isInsideRect(mouse))
			{
				mousepos = { mouse.GetPosX() - x_field,mouse.GetPosY() - y_field };

				mousepos.x /= dimension;
				mousepos.y /= dimension;
				if (!Field[mousepos.y * width + mousepos.x].HasBomb() && Field[mousepos.y * width + mousepos.x].statement == Tile::state::locked)
				{
					checkingForMines(mousepos.x, mousepos.y);
				}
				else
				{
					if (Field[mousepos.y * width + mousepos.x].HasBomb())
					{
						Field[mousepos.y * width + mousepos.x].bombedYou = true;
						gameState = { GameStates::IsOver };
					}

				}
			}
			if (counterPerClicks > 0.0166666667 * 4.5f)
			{
				counterPerClicks = 0;
				if (mouse.RightIsPressed() && FieldRect.isInsideRect(mouse))
				{
					mousepos = { mouse.GetPosX() - x_field,mouse.GetPosY() - y_field };

					mousepos.x /= dimension;
					mousepos.y /= dimension;
					if (Field[mousepos.y * width + mousepos.x].statement != Tile::state::flagged && flagsNumber > 0 && Field[mousepos.y * width + mousepos.x].statement == Tile::state::locked)
					{

						flagsNumber--;
						Field[mousepos.y * width + mousepos.x].statement = Tile::state::flagged;
						if (Field[mousepos.y * width + mousepos.x].HasBomb())
						{
							nMines--;
						}
					}
					else
					{
						if (Field[mousepos.y * width + mousepos.x].statement == Tile::state::flagged)
						{
							flagsNumber++;
							Field[mousepos.y * width + mousepos.x].statement = Tile::state::locked;
							if (Field[mousepos.y * width + mousepos.x].HasBomb())
							{
								nMines++;
							}
						}

					}
				}
			}
			bool allOpenedOrFlaggedOrNumbered = true;
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
				{
					allOpenedOrFlaggedOrNumbered = allOpenedOrFlaggedOrNumbered && nMines == 0 && (Field[y * width + x].statement == Tile::state::flagged || Field[y * width + x].statement == Tile::state::numbered || Field[y * width + x].statement == Tile::state::revealed);
				}
			if (allOpenedOrFlaggedOrNumbered)
				gameState = { GameStates::Victory };
		}
		else
		{
			if(gameState == GameStates::NotStarted)
				if (kbd.KeyIsPressed(VK_SPACE))
					gameState = { GameStates::Started };
		}
	
	}
	counterPerClicks += 0.0166666667;
}



void GameField::DrawNumberFlags(int x_in,Graphics& gfx,int num) const
{
	int x = x_in;
	x %= 10;
	int num_off = (num - 1) * 13;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx,42 - num_off,14);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 42 - num_off, 14);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 42 - num_off, 14);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 42 - num_off, 14);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 42 - num_off, 14);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 42 - num_off, 14);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 42 - num_off, 14);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 42 - num_off, 14);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 42 - num_off, 14);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 42 - num_off, 14);
		break;
	}
	num_off += 13;
	x = x_in;
	x /= 10;
	x %= 10;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx, 42 - num_off, 14);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 42 - num_off, 14);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 42 - num_off, 14);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 42 - num_off, 14);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 42 - num_off, 14);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 42 - num_off, 14);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 42 - num_off, 14);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 42 - num_off, 14);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 42 - num_off, 14);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 42 - num_off, 14);
		break;
	}
	num_off += 13;
	x = x_in;
	x /= 100;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx, 42 - num_off, 14);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 42 - num_off, 14);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 42 - num_off, 14);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 42 - num_off, 14);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 42 - num_off, 14);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 42 - num_off, 14);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 42 - num_off, 14);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 42 - num_off, 14);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 42 - num_off, 14);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 42 - num_off, 14);
		break;
	}

}

void GameField::DrawTimer(int x_in, Graphics & gfx, int num) const
{
	int x = x_in;
	x %= 10;
	int num_off = (num - 1) * 13;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx, 247 - num_off, 14);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 247 - num_off, 14);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 247 - num_off, 14);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 247 - num_off, 14);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 247 - num_off, 14);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 247 - num_off, 14);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 247 - num_off, 14);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 247 - num_off, 14);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 247 - num_off, 14);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 247 - num_off, 14);
		break;
	}
	num_off += 13;
	x = x_in;
	x /= 10;
	x %= 10;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx, 247 - num_off, 14);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 247 - num_off, 14);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 247 - num_off, 14);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 247 - num_off, 14);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 247 - num_off, 14);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 247 - num_off, 14);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 247 - num_off, 14);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 247 - num_off, 14);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 247 - num_off, 14);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 247 - num_off, 14);
		break;
	}
	num_off += 13;
	x = x_in;
	x /= 100;
	switch (x)
	{
	case 0: Sprites::DrawNumberZero(gfx, 247 - num_off, 14);
		break;
	case 1: Sprites::DrawNumberOne(gfx, 247 - num_off, 14);
		break;
	case 2: Sprites::DrawNumberTwo(gfx, 247 - num_off, 14);
		break;
	case 3: Sprites::DrawNumberThree(gfx, 247 - num_off, 14);
		break;
	case 4: Sprites::DrawNumberFour(gfx, 247 - num_off, 14);
		break;
	case 5: Sprites::DrawNumberFive(gfx, 247 - num_off, 14);
		break;
	case 6: Sprites::DrawNumberSix(gfx, 247 - num_off, 14);
		break;
	case 7: Sprites::DrawNumberSeven(gfx, 247 - num_off, 14);
		break;
	case 8: Sprites::DrawNumberEight(gfx, 247 - num_off, 14);
		break;
	case 9: Sprites::DrawNumberNine(gfx, 247 - num_off, 14);
		break;
	}
}

void GameField::checkingForMines(int x_in, int y_in)
{
	int mines_quant = 0;
	bool hasAnyBomb = false;
	bool allrevealed = true;
	for (int x = x_in - 1; x <= x_in + 1; x++)
		for (int y = y_in - 1; y <= y_in + 1; y++)
		{
			if (x > width - 1 || y > height - 1 || x < 0 || y < 0)
				continue;
			if (Field[y * width + x].HasBomb())
			{
				mines_quant++;
				hasAnyBomb = hasAnyBomb || Field[y * width + x].HasBomb();
			}
			if (Field[y * width + x].statement == Tile::state::locked)
			{
				allrevealed = false;
			}

		}
			if (hasAnyBomb)
			{
				Field[y_in * width + x_in].statement = { Tile::state::numbered };
				Field[y_in * width + x_in].bombsAround = mines_quant;
				return;
			}
			if (allrevealed)
			{
				return;
			}
			if (!hasAnyBomb)
			{
				for (int x = x_in - 1; x <= x_in + 1; x++)
					for (int y = y_in - 1; y <= y_in + 1; y++)
					{
						if (x > width - 1 || y > height - 1 || x < 0 || y < 0)
							continue;
						if (Field[y * width + x].statement != Tile::state::numbered || Field[y * width + x].statement != Tile::state::flagged)
						{
							Field[y * width + x].statement = { Tile::state::revealed };

						}
					}
				for (int x = x_in - 1; x <= x_in + 1; x++)
					for (int y = y_in - 1; y <= y_in + 1; y++)
					{
						if (x > width - 1 || y > height - 1 || x < 0 || y < 0)
							continue;
						if (Field[y * width + x].statement != Tile::state::numbered || Field[y * width + x].statement != Tile::state::flagged)
						{
							checkingForMines(x, y);
						}
					}
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

