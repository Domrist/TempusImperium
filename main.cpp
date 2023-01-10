#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include <stdio.h>
#include <iostream>
#include "Pool.h"
#include "Tile.h"
#include "TempusImperium.h"
#include "Grid.h"
#include "RoadCluster.h"
#include "UiElements.h"

#include "Building.h"


//все строения ии ресурсы должны иметь ссылку на родительскую клетку
int main()
{

	//init game data
	TempusImperium::currentRound = 1;
	TempusImperium::currentTurn = 1;
	TempusImperium::goldCount = 10;
	TempusImperium::currentQueryActionIndex = 1;

	sf::RenderWindow window(sf::VideoMode(900, 700), "TempusImperiumLinuxApp",sf::Style::Close);
	window.setFramerateLimit(60);

	sf::View view(sf::FloatRect(0.f, 0.f, 800.f, 600.f));
	view.setCenter(0,0);

	Pool::DrawPool::view = &view;

	Pool::EventPool::clicked = false;

	Pool::DrawPool::setRenderWindow(&window);

	sf::Event event;

	Pool::EventPool::renderWindow = &window;
	Pool::EventPool::event = &event;


	Title t("Tempus Imperium by ChrisAnderson",32,sf::Color::White);
	t.txt.setPosition(-400+t.txt.getGlobalBounds().width/2,-268-17);
 	
 	TempusImperium::initUserData();
 	
	while (Pool::DrawPool::renderWindow->isOpen())
	{
		while (Pool::DrawPool::renderWindow->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)Pool::DrawPool::renderWindow->close();
			if (event.type == sf::Event::KeyPressed)
			{
				// Получаем нажатую клавишу - выполняем соответствующее действие
				if (event.key.code == sf::Keyboard::Escape){
					Pool::DrawPool::renderWindow->close();
				}
			}
		}
		/*Pool::Pool::clearUpdatePoolFromNullPtr();
		Pool::Pool::clearDrawPoolFromNullPtr();*/

		TempusImperium::updateUIStatus();
		Pool::StatusPool::updateStatusPool();
		Pool::DrawPool::updateDrawPool();
	}
	TempusImperium::clearData();
	return 0;
}