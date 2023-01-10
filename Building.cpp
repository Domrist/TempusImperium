#include "Building.h"
#include "Pool.h"
#include "TempusImperium.h"

#include <iostream>

std::string Building::getBuildingType(){
	switch(this->buildingType){
		case BuildingType::Tree:
				return "Building type - resource tree";
			break;
		case BuildingType::Rock :
				return "Building type - resource rock";
			break;
		case BuildingType::Farm:
				return "Building type - farm";
			break;
		case BuildingType::Market :
				return "Building type - market";
			break;
		case BuildingType::Fort :
				return "Building type - fort";
			break;
		case BuildingType::Castle :
				return "Building type - castle";
			break;
		case BuildingType::Enemy :
				return "Building type - enemy";
			break;
		case BuildingType::Lake :
				return "Building type - lake";
			break;
	}
	return "";
}

Building::Building(BuildingType buildingType){
	
	this->buildingType = buildingType;
	this->updateMethod = nullptr;	
	this->treeCount = -1;
	this->rockCount = -1;
	this->parentCell = nullptr;
	this->isKilled = false;
	this->isUsed = false;
	this->isCounted = false;

	switch(buildingType){

		case BuildingType::Tree:
				this->treeCount = 4;
				this->setTxt("Images/Trees/Tree4.png",sf::IntRect(0,0,128,128));
				TempusImperium::treeVector.push_back(this);
			break;
		case BuildingType::Rock:
				this->rockCount = 4;
				this->setTxt("Images/Rocks/stone4.png",sf::IntRect(0,0,128,128));
				TempusImperium::rockVector.push_back(this);
			break;
		case BuildingType::Farm:
				this->setTxt("Images/Buildings/farm.png",sf::IntRect(0,0,128,128));
				TempusImperium::farmVector.push_back(this);
			break;
		case BuildingType::Market:
				this->setTxt("Images/Buildings/market.png",sf::IntRect(0,0,128,128));
				TempusImperium::marketVector.push_back(this);
			break;
		case BuildingType::Fort:
				this->setTxt("Images/Buildings/fort.png",sf::IntRect(0,0,128,128));
				TempusImperium::fortVector.push_back(this);
			break;
		case BuildingType::Castle:
				this->setTxt("Images/Buildings/castle.png",sf::IntRect(0,0,128,128));
				TempusImperium::castleVector.push_back(this);
			break;
		case BuildingType::Enemy:
				this->setTxt("Images/Buildings/enemy.png",sf::IntRect(0,0,128,128));
				TempusImperium::enemyVector.push_back(this);
			break;
		case BuildingType::Lake:
				this->setSize(sf::Vector2f(33,33));
				this->setOrigin(16,16);
				this->setFillColor(sf::Color(0,0,255));
			break;
		}
		this->setTexture(&(this->texture));
		this->setSize(sf::Vector2f(33,33));	
		this->setOrigin(16,16); //center of GameObject

		/*Pool::Pool::addToPool(this);*/
}

Building::~Building(){

}

void Building::activateFarm(){
	if(!this->isUsed){
		//обращаемся к родительской клетке
		//у родительской клетки начинаем искать непосещённые дороги
		//если среди соседних клеток у дороги есть ферма - приплюсовываем 1 золото
		this->isUsed = true;
		this->roundUsed = TempusImperium::currentRound;
		this->updateMethod = nullptr;
		TempusImperium::goldCount++;
		TempusImperium::deactivateAllBuildings();
		TempusImperium::deletePromptTitle();
		TempusImperium::initGameUI();
		TempusImperium::goToNextTurn();
	}
}

//+ голда за каждый форт и маркет на присоединённой дороге
void Building::activateCastle(){

	if(this->isUsed){
		return;
	}
	this->isUsed = true;

	//вот эти два цикла потом перезаписать в метод вызываемый в конце каждого хода
	for(Building* pFort : TempusImperium::fortVector){
		pFort->isCounted = false;
	}
	for(Building* pMarket : TempusImperium::marketVector){
		pMarket->isCounted = false;
	}
	//added loop below for test
	for(Road* pRoad : TempusImperium::roadVector){ //вот этуу херню запихать в переключение после каждого хода
		pRoad->isVisited = false;
		pRoad->prevsRoad = nullptr;
	}

	Cell* pCell = this->parentCell;
	//начинаем идти по этим дорогам и ищем ферму
	for(Road* currentRoad : pCell->adjacentRoads){
		//std::cout << currentRoad->isBuilded << "\t" << currentRoad->isVisited << std::endl;
		if(currentRoad->isBuilded && !currentRoad->isVisited){
			currentRoad->findMarketsAndForts();
		}
	}

	TempusImperium::deletePromptTitle();
	TempusImperium::initGameUI();
	TempusImperium::goToNextTurn();
	TempusImperium::deactivateAllBuildings();
}

//алгоритм для поиска всех подсоединённых дорогой ферм и начисления золота за каждую ферму
void Building::activateMarket(){
		if(this->isUsed){
			return;
		}

		//на всякий случай чистим все дороги
		for(Road* pRoad : TempusImperium::roadVector){ //вот этуу херню запихать в переключение после каждого хода
			pRoad->isVisited = false;
			pRoad->prevsRoad = nullptr;
		}

		for(Building* pFarm : TempusImperium::farmVector){
			pFarm->isCounted = false;
		}

		this->isUsed = true;
		//отписываем все строения от метода обновления - ,долбаёб, не забудь сюда дописать метод
		
		
		//обращаемся к блиайшим дорогам через родительскую клетку
		Cell* pCell = this->parentCell;
		//начинаем идти по этим дорогам и ищем ферму
		for(Road* currentRoad : pCell->adjacentRoads){
			//std::cout << currentRoad->isBuilded << "\t" << currentRoad->isVisited << std::endl;
			if(currentRoad->isBuilded && !currentRoad->isVisited){
				currentRoad->findFarm();
			}
		}

		TempusImperium::deletePromptTitle();
		TempusImperium::initGameUI();
		TempusImperium::goToNextTurn();
		TempusImperium::deactivateAllBuildings();
}

//остановился здесь
void Building::activateFort(){

	if(this->isUsed){
		return;
	}
	
	TempusImperium::deactivateAllBuildings(); // вот тут нужно очищать все строения
	TempusImperium::deletePromptTitle();
	TempusImperium::createPrompt("Select enemy to kill",29);
	//на всякий случай чистим все дороги
	for(Road* pRoad : TempusImperium::roadVector){ //вот этуу херню запихать в переключение после каждого хода
		pRoad->isVisited = false;
		pRoad->prevsRoad = nullptr;
	}

	this->isUsed = true;

	//обращаемся к блиайшим дорогам через родительскую клетку
	Cell* pCell = this->parentCell;
	//начинаем идти по этим дорогам и ищем врагов
	for(Road* currentRoad : pCell->adjacentRoads){

		if(currentRoad->isBuilded && !currentRoad->isVisited){
			currentRoad->findEnemy();
		}
	}
}

void Building::updateStatus(){
	
	if(this->updateMethod != nullptr && this->isClicked()){
		(this->*updateMethod)();
	}
}

void Building::decreaseTreeCount(){

	if(this->treeCount <= 0 ){
		return;
	}
	
	this->treeCount--;
	//append to cluster balance
	switch(treeCount){
		case 3:
				setTxt("Images/Trees/Tree3.png",sf::IntRect(0,0,128,128));
			break;
		case 2:
				setTxt("Images/Trees/Tree2.png",sf::IntRect(0,0,128,128));
			break;
		case 1:
				setTxt("Images/Trees/Tree1.png",sf::IntRect(0,0,128,128));
			break;
		case 0:
				setTxt("Images/Trees/Tree0.png",sf::IntRect(0,0,128,128));
			break;
	}
	TempusImperium::currentRoadCluster->treesCount++;
}

void Building::decreaseRockCount(){
	if(this->rockCount<=0){
		return;
	}
	
	this->rockCount--;
	//append to cluster balance
	switch(rockCount){
		case 3:
			setTxt("Images/Rocks/stone3.png",sf::IntRect(0,0,128,128));
			break;
		case 2:
			setTxt("Images/Rocks/stone2.png",sf::IntRect(0,0,128,128));
			break;
		case 1:
			setTxt("Images/Rocks/stone1.png",sf::IntRect(0,0,128,128));
			break;
		case 0:
			setTxt("Images/Rocks/stone0.png",sf::IntRect(0,0,128,128));
			break;
	}
	TempusImperium::currentRoadCluster->rocksCount++;
}

void Building::killEnemy(){
	if(this->isKilled){
		return;
	}
	this->isKilled = true;
	for(Building* pEnemy : TempusImperium::enemyVector){
		pEnemy->updateMethod = nullptr;
	}
	this->setTxt("Images/Buildings/defeatedEnemy.png",sf::IntRect(0,0,128,128));	
	TempusImperium::goldCount++;
	TempusImperium::deletePromptTitle();
	TempusImperium::initGameUI();
	TempusImperium::goToNextTurn();
	TempusImperium::deactivateAllBuildings();
}