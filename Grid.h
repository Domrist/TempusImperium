#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "GameObject.h"

class Road;
class Building;

class Cell : public GameObject
{
	public:
		Building* cellSprite;
		int gridX;
		int gridY;
		int rowIndex;
		int columnIndex;
		std::vector<Cell*> adjacentCells;
		std::vector<Road*> adjacentRoads;
		std::vector<void(Cell::*)()> methodVector;
		Cell(float x,float y);
		Cell();
		~Cell();
		void updateStatus();
		
		void setGridIndex(int xGridIndex,int yGridIndex); //индекс ячейки для поиска
		void setCellObject(Building* sprite);
		void setPos(float x,float y);
		void setAdjacentObject();

		void setDig();
		void setLakeTile();

		void setBuilding();
};

#endif