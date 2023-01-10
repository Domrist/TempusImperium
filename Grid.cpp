#include "Grid.h"
#include <SFML/Graphics/RectangleShape.hpp>

#include <iostream>
#include "Pool.h"
#include "TempusImperium.h"

#include "Tile.h"

Cell::Cell(float x,float y){
	this->setSize(sf::Vector2f(33,33));
	this->setFillColor(sf::Color::Red); 	
	this->setOrigin(16,16);
	this->setPosition(x,y);
	this->cellSprite = nullptr;
}

void Cell::setCellObject(Building* sprite){
	this->cellSprite = sprite;
	this->cellSprite->parentObject = this; //а может и нахер не надо??
	this->cellSprite->parentCell = this;
	this->cellSprite->setPosition(this->getPosition().x,this->getPosition().y);
	this->setFillColor(sf::Color(255,255,255,0));
}
void Cell::updateStatus(){ ///////!!!! important part
	for(int i = 0;i < this->methodVector.size();i++){
		(this->*methodVector[i])();
	}
}

Cell::Cell() : GameObject(){
	this->setFillColor(sf::Color(255,255,123,0)); // aplha 0 - full transparent. 255 - full color
}

Cell::~Cell(){
	if(this->cellSprite != nullptr){
		delete this->cellSprite;
	}
}
void Cell::setPos(float x,float y){
	this->setPosition(x,y);
	if(this->cellSprite == nullptr){
		return;
	}
	this->cellSprite->setPosition(this->getPosition().x,this->getPosition().y);
}

void Cell::setAdjacentObject(){
	sf::Vector2f mouse = Pool::EventPool::renderWindow->mapPixelToCoords(sf::Mouse::getPosition(*(Pool::EventPool::renderWindow)),*Pool::DrawPool::view);
	if( sf::Mouse::isButtonPressed(sf::Mouse::Left) && this->getGlobalBounds().contains(mouse)){
		
		if(this->cellSprite != nullptr){
			return;
		}
		this->setCellObject(TempusImperium::tmpGameObject);
		TempusImperium::tmpGameObject->parentObject = this;
		TempusImperium::tmpGameObject = nullptr;
		return;
	}
}

void Cell::setLakeTile(){
	if(this->isClicked()){
		Building* tmpLake = new Building(BuildingType::Lake);
		this->setCellObject(tmpLake);
		TempusImperium::lakeVector.push_back(tmpLake);

		for(Building* tmpLake : TempusImperium::lakeVector){
			Cell* pCell = (Cell*)tmpLake->parentObject;
			
			for(int i = 0;i < pCell->adjacentCells.size();i++){
				if(pCell->adjacentCells[i]->cellSprite == nullptr){
					pCell->adjacentCells[i]->setFillColor(sf::Color::Red);
					pCell->adjacentCells[i]->methodVector.clear();
				}
			}
		}
		for(auto tCell : TempusImperium::grid){
			tCell->methodVector.clear();
		}
		TempusImperium::deletePromptTitle();
		TempusImperium::initGameUI();
		TempusImperium::goToNextTurn();
	}
}

void Cell::setDig(){
	this->setFillColor(sf::Color::Green);
	this->methodVector.push_back(&Cell::setLakeTile);
}

void Cell::setBuilding(){
	if(this->isClicked()){
		this->setCellObject(TempusImperium::tmpGameObject);
		for(int i = 0;i < TempusImperium::grid.size();i++){
			TempusImperium::grid[i]->methodVector.clear();
			TempusImperium::grid[i]->setFillColor(sf::Color::Red);
		}
		TempusImperium::deactivateAllCells();
		TempusImperium::deletePromptTitle();
		TempusImperium::initGameUI();
		TempusImperium::goToNextTurn();
	}
}