#ifndef POOL_H
#define POOL_H

#include <stdio.h>
#include <iostream>
#include "GameObject.h"
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "Grid.h"

namespace Pool
{
	class Pool
	{
		public:
			static std::vector<sf::Drawable*> drawPool;
			static std::vector<GameObject*> statusPool;
			static sf::RenderWindow* renderWindow;
			static sf::Event* event;
			static sf::View* view;
			
			virtual void updatePool();
			
			static void addToPool(GameObject *obj);
			static void addToDrawPool(sf::Drawable* drawObj);
	};

	class DrawPool : public Pool
	{
		public:
			static void setRenderWindow(sf::RenderWindow *win);
			static void updateDrawPool();
	};

	class StatusPool : public Pool
	{
		public:
			static void updateStatusPool();
	};

	class EventPool : public Pool
	{
		public:
			static bool clicked;
			static void updateEventPool();
			
	};
}

#endif