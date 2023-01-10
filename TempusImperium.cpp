#include "TempusImperium.h"
#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include <chrono>
#include <ctime>
#include <math.h>
#include "Pool.h"
#include <thread>
#include "Grid.h"

#include "Tile.h"
#include "Building.h"
#include "RoadCluster.h"

#include "UiElements.h"

#include <random>

int TempusImperium::goldCount;
int TempusImperium::currentRound;
int TempusImperium::dateMas[10];
int TempusImperium::currentTurn;

RoadCluster* TempusImperium::currentRoadCluster;

std::thread* TempusImperium::myThread; //рудиментарные две строчки которые стоит удалить
std::vector<Cell*> TempusImperium::grid;

std::vector<Road*> TempusImperium::roadVector;
std::vector<Building*> TempusImperium::farmVector;
std::vector<Building*> TempusImperium::marketVector;
std::vector<Building*> TempusImperium::fortVector;
std::vector<Building*> TempusImperium::castleVector;
std::vector<Building*> TempusImperium::enemyVector;
std::vector<Building*> TempusImperium::lakeVector;
std::vector<Building*> TempusImperium::treeVector;
std::vector<Building*> TempusImperium::rockVector;

sf::Vector2f TempusImperium::mousePos;

std::vector<ActionType> TempusImperium::queryAction;
int TempusImperium::currentQueryActionIndex;

Background* TempusImperium::backgroundGrid;
Building* TempusImperium::tmpGameObject;

//UI Block
Title* TempusImperium::promptTitle;

Title* TempusImperium::currentRoundTitle;
Title* TempusImperium::currentTurnTitle;
Title* TempusImperium::currentMainActionTitle;
Title* TempusImperium::currentGoldCountTitle;
Title* TempusImperium::additionallMovesTitle;

///Buttons
Button* TempusImperium::skipButton;
Button* TempusImperium::activateButton;

Button* TempusImperium::buildBuildingButton;
Button* TempusImperium::digButton;
Button* TempusImperium::currentActionButton;

Button* TempusImperium::buildRoadButton; //какого хера это GameObject,а не Button?ПопозжеsetButtonPosition

Button* TempusImperium::endCollectResources;

Button* TempusImperium::buildFarmButton;
Button* TempusImperium::buildMarketButton;
Button* TempusImperium::buildFortmButton;
Button* TempusImperium::buildCastleButton;


bool TempusImperium::isNextTurn;

enum class TileType
{
	Trees,
	Rocks,
	Farm,
	River,
	Quarry,
	Market
};

void TempusImperium::deactivateAllRoads(){
	for(int i = 0;i < TempusImperium::roadVector.size();i++){
		if(!TempusImperium::roadVector[i]->isBuilded){
			TempusImperium::roadVector[i]->setFillColor(sf::Color(59,20,20,50));
		}
		else{
			TempusImperium::roadVector[i]->setFillColor(sf::Color(110, 113, 117));	
		}
		TempusImperium::roadVector[i]->updateVector.clear();
	}
}

void TempusImperium::deleteButtonFromPool(Button* &pButton){
	
	//тут циклы для удаления геймобжекта из статус и дров пула
	Pool::Pool::statusPool.erase(std::remove(begin(Pool::Pool::statusPool), end(Pool::Pool::statusPool), pButton),
             end(Pool::Pool::statusPool));
	Pool::Pool::statusPool.shrink_to_fit();

	Pool::Pool::drawPool.erase(std::remove(begin(Pool::Pool::drawPool), end(Pool::Pool::drawPool), pButton),
             end(Pool::Pool::drawPool));
	Pool::Pool::drawPool.shrink_to_fit();

	Pool::Pool::drawPool.erase(std::remove(begin(Pool::Pool::drawPool), end(Pool::Pool::drawPool), &(pButton->buttonText)),
             end(Pool::Pool::drawPool));
	Pool::Pool::drawPool.shrink_to_fit();

	delete pButton;
	pButton = nullptr;
}

void TempusImperium::hideCurrentTurnUI(){
	//удаляем все видимые элементы:
		//Кнопка активного хода
		//дополнительные кнопки
		//Title-ы и прочий текст
	//TempusImperium::deleteButtonFromPool();
	TempusImperium::deinitAdditionalActionButton();
	TempusImperium::deinitialiseGameUI();
	TempusImperium::deleteButtonFromPool(TempusImperium::currentActionButton);
}

void TempusImperium::deinitAdditionalActionButton(){
	if(TempusImperium::skipButton!=nullptr){
		TempusImperium::deleteButtonFromPool(TempusImperium::skipButton);
	}
	if(TempusImperium::activateButton!=nullptr){
		TempusImperium::deleteButtonFromPool(TempusImperium::activateButton);
	}
	if(TempusImperium::buildBuildingButton!=nullptr){
		TempusImperium::deleteButtonFromPool(TempusImperium::buildBuildingButton);
	}
	if(TempusImperium::digButton!=nullptr){
		TempusImperium::deleteButtonFromPool(TempusImperium::digButton);
	}
	if(TempusImperium::buildRoadButton!=nullptr){
		TempusImperium::deleteButtonFromPool(TempusImperium::buildRoadButton);
	}
}

void TempusImperium::deactivateAllCells(){
	for(int i = 0;i < TempusImperium::grid.size();i++){
		if(TempusImperium::grid[i]->cellSprite != nullptr){
			TempusImperium::grid[i]->setFillColor(sf::Color(255,255,123,0));
		}
		else if(TempusImperium::grid[i]->cellSprite == nullptr){
			TempusImperium::grid[i]->setFillColor(sf::Color::Red);
		}
		TempusImperium::grid[i]->methodVector.clear();
	}
	
}



void TempusImperium::initEnemy(){
	
	std::vector<Cell*> freeCellsCollection;
	for(int i = 0;i < TempusImperium::grid.size();i++){
		if(TempusImperium::grid[i]->cellSprite == nullptr){
			freeCellsCollection.push_back(TempusImperium::grid[i]);
		}
	}

	std::random_shuffle(freeCellsCollection.begin(),freeCellsCollection.end());

	for(int i = 0;i < 10;i++){
		freeCellsCollection[i]->setCellObject(new Building(BuildingType::Enemy));
	}
}

void TempusImperium::acitvateEveryEnemy(){
	
	int enemyVectorSize = TempusImperium::enemyVector.size();

	//перебираем все клетки врагов
	for(int i = 0;i < enemyVectorSize;i++){

		//перебираем все ближайшие клетки врагов
		if(TempusImperium::enemyVector[i]->parentCell != nullptr){
			//std::cout << i << "\t is not nullptr" << std::endl;
		}
		if(TempusImperium::enemyVector[i]->isKilled){
			continue;
		}
		for(Cell* tmpCell : TempusImperium::enemyVector[i]->parentCell->adjacentCells){
			if(tmpCell != nullptr){
				if(tmpCell->cellSprite != nullptr){
					if(tmpCell->cellSprite->buildingType != BuildingType::Tree && 
					   tmpCell->cellSprite->buildingType != BuildingType::Rock &&
					   tmpCell->cellSprite->buildingType != BuildingType::Enemy &&
					   tmpCell->cellSprite->buildingType != BuildingType::Lake){
						TempusImperium::goldCount--;
						break;
					}
				}
			}	
		}
	}
}

//вот тут проёб с памятью,подлатать
//вот тут надо сделать так чтобы при инициализации доп.кнопок в конструктор
//пихать булево, которое говорит о том что надо инициализировать кнопку как дополнительную

void TempusImperium::initAdditionalActionButton(){
	
	//добавить массив с точными позициями новых кнопок
	//clear all additional buttons
	TempusImperium::deleteButtonFromPool(TempusImperium::skipButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::activateButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildBuildingButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::digButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildRoadButton);

	int w=200;
	int h=50;
	int defaultFontSize = 16;

	int xScaler = 15;
	int currentTurnButtonPosX =xScaler + (TempusImperium::backgroundGrid->getPosition().x + TempusImperium::backgroundGrid->getGlobalBounds().width/2);
	
	//инициализировать все остальные кнопки
	switch(TempusImperium::queryAction[TempusImperium::currentQueryActionIndex]){
		case ActionType::Build: //если сейчас строится здание...
				
				TempusImperium::skipButton = new Button("Skip turn",32,0,0,w,h);
				TempusImperium::skipButton->setMethod(&Button::skipTurn);
				TempusImperium::skipButton->setButtonPosition(currentTurnButtonPosX+TempusImperium::skipButton->getGlobalBounds().width/2,30);
				
				
				TempusImperium::activateButton = new Button("Activate",defaultFontSize,0,0,w,h,true);
				TempusImperium::activateButton->setMethod(&Button::activateTurn);
				TempusImperium::skipButton->setAdjacentButton(TempusImperium::activateButton,3,70);
				

				TempusImperium::buildRoadButton = new Button("Build road",defaultFontSize,0,0,w,h,true);
				TempusImperium::buildRoadButton->setMethod(&Button::buildRoadTurn);
				TempusImperium::activateButton->setAdjacentButton(TempusImperium::buildRoadButton,3,70);

				TempusImperium::digButton = new Button("Dig",defaultFontSize,0,0,w,h,true);
				TempusImperium::digButton->setMethod(&Button::digTurn);
				TempusImperium::buildRoadButton->setAdjacentButton(TempusImperium::digButton,3,70);
			break;
		case ActionType::Activate: //если сейчас ход активации
				TempusImperium::skipButton = new Button("Skip turn",32,0,0,w,h);
				TempusImperium::skipButton->setMethod(&Button::skipTurn);
				TempusImperium::skipButton->setButtonPosition(currentTurnButtonPosX+TempusImperium::skipButton->getGlobalBounds().width/2,30);

				TempusImperium::buildBuildingButton = new Button("Build",30,0,0,w,h,true);
				TempusImperium::buildBuildingButton->setMethod(&Button::buildTurn);
				TempusImperium::skipButton->setAdjacentButton(TempusImperium::buildBuildingButton,3,70);


				TempusImperium::buildRoadButton = new Button("Build road",defaultFontSize,0,0,w,h,true);
				TempusImperium::buildRoadButton->setMethod(&Button::buildRoadTurn);
				TempusImperium::buildBuildingButton->setAdjacentButton(TempusImperium::buildRoadButton,3,70);

				TempusImperium::digButton = new Button("Dig",defaultFontSize,0,0,w,h,true);
				TempusImperium::digButton->setMethod(&Button::digTurn);
				TempusImperium::buildRoadButton->setAdjacentButton(TempusImperium::digButton,3,70);
			break;
		case ActionType::Dig: //если сейчас действие копания

				TempusImperium::skipButton = new Button("Skip turn",32,0,0,w,h);
				TempusImperium::skipButton->setMethod(&Button::skipTurn);
				TempusImperium::skipButton->setButtonPosition(currentTurnButtonPosX+TempusImperium::skipButton->getGlobalBounds().width/2,30);

				TempusImperium::activateButton = new Button("Activate",defaultFontSize,0,0,w,h,true);
				TempusImperium::activateButton->setMethod(&Button::activateTurn);
				TempusImperium::skipButton->setAdjacentButton(TempusImperium::activateButton,3,70);
				
				TempusImperium::buildBuildingButton = new Button("Build",30,0,0,w,h,true);
				TempusImperium::buildBuildingButton->setMethod(&Button::buildTurn);
				TempusImperium::activateButton->setAdjacentButton(TempusImperium::buildBuildingButton,3,70);

				TempusImperium::buildRoadButton = new Button("Build road",defaultFontSize,0,0,w,h,true);
				TempusImperium::buildRoadButton->setMethod(&Button::buildRoadTurn);
				TempusImperium::buildBuildingButton->setAdjacentButton(TempusImperium::buildRoadButton,3,70);

			break;
		case ActionType::BuildRoad: //если сейчас действие постройки дороги
				TempusImperium::skipButton = new Button("Skip turn",32,0,0,w,h);
				TempusImperium::skipButton->setMethod(&Button::skipTurn);
				TempusImperium::skipButton->setButtonPosition(currentTurnButtonPosX+TempusImperium::skipButton->getGlobalBounds().width/2,30);

				TempusImperium::activateButton = new Button("Activate",defaultFontSize,0,0,w,h,true);
				TempusImperium::activateButton->setMethod(&Button::activateTurn);
				TempusImperium::skipButton->setAdjacentButton(TempusImperium::activateButton,3,70);
				
				TempusImperium::buildBuildingButton = new Button("Build",30,0,0,w,h,true);
				TempusImperium::buildBuildingButton->setMethod(&Button::buildTurn);
				TempusImperium::activateButton->setAdjacentButton(TempusImperium::buildBuildingButton,3,70);

				TempusImperium::digButton = new Button("Dig",defaultFontSize,0,0,w,h,true);
				TempusImperium::digButton->setMethod(&Button::digTurn);
				TempusImperium::buildBuildingButton->setAdjacentButton(TempusImperium::digButton,3,70);
			break;
	}
}

void TempusImperium::clearData(){

	delete TempusImperium::backgroundGrid;
	delete TempusImperium::currentRoundTitle;
	delete TempusImperium::currentTurnTitle;
	delete TempusImperium::currentGoldCountTitle;
	delete TempusImperium::additionallMovesTitle;

	delete TempusImperium::activateButton;
	delete TempusImperium::skipButton;
	delete TempusImperium::buildRoadButton;
	delete TempusImperium::buildBuildingButton;
	delete TempusImperium::digButton;

	delete TempusImperium::currentActionButton;

	//тут наверное лучше сперва очищать все оъекты из статус и дроу пула

	for(int i = 0;i < TempusImperium::grid.size();i++){
	 	delete TempusImperium::grid[i];
	}

	for(auto road : TempusImperium::roadVector){
		delete road;
	}

	for(auto farm : TempusImperium::farmVector){
		delete farm;
	}

	for(auto market: TempusImperium::marketVector){
		delete market;
	}

	for(auto fort: TempusImperium::fortVector){
		delete fort;
	}

	for(auto castle: TempusImperium::castleVector){
		delete castle;
	}
}
 
void TempusImperium::updateUIStatus(){
	if(TempusImperium::currentRoundTitle != nullptr)
		TempusImperium::currentRoundTitle->txt.setString("Current round :" + std::to_string(TempusImperium::currentRound) + "/5");
	if(TempusImperium::currentTurnTitle != nullptr)
		TempusImperium::currentTurnTitle->txt.setString("Current turn :" + std::to_string(TempusImperium::currentTurn) + "/10");
	if(TempusImperium::currentTurnTitle != nullptr)
		TempusImperium::currentGoldCountTitle->txt.setString("Gold count:" + std::to_string(TempusImperium::goldCount));
	//
	//хуйню ниже можно запихать в отдельную кнопку
	if(TempusImperium::currentActionButton == nullptr && TempusImperium::isNextTurn){ //expermental
		TempusImperium::isNextTurn = false;
		float currentTurnButtonPosY = -100;
		int defaultFontSize = 32;
		int w = 200;
		int h = 100;
		float xScaler = 35;
		float currentTurnButtonPosX =xScaler + (TempusImperium::backgroundGrid->getPosition().x + TempusImperium::backgroundGrid->getGlobalBounds().width/2);

		switch(TempusImperium::queryAction[TempusImperium::currentQueryActionIndex]){
			case ActionType::Build:
					TempusImperium::currentActionButton = new Button("Build",defaultFontSize,0,0,w,h);
					TempusImperium::currentActionButton->setButtonPosition(currentTurnButtonPosX + TempusImperium::currentActionButton->getGlobalBounds().width/2,currentTurnButtonPosY);
					TempusImperium::currentActionButton->setMethod(&Button::buildTurn);
				break;
			case ActionType::Activate:
					TempusImperium::currentActionButton = new Button("Activate",defaultFontSize,0,0,w,h);
					TempusImperium::currentActionButton->setButtonPosition(currentTurnButtonPosX + TempusImperium::currentActionButton->getGlobalBounds().width/2,currentTurnButtonPosY);
					TempusImperium::currentActionButton->setMethod(&Button::activateTurn);
				break;
			case ActionType::Dig:
					TempusImperium::currentActionButton = new Button("Dig",defaultFontSize,0,0,w,h);
					TempusImperium::currentActionButton->setButtonPosition(currentTurnButtonPosX + TempusImperium::currentActionButton->getGlobalBounds().width/2,currentTurnButtonPosY);
					TempusImperium::currentActionButton->setMethod(&Button::digTurn);
				break;
			case ActionType::BuildRoad:
					TempusImperium::currentActionButton = new Button("Build road",defaultFontSize,0,0,w,h);
					TempusImperium::currentActionButton->setButtonPosition(currentTurnButtonPosX + TempusImperium::currentActionButton->getGlobalBounds().width/2,currentTurnButtonPosY);
					TempusImperium::currentActionButton->setMethod(&Button::buildRoadTurn);
				break;
		}
	}

}

void TempusImperium::initHorizontalRoads(){
	float firstXHorRoad = -400 + 30 + 16;
	float distanceBetweenHorRoadX = 15 + 33;
	float firstYHorRoad = -248 + 8;
	float distanceBetweenHorRoadY = 15 + 30;
	//инициализируем список дорог
	//сперва инициализируем все горизонтальные дороги
	for(int row = 0;row < 11;row++){
		for(int x = 0;x < 10;x++){
			Road* tmpRoad = new Road(firstXHorRoad,firstYHorRoad ,true);
			tmpRoad->colIndex = x;
			tmpRoad->rowIndex = row;
			TempusImperium::roadVector.push_back(tmpRoad);
			firstXHorRoad +=distanceBetweenHorRoadX;
		}
		firstXHorRoad = -400 + 30 + 16;
		firstYHorRoad += distanceBetweenHorRoadX;
	}
}

void TempusImperium::initVerticalRoads(){
	float firstXHorRoad = -400 + 15 + 8;
	float firstYHorRoad = -248 + 15+ 16;

	float distanceBetweenHorRoadX = 33 + 15;
	float distanceBetweenHorRoadY = 15+33;
	
	for(int row = 0;row < 10;row++){
		for(int x = 0;x < 11;x++){
			
			Road* tmpRoad = new Road(firstXHorRoad,firstYHorRoad ,false);
			tmpRoad->colIndex = x;
			tmpRoad->rowIndex = row;
			
			TempusImperium::roadVector.push_back(tmpRoad);
			firstXHorRoad += distanceBetweenHorRoadX;
		}
		firstXHorRoad = -400+15 + 8;
		firstYHorRoad+=distanceBetweenHorRoadY;
	}
}

void TempusImperium::setAdajcentsRoads(){
	for(int currentRoadIndex = 0;currentRoadIndex < TempusImperium::roadVector.size();currentRoadIndex++){
		Road* pCurrentRoad = TempusImperium::roadVector[currentRoadIndex];
		for(int nextRoadIndex = 0;nextRoadIndex < TempusImperium::roadVector.size();nextRoadIndex++){
			Road* pSecondRoad = TempusImperium::roadVector[nextRoadIndex];
			if( currentRoadIndex!=nextRoadIndex ){
				if(pCurrentRoad->isRoadAdjacent(pSecondRoad)){
					pCurrentRoad->adjacentRoads.push_back(pSecondRoad);
				}
			}
		}
	}
}

//вроде эта штука нормально работает
//проставляем дорогам соседстующие клетки
//проставвляем клеткам соседние дороги
void TempusImperium::setAdjacentCellToRoads(){
	for(auto CurrentRoad : TempusImperium::roadVector){
		for(auto CurrentCell : TempusImperium::grid){
			if(CurrentRoad->isHorizontal){
				if(CurrentRoad->colIndex == CurrentCell->gridX &&(CurrentRoad->rowIndex == CurrentCell->gridY||CurrentRoad->rowIndex-1 == CurrentCell->gridY )){
					CurrentRoad->adjacentCells.push_back(CurrentCell);
					CurrentCell->adjacentRoads.push_back(CurrentRoad);
				}
			}
			else if(!CurrentRoad->isHorizontal){
				if(CurrentRoad->rowIndex == CurrentCell->gridY && (CurrentRoad->colIndex-1 == CurrentCell->gridX||CurrentRoad->colIndex == CurrentCell->gridX) ){
					CurrentRoad->adjacentCells.push_back(CurrentCell);
					CurrentCell->adjacentRoads.push_back(CurrentRoad);
				}
			}
		}
	}
}


void TempusImperium::initGameUI(){ //удалена инициализация кнопок
	
	int xScaler = 5;
	float rightSide = TempusImperium::backgroundGrid->getPosition().x + TempusImperium::backgroundGrid->getGlobalBounds().width/2;
	float localhalfWidth;
	if(TempusImperium::currentRoundTitle == nullptr){
		TempusImperium::currentRoundTitle = new Title("Current round :",20,sf::Color::White);
		localhalfWidth = TempusImperium::currentRoundTitle->txt.getGlobalBounds().width/2;
		TempusImperium::currentRoundTitle->txt.setPosition(localhalfWidth + rightSide + xScaler,-238);
	}

	if(TempusImperium::currentTurnTitle==nullptr){
		TempusImperium::currentTurnTitle = new Title("Current turn :",20,sf::Color::White);
		localhalfWidth = TempusImperium::currentTurnTitle->txt.getGlobalBounds().width/2;
		TempusImperium::currentTurnTitle->txt.setPosition(localhalfWidth + xScaler + rightSide,-218);
	}
	if(TempusImperium::currentGoldCountTitle==nullptr){
		TempusImperium::currentGoldCountTitle = new Title("Gold count:",20,sf::Color::White);
		localhalfWidth = TempusImperium::currentGoldCountTitle->txt.getGlobalBounds().width/2;
		TempusImperium::currentGoldCountTitle->txt.setPosition(localhalfWidth + xScaler + rightSide,-198);	
	}
	if(TempusImperium::currentMainActionTitle==nullptr){
		TempusImperium::currentMainActionTitle = new Title("Current main action:",20,sf::Color::White);
		localhalfWidth = TempusImperium::currentMainActionTitle->txt.getGlobalBounds().width/2;
		TempusImperium::currentMainActionTitle->txt.setPosition(localhalfWidth + xScaler + rightSide,-178);	
	}
	if(TempusImperium::additionallMovesTitle==nullptr){
		TempusImperium::additionallMovesTitle = new Title("Additional moves",20,sf::Color::White);
		localhalfWidth = TempusImperium::additionallMovesTitle->txt.getGlobalBounds().width/2;
		TempusImperium::additionallMovesTitle->txt.setPosition(localhalfWidth + xScaler + rightSide,-28);	
	}

	TempusImperium::initAdditionalActionButton();
	
}

void TempusImperium::deinitialiseGameUI(){
	if(TempusImperium::currentRoundTitle != nullptr){
		TempusImperium::deleteTitleFromDrawPool(TempusImperium::currentRoundTitle);
		delete TempusImperium::currentRoundTitle;
		TempusImperium::currentRoundTitle = nullptr;
	}

	if(TempusImperium::currentTurnTitle != nullptr){
		TempusImperium::deleteTitleFromDrawPool(TempusImperium::currentTurnTitle);
		delete TempusImperium::currentTurnTitle ;
		TempusImperium::currentTurnTitle  = nullptr;
	}

	if(TempusImperium::currentGoldCountTitle!= nullptr){
		TempusImperium::deleteTitleFromDrawPool(TempusImperium::currentGoldCountTitle);
		delete TempusImperium::currentGoldCountTitle;
		TempusImperium::currentGoldCountTitle = nullptr;
	}

	if(TempusImperium::currentMainActionTitle!= nullptr){
		TempusImperium::deleteTitleFromDrawPool(TempusImperium::currentMainActionTitle);
		delete TempusImperium::currentMainActionTitle;
		TempusImperium::currentMainActionTitle = nullptr;
	}

	if(TempusImperium::additionallMovesTitle != nullptr){
		TempusImperium::deleteTitleFromDrawPool(TempusImperium::additionallMovesTitle);
		delete TempusImperium::additionallMovesTitle;
		TempusImperium::additionallMovesTitle = nullptr;
	}
}

void TempusImperium::initDateMas(){
	time_t now = time(0);
	
	tm *ltm = localtime(&now);
	
	float pYear = 1900 + ltm->tm_year;
	int year = pYear - ((int)(pYear/100)*100);
	TempusImperium::dateMas[0] = year/10;
	TempusImperium::dateMas[1] = year - year/10*10;

	//за последующую херню прямо писос как стыдно. И иначе как проще получить цифры по отдельности из двухзначного числа
	int month =  1 + ltm->tm_mon;
	if(month < 10){
		TempusImperium::dateMas[2] = 0;
		TempusImperium::dateMas[3] = month;
	}
	else if(month == 10){
		TempusImperium::dateMas[2] = 1;
		TempusImperium::dateMas[3] = 0;
	}
	else if(month < 10){
		TempusImperium::dateMas[2] = month/10;
		TempusImperium::dateMas[3] =	month - month/10*10;
	}

	int day =  ltm->tm_mday;
	if(day < 10){
		TempusImperium::dateMas[4] = 0;
		TempusImperium::dateMas[5] = day;
	}
	else if(day % 10 == 0){
		TempusImperium::dateMas[4] = day/10;
		TempusImperium::dateMas[5] = 0;	
	}
	else{
		TempusImperium::dateMas[4] = day/10;
		TempusImperium::dateMas[5] = day - day/10*10;		
	}

	int hour = 5+ltm->tm_hour;
	if(hour < 10){
		TempusImperium::dateMas[6] = 0;
		TempusImperium::dateMas[7] = hour;
	}
	else if(hour % 10 == 0){
		TempusImperium::dateMas[6] = hour/10;
		TempusImperium::dateMas[7] = 0;
	}

	else{
		TempusImperium::dateMas[6] = hour/10;
		TempusImperium::dateMas[7] = hour - hour/10*10;
	}
	

	int min =  30+ltm->tm_min;
	if(min < 10){
		TempusImperium::dateMas[8] = 0 ;
		TempusImperium::dateMas[9] = min;
	}
	else if(min % 10 == 0){
		TempusImperium::dateMas[8] = min/10;
		TempusImperium::dateMas[9] = 0;
	}
	else{
		TempusImperium::dateMas[8] = min/10;
		TempusImperium::dateMas[9] = min - min/10 * 10;
	}
}

void TempusImperium::setQueryAction(int dateQuery[10]){
	for(int i = 0;i < 10;i++){
		if(dateQuery[i] == 0){
			TempusImperium::queryAction.push_back(ActionType::Activate);
		}
		else if(dateQuery[i] ==1 || dateQuery[i] == 2){
			TempusImperium::queryAction.push_back(ActionType::BuildRoad);
		}
		else if(dateQuery[i] ==3 || dateQuery[i] == 4){
			TempusImperium::queryAction.push_back(ActionType::Dig);
		}
		else if( dateQuery[i] >= 5 || dateQuery[i] <= 9){
			TempusImperium::queryAction.push_back(ActionType::Build);
		}
	}
}

void TempusImperium::getPlayersSetStartTileInput(std::vector<std::vector<Cell*>> &colRow,int f,int ff){

	for(int i = 0;i<colRow[f][ff]->adjacentCells.size();i++){
		colRow[f][ff]->adjacentCells[i]->methodVector.push_back(&Cell::setAdjacentObject);
	}

	while(TempusImperium::tmpGameObject != nullptr){//вот тут вот хуйня
		//if(Pool::DrawPool::renderWindow->pollEvent(*Pool::Pool::event)){
			Pool::DrawPool::updateDrawPool();
			Pool::StatusPool::updateStatusPool();
		//}
	}

	for(int i = 0;i<colRow[f][ff]->adjacentCells.size();i++){
		colRow[f][ff]->adjacentCells[i]->methodVector.clear();
	}
	//TempusImperium::deletePromptTitle();
}

//тут делать delete ко всем элементам
void TempusImperium::deactivateAllBuildings(){
	for(auto farm : TempusImperium::farmVector){
		farm->updateVector.clear();
		farm->updateMethod = nullptr;
	}
	for(auto market : TempusImperium::marketVector){
		market->updateVector.clear();
		market->updateMethod = nullptr;
	}
	for(auto fort : TempusImperium::fortVector){
		fort->updateVector.clear();
		fort->updateMethod = nullptr;
	}
	for(auto castle : TempusImperium::castleVector){
		castle->updateVector.clear();
		castle->updateMethod = nullptr;
	}
}

void TempusImperium::initUserData(){

	TempusImperium::activateButton  = nullptr;
	TempusImperium::skipButton = nullptr;
	TempusImperium::buildBuildingButton = nullptr;
	TempusImperium::digButton = nullptr;
	TempusImperium::buildRoadButton = nullptr;

	TempusImperium::currentRoundTitle = nullptr;
	TempusImperium::currentTurnTitle = nullptr;
	TempusImperium::currentGoldCountTitle = nullptr;
	TempusImperium::currentMainActionTitle = nullptr;
	TempusImperium::additionallMovesTitle = nullptr;

	TempusImperium::isNextTurn = true;	
	TempusImperium::goldCount = 10;
	TempusImperium::backgroundGrid = new Background("Images/Buildings/gridBackgroundImage.png",sf::IntRect(0,0,495,495));
	TempusImperium::backgroundGrid->setSize(sf::Vector2f(495,495));
	TempusImperium::backgroundGrid->setOrigin(248,248);
	TempusImperium::backgroundGrid->setPosition(-400 + 248 + 15,0);

	//15 - road width
	//32 - tile width/height
	//11 * 15 + 32 * 10
	float firstX = -400 + 30 + 16;
	float firstY = -248 + 15 + 16; //16 - center of 33 / 2
	
	int rowIndex = 1;
	int colIndex = 1;
	float distanceBetweenCells = 15+33;
	using namespace std;

	//init grid positions and data cells
	for(int y = 0;y < 10;y++){
		for(int x = 0;x < 10;x++){
			TempusImperium::grid.push_back(new Cell(firstX,firstY));
			TempusImperium::grid[TempusImperium::grid.size()-1]->gridX = x;
			TempusImperium::grid[TempusImperium::grid.size()-1]->gridY = y;
			TempusImperium::grid[TempusImperium::grid.size()-1]->rowIndex = rowIndex;
			TempusImperium::grid[TempusImperium::grid.size()-1]->columnIndex = colIndex;
			colIndex++;
			firstX += distanceBetweenCells;
			if(colIndex==10){
				colIndex=0;
			}
		}
		firstX = -400 + 30 + 16;
		firstY +=distanceBetweenCells;
		rowIndex++;
		if(rowIndex==10){
			rowIndex = 0;
		}
	}

	//заполняем ближайшего соседа для каждой из клетки
	//вроде этот метод рабочий
	TempusImperium::setAdjacentsCell();
	
	//INIT HORIZONTAL ROADS
	TempusImperium::initHorizontalRoads();
	TempusImperium::initVerticalRoads();
	TempusImperium::setAdajcentsRoads();

    //тут тотальный проёб с получением акутальной даты
	TempusImperium::initDateMas();	

	TempusImperium::setQueryAction(TempusImperium::dateMas);

	TempusImperium::setAdjacentCellToRoads();

	//вот эту хуйню можно потом заполнять случайными элементами в случайном порядке
	//вариации рандомизаций - полно
	//сделать настройки с выбором рандомизации поля
	//сделать запрос на ручную установку тайлов
	TileType tileColumnMas[10] = {TileType::Trees,TileType::River,TileType::Quarry,TileType::Farm,TileType::Trees,
								  TileType::Market,TileType::River,TileType::Farm,TileType::Trees,TileType::River};

	TileType tileRowMas[10] = {TileType::Quarry,TileType::Trees,TileType::Quarry,TileType::Trees,TileType::Quarry,
						  TileType::Trees,TileType::Quarry,TileType::Trees,TileType::Quarry,TileType::Trees};

	//8 дерева 
	//6 камней
	std::vector<std::vector<Cell*>> columnsCollection;
	std::vector<std::vector<Cell*>> rowCollection;
	//тупо собираем коллекцию колонок и строк
	for(int i = 1;i < 10;i++){
		columnsCollection.push_back(std::vector<Cell*>());
		rowCollection.push_back(std::vector<Cell*>());
		for(int cellIndex = 0;cellIndex < TempusImperium::grid.size();cellIndex++){
			if(TempusImperium::grid[cellIndex]->columnIndex == i){
				columnsCollection[columnsCollection.size()-1].push_back(TempusImperium::grid[cellIndex]);
			}
			if(TempusImperium::grid[cellIndex]->rowIndex == i){
				rowCollection[rowCollection.size()-1].push_back(TempusImperium::grid[cellIndex]);
			}
		}
	}
	columnsCollection.push_back(std::vector<Cell*>());
	rowCollection.push_back(std::vector<Cell*>());

	for(int cellIndex = 0;cellIndex < TempusImperium::grid.size();cellIndex++){
		if(TempusImperium::grid[cellIndex]->columnIndex == 0){
			columnsCollection[columnsCollection.size()-1].push_back(TempusImperium::grid[cellIndex]);
		}
		if(TempusImperium::grid[cellIndex]->rowIndex == 0){
			rowCollection[rowCollection.size()-1].push_back(TempusImperium::grid[cellIndex]);
		}
	}
	Building* tmpMarket;
	Building* tmpFarm;
	Building* tmpLake;
	for(int col =0;col < 10;col++){ //перебираем коллекцию колонок
		for(int row =0;row< 10;row++){ //перебираем все элементы в текущей колонке(да хуй с ним, пускай будем строчки перебирать)
			if(columnsCollection[col][row]->rowIndex == TempusImperium::dateMas[col]){
								
				switch(tileColumnMas[col]){
					case TileType::Trees:
							columnsCollection[col][row]->setCellObject(new Building(BuildingType::Tree));
						break;
					case TileType::Quarry:
							columnsCollection[col][row]->setCellObject(new Building(BuildingType::Rock));
						break;
					case TileType::Farm:
							tmpFarm =  new Building(BuildingType::Farm);
							columnsCollection[col][row]->setCellObject(tmpFarm);
						break;
					case TileType::Market:
							tmpMarket = new Building(BuildingType::Market);
							columnsCollection[col][row]->setCellObject(tmpMarket);
						break;
					case TileType::River:
							tmpLake = new Building(BuildingType::Lake);
							columnsCollection[col][row]->setCellObject(tmpLake);
							TempusImperium::lakeVector.push_back(tmpLake);
						break;
				}
			}
		}
	}
	
	//вот эту хуйню решить
	//сделать функци с просьбой кликнуть на свободную рядом клетку
	for(int row = 0;row < 10;row++){
		for(int col = 0;col < 10;col++){
			if(rowCollection[row][col]->columnIndex == TempusImperium::dateMas[row] && rowCollection[row][col]->cellSprite == nullptr){
				switch(tileRowMas[row]){
					case TileType::Trees:
							rowCollection[row][col]->setCellObject(new Building(BuildingType::Tree));
						break;
					case TileType::Quarry:
							rowCollection[row][col]->setCellObject(new Building(BuildingType::Rock));
						break;
				}
				continue;
			}
			else if(rowCollection[row][col]->cellSprite != nullptr && rowCollection[row][col]->columnIndex == TempusImperium::dateMas[row]){

				switch(tileRowMas[row]){
					case TileType::Trees:
							TempusImperium::tmpGameObject = new Building(BuildingType::Tree);
						break;
					case TileType::Quarry:
							TempusImperium::tmpGameObject = new Building(BuildingType::Rock);
						break;
					
				}
				/*TempusImperium::setCellPrompt = new Title("Please select a cell\nto place the resource ",20,sf::Color::White);
				TempusImperium::setCellPrompt->txt.setPosition(248+15,-238);*/
				TempusImperium::tmpGameObject->setPosition(250,-190);
				//вот эту хуйню закинуть в поток
				//лучше оставить под отдбельный поток
				for(int i = 0;i<rowCollection[row][col]->adjacentCells.size();i++){
					if(rowCollection[row][col]->adjacentCells[i]->cellSprite == nullptr){
						rowCollection[row][col]->adjacentCells[i]->setFillColor(sf::Color::Green);
					}
				}
	
				//вот здесь подписать клетки на событие и убрать хуйню связанную с потоками
				TempusImperium::myThread = new std::thread(TempusImperium::getPlayersSetStartTileInput,std::ref(rowCollection),row,col);
				TempusImperium::myThread->join();
				delete TempusImperium::myThread;
				
				for(int i = 0;i<rowCollection[row][col]->adjacentCells.size();i++){
					if(rowCollection[row][col]->adjacentCells[i]->cellSprite == nullptr){
						rowCollection[row][col]->adjacentCells[i]->setFillColor(sf::Color::Red);
					}
				}
			}
		}
	}

	//std::cout << "Раскладка - Done\n";
	//а теперь вот здесь начинаем делать кнопки
	TempusImperium::initEnemy();
	TempusImperium::initGameUI(); //удалил инициализацию доп кнопок
}

void Background::updateStatus(){}

Background::Background(std::string path,sf::IntRect rect) : GameObject(path,rect){

}

//зачем? попозже удалить
void TempusImperium::setActivateAllBuildings(){
	for(auto farm : farmVector){
		//farm->updateVector.push_back(&Building::activateFarm);
		farm->updateMethod = &Building::activateFarm;
	}
	for(auto market : marketVector){
		//market->updateVector.push_back(&Building::activateMarket);
		market->updateMethod=&Building::activateMarket;
	}
	for(auto fort : fortVector){
		//fort->updateVector.push_back(&Building::activateFort);
		fort->updateMethod=&Building::activateFort;
	}
	for(auto castle : castleVector){
		//castle->updateVector.push_back(&Building::activateCastle);
		castle->updateMethod=&Building::activateCastle;
	}
}

void TempusImperium::setAdjacentsCell(){
	Cell* currentCell;
	Cell* nextCell;
	for(int currentCellIndex = 0;currentCellIndex< TempusImperium::grid.size();currentCellIndex++){
		currentCell = TempusImperium::grid[currentCellIndex];
		for(int nextCellIndex = 0;nextCellIndex<TempusImperium::grid.size();nextCellIndex++){
			nextCell = TempusImperium::grid[nextCellIndex];
			if(currentCellIndex != nextCellIndex){
				if(currentCell->gridX == nextCell->gridX && (currentCell->gridY == nextCell->gridY+1 ||currentCell->gridY == nextCell->gridY-1 )){
					currentCell->adjacentCells.push_back(nextCell);
				}
				else if(currentCell->gridY == nextCell->gridY && (currentCell->gridX == nextCell->gridX+1 ||currentCell->gridX == nextCell->gridX-1 )){
					currentCell->adjacentCells.push_back(nextCell);
				}
			}
		}
	}
}

void TempusImperium::goToNextTurn(){

	TempusImperium::currentTurn++;
	TempusImperium::currentQueryActionIndex++;
	
	if(TempusImperium::currentTurn == 10){
		TempusImperium::currentQueryActionIndex = 0;
		TempusImperium::currentRound++;
		TempusImperium::acitvateEveryEnemy();
		TempusImperium::currentTurn = 1;
		for(Building* farm : TempusImperium::farmVector){
			farm->isUsed = false;
		}
		for(Building* fort : TempusImperium::fortVector){
			fort->isUsed = false;
		}
		for(Building* castle : TempusImperium::castleVector){
			castle->isUsed = false;
		}
		for(Building* market : TempusImperium::marketVector){
			market->isUsed = false;
		}
	}
	
	if(TempusImperium::currentRound == 6){
		std::cout << "END GAME\n";
		TempusImperium::calculateFinalScore();// for test
	}
	TempusImperium::isNextTurn = true;
	TempusImperium::deleteButtonFromPool(TempusImperium::currentActionButton); //возможен тут проёб
	//next two line - for test
	TempusImperium::deinitialiseGameUI();
	TempusImperium::initGameUI();
}

void TempusImperium::deleteButtonFromPool(){

	/*for(int i = 0;i < Pool::Pool::drawPool.size();i++){
		if(Pool::Pool::drawPool[i] == &(TempusImperium::currentActionButton->buttonText)){
			Pool::Pool::drawPool.erase(Pool::Pool::drawPool.begin()+i);
			break;
		}
	}
	for(int i = 0;i < Pool::Pool::drawPool.size();i++){
		if(Pool::Pool::drawPool[i] == TempusImperium::currentActionButton){
			Pool::Pool::drawPool.erase(Pool::Pool::drawPool.begin()+i);
			break;
		}
	}
    for(int i = 0;i < Pool::Pool::statusPool.size();i++){
		if(Pool::Pool::statusPool[i] == TempusImperium::currentActionButton){
			Pool::Pool::statusPool.erase(Pool::Pool::statusPool.begin()+i);
			break;
		}
	}*/
	delete TempusImperium::currentActionButton;
	TempusImperium::currentActionButton = nullptr; //experimental
}


//в принципе, вот эту херню можно удалтить
void TempusImperium::deleteTitleFromDrawPool(Title* &toDelete){
	for(int i = 0;i < Pool::Pool::drawPool.size();i++){
		if(Pool::Pool::drawPool[i] == toDelete){
			Pool::Pool::drawPool.erase(Pool::Pool::drawPool.begin()+i);
			break;
		}
	}
}
//вот эту хуйню переделать под нормальное удаление из пула
void TempusImperium::createPrompt(std::string prompString,int fontSize){
	TempusImperium::promptTitle = new Title(prompString,fontSize,sf::Color::White);
	float rightSide = TempusImperium::backgroundGrid->getPosition().x + TempusImperium::backgroundGrid->getGlobalBounds().width/2;
	float localhalfWidth = TempusImperium::promptTitle->txt.getGlobalBounds().width/2;
	TempusImperium::promptTitle->txt.setPosition(localhalfWidth + rightSide + 50,-238);
}

void TempusImperium::deletePromptTitle(){
	TempusImperium::deleteTitleFromDrawPool(TempusImperium::promptTitle);
}

bool TempusImperium::isCurrentClusterContainsResources(){
	for(int i = 0;i < TempusImperium::currentRoadCluster->clusterRoads.size();i++){
		for(int ii = 0;ii < TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells.size();ii++){
			
			if(TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite != nullptr){
				Building* tmpBuild = TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite;
				if(tmpBuild->buildingType == BuildingType::Tree|| tmpBuild->buildingType == BuildingType::Rock ){
					return true;
				}
			}
		}
	}
	return false;
}

