/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd ),
	rng(std::random_device()()),
	game_field(10,10,52,rng)
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	dt = ft.Mark();
	UpdateModel();
	if (secondTimer > 0.01666666666666666666666666666667f * 60.f)
		secondTimer = 0;
	secondTimer += dt;
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	if (game_field.gameState != GameField::GameStates::IsOver)
	{
		game_field.Update(wnd.mouse,wnd.kbd,secondTimer);
	}
}

void Game::ComposeFrame()
{
	game_field.Draw(gfx);
}