//полностью наебнулась система активации зданий
void TempusImperium::initCollectResources(){
	TempusImperium::createPrompt("Select resources to collect",24);
	
	for(int i = 0;i < TempusImperium::currentRoadCluster->clusterRoads.size();i++){
		for(int ii = 0;ii < TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells.size();ii++){
			
			if(TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite != nullptr){
				Building* tmpBuild = TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite;
				if(tmpBuild->buildingType == BuildingType::Tree && tmpBuild->treeCount > 0){
					//tmpBuild->updateVector.push_back(&Building::decreaseTreeCount);
					tmpBuild->updateMethod = &Building::decreaseTreeCount;
				}
				else if(tmpBuild->buildingType == BuildingType::Rock && tmpBuild->rockCount > 0){
					//tmpBuild->updateVector.push_back(&Building::decreaseRockCount);
					tmpBuild->updateMethod = &Building::decreaseRockCount;
				}
			}
		}
	}

	TempusImperium::endCollectResources = new Button("Done",32,250,200,240,100);
	TempusImperium::endCollectResources->setMethod(&Button::endCollectResources);
}

void TempusImperium::initBuildTurnAvailableBuildings(std::vector<BuildingType> buildingTyppeVector){
	TempusImperium::deleteButtonFromPool(TempusImperium::endCollectResources);
	TempusImperium::createPrompt("Select building to build",24);

	int yNextPos = -150;
	int yStep = 100;

	TempusImperium::deleteButtonFromPool(TempusImperium::buildFarmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildMarketButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildFortmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildCastleButton);

	int buttonH = 90;

	for(int i = 0;i < buildingTyppeVector.size();i++){
		if(buildingTyppeVector[i] == BuildingType::Farm){
			TempusImperium::buildFarmButton = new Button("Farm",25,250,yNextPos,240,buttonH);
			TempusImperium::buildFarmButton->setMethod(&Button::buildFarm);
			yNextPos += yStep;
		}
		if(buildingTyppeVector[i] == BuildingType::Market){
			TempusImperium::buildMarketButton = new Button("Market",25,250,yNextPos,240,buttonH);
			TempusImperium::buildMarketButton->setMethod(&Button::buildMarket);
			yNextPos += yStep;
		}
		if(buildingTyppeVector[i] == BuildingType::Fort){
			TempusImperium::buildFortmButton = new Button("Fort",25,250,yNextPos,240,buttonH);
			TempusImperium::buildFortmButton->setMethod(&Button::buildFort);
			yNextPos += yStep;
		}
		if(buildingTyppeVector[i] == BuildingType::Castle){
			TempusImperium::buildCastleButton = new Button("Castle",25,250,yNextPos,240,buttonH);
			TempusImperium::buildCastleButton->setMethod(&Button::buildCastle);
			yNextPos += yStep;
		}
	}
	//вот здесь инициализируем кнопки UI с предложением выбрать
	//необходимое строение для постройик/
	//подписать строения на выборв 
}

//make lake and longest road
void TempusImperium::calculateFinalScore(){
	int score = 0;
	std::cout << "Castle : " << TempusImperium::castleVector.size() << "\t total = " << 10 * TempusImperium::castleVector.size() <<std::endl;
	score += 10 * TempusImperium::castleVector.size();

	std::cout << "Markets : " << TempusImperium::marketVector.size() << "\t total = " << 5 * TempusImperium::marketVector.size() <<std::endl;
	score += 5 * TempusImperium::marketVector.size();

	std::cout << "Forts : " << TempusImperium::fortVector.size() << "\t total = " << 5 * TempusImperium::fortVector.size() <<std::endl;
	score += 5 * TempusImperium::fortVector.size();	

	std::cout << "Farms : " << TempusImperium::farmVector.size() << "\t total = " << 2 * TempusImperium::farmVector.size() <<std::endl;
	score += 2 * TempusImperium::farmVector.size();

	std::cout << "Gold : " << TempusImperium::goldCount << "\t total = " << TempusImperium::goldCount % 5 << std::endl;
	score += TempusImperium::goldCount % 5;

	int livesEnemies = 0;

	for(Building* pLiveEnemy : TempusImperium::enemyVector){
		if(!pLiveEnemy->isKilled){
			livesEnemies++;
		}
	}

	std::cout << "Lives enemies : " << livesEnemies << "\t total = " << -10 * livesEnemies <<std::endl;
	score += -10 * livesEnemies;

	for(Road* pRoad : TempusImperium::roadVector){
		pRoad->isVisited = false;
		pRoad->prevsRoad = nullptr;
		pRoad->roadLength = 1;
	}
	TempusImperium::calculateLongestPath();
	
	//std::cout << "Count of road cluster longest roads = " << Road::longestRoadClusters.size() << std::endl;
	int max = 0;
	int longestRoadClusterIndex = 0;
	for(int i = 0; i < Road::longestRoadClusters.size() ; i++ ){ //for test.delete later
		std::cout << Road::longestRoadClusters[i].size() << std::endl;
		if(max < Road::longestRoadClusters[i].size()){
			longestRoadClusterIndex = i;
			max = Road::longestRoadClusters[i].size();
		}
	}
	for(Road* pRoad : Road::longestRoadClusters[longestRoadClusterIndex]){
		pRoad->setFillColor(sf::Color::Green);
	}
	std::cout << "Longest road : " << Road::longestRoadClusters[longestRoadClusterIndex].size() << std::endl;
	score += Road::longestRoadClusters[longestRoadClusterIndex].size();

	std::cout << "FINAL : " << score << std::endl;
}

void TempusImperium::calculateLongestPath(){
	
	std::vector<Road*> firstRoads;
	
	for(Road* pRoad : TempusImperium::roadVector){
		pRoad->isVisited = false; //clear roads
		pRoad->prevsRoad = nullptr; //clear roads
		if(pRoad->isBuilded){
			
			int buildedAdjucentRoads = 0;
			
			for(Road* pAdjucentRoad : pRoad->adjacentRoads){
				if(pAdjucentRoad->isBuilded){
					buildedAdjucentRoads++;
				}
			}

			if(buildedAdjucentRoads == 1){
				firstRoads.push_back(pRoad);
			}
		}
	}

	for(Road* pRoad : firstRoads){
		Road::longestRoadClusters.push_back(std::vector<Road*>());
		pRoad->findLongestPath();
		for(Road* ppRoad : TempusImperium::roadVector){
			ppRoad->prevsRoad = nullptr;
			ppRoad->isVisited = false;
		}
	}
}