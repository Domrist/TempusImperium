#include "Tile.h"
#include "Pool.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdio.h>
#include "GameObject.h"
#include "TempusImperium.h"
#include "Grid.h"
#include <cmath>
#include "RoadCluster.h"

Road* Road::longestRoad = nullptr;
Road* Road::beginLongestRoad = nullptr;
std::vector<std::vector<Road*>> Road::longestRoadClusters;

Road::Road(float x,float y,bool horVer){
	this->isBuilded = false;
	this->isVisited = false;
	this->prevsRoad = nullptr;
	this->roadLength = 1;
	
	if(!horVer){
		this->setSize(sf::Vector2f(15,33));//ширина и высота
		this->setOrigin(8,16);
		this->isHorizontal = false;
	}
	else{
		this->setSize(sf::Vector2f(33,15));//ширина и высота
		this->setOrigin(16,8);
		this->isHorizontal = true;
	}
	this->setFillColor(sf::Color(59,20,20,50));
	this->setPosition(x,y);
	Pool::Pool::addToPool(this);
}

bool Road::isRoadAdjacent(Road* nextRoad){
	//говно - да
	if(this->isHorizontal && !nextRoad->isHorizontal)
	{
		if(this->colIndex == nextRoad->colIndex && this->rowIndex == nextRoad->rowIndex){
			return true;
		}
		else if(this->colIndex == nextRoad->colIndex && this->rowIndex-1 == nextRoad->rowIndex ){
			return true;
		}
		else if(this->colIndex+1 == nextRoad->colIndex && this->rowIndex-1 == nextRoad->rowIndex){
			return true;
		}
		else if(this->colIndex+1 == nextRoad->colIndex && this->rowIndex == nextRoad->rowIndex ){
			return true;
		}
	}
	else if(this->isHorizontal && nextRoad->isHorizontal){
		if((this->colIndex+1 == nextRoad->colIndex||this->colIndex-1 == nextRoad->colIndex) && this->rowIndex == nextRoad->rowIndex){
			return true;
		}
	}

	if(!this->isHorizontal && nextRoad->isHorizontal){
		if(this->colIndex-1==nextRoad->colIndex&&this->rowIndex == nextRoad->rowIndex){
			return true;
		}
		else if(this->colIndex==nextRoad->colIndex&&this->rowIndex == nextRoad->rowIndex){
			return true;
		}
		else if(this->colIndex==nextRoad->colIndex&&this->rowIndex+1 == nextRoad->rowIndex){
			return true;
		}
		else if(this->colIndex-1==nextRoad->colIndex&&this->rowIndex+1 == nextRoad->rowIndex){
			return true;
		}
	}
	else if(!this->isHorizontal && !nextRoad->isHorizontal){
		if(this->colIndex==nextRoad->colIndex && (this->rowIndex-1==nextRoad->rowIndex||this->rowIndex+1==nextRoad->rowIndex)){
			return true;
		}
	}
	return false;
}

void Road::buildRoad(){
	if(this->isClicked() && !this->isBuilded){
		this->isBuilded = true;
		this->setFillColor(sf::Color(110, 113, 117));
		//go to next turn;
		for(auto road : TempusImperium::roadVector ){
			road->updateVector.clear();
		}
		TempusImperium::deletePromptTitle();
		TempusImperium::initGameUI();
		TempusImperium::goToNextTurn();
		return; //нахуя, аглавное зачем?
	}
}
void Road::updateStatus(){ //это 
	for(int i = 0;i < this->updateVector.size();i++){
		(this->*updateVector[i])();
	}
}
void Road::selectCluster(){

	TempusImperium::deletePromptTitle();

	if(this->isClicked()){
		for(int i = 0;i < RoadCluster::roadClusterVector.size();i++){
			for(int ii = 0;ii < RoadCluster::roadClusterVector[i]->clusterRoads.size();ii++){
				if(RoadCluster::roadClusterVector[i]->clusterRoads[ii] == this){
					TempusImperium::currentRoadCluster = RoadCluster::roadClusterVector[i];

					//после того как получили актуальный кластер дороги:
					//проверяем кластер на наличие ресурсов

					//если ресурсы есть:
					if(TempusImperium::isCurrentClusterContainsResources()){ 
						//очищаем все дороги от методов
						//очищаем все дороги до своих стандартных цветов
						TempusImperium::deactivateAllRoads();
						//скрываем ui
						TempusImperium::hideCurrentTurnUI();
						
						//предлагаем потратить ресурсы, которые находятся рядом с дорогой
						TempusImperium::initCollectResources();
						return;
					}
					else{ //если ресурсов нет - идём дальше
					}
				}
			}
		}
	}
}

void Road::lookForNearesResources(){
	if(!this->isVisited){
		RoadCluster::addRoadToCluster(this);
	}
	this->isVisited = true;

	//проходимся по соседним клеткам. Если клетки без ресурсов - то meh
	for(int i = 0;i < this->adjacentCells.size();i++){
		if(this->adjacentCells[i]->cellSprite != nullptr){
			Building* tmpBuild = this->adjacentCells[i]->cellSprite; //вот эту херню можно поменять потом на метод поиска ресурсов на соседних клетках от дороги(метод уже написан)
			if(tmpBuild->buildingType ==  BuildingType::Tree || tmpBuild->buildingType ==  BuildingType::Rock){
				this->setFillColor(sf::Color::Green);
			}
		}
	}

	int countOfNonVisitedRoads = 0;
	
	for(int i = 0;i < this->adjacentRoads.size();i++){
		if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
			countOfNonVisitedRoads++;
		}
	}
	if(countOfNonVisitedRoads == 0 && this->prevsRoad != nullptr){
		this->prevsRoad->lookForNearesResources();
	}
	if(countOfNonVisitedRoads == 0 && this->prevsRoad == nullptr){ //must be empty
	}
	if(countOfNonVisitedRoads > 0){
		for(int i = 0;i < this->adjacentRoads.size();i++){
			if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
				this->adjacentRoads[i]->prevsRoad = this;
				this->adjacentRoads[i]->lookForNearesResources();
				break;
			}
		}
	}
}

int Road::isResourceAdjacent(BuildingType type){
	int countOfAdjacentResources = 0;
	for(Cell* tmpCell : this->adjacentCells){
		if(tmpCell->cellSprite != nullptr){
			if(tmpCell->cellSprite->buildingType == type){
				countOfAdjacentResources+=1;
			}
		}
	}
	return countOfAdjacentResources;
}

//потом сделать контролльную проверкку этого метода
void Road::findFarm(){

	//если мы не посещали эту дорогу
	if(!this->isVisited){
		this->isVisited = true;
		
		int countOfAdjacentFarms = 0;
		for(Cell* pCell : this->adjacentCells){
			if(pCell->cellSprite != nullptr){
				
				if(pCell->cellSprite->buildingType == BuildingType::Farm && !pCell->cellSprite->isCounted){
					pCell->cellSprite->isCounted = true;
					TempusImperium::goldCount++;
				}	
			}
		}
	}

	int countOfNonVisitedRoads = 0;
	
	//получаем количество доступных дорог рядом
	for(int i = 0;i < this->adjacentRoads.size();i++){
		if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
			countOfNonVisitedRoads++;
		}
	}
	
	//если есть доступные дороги
	if(countOfNonVisitedRoads > 0){
		for(int i = 0;i < this->adjacentRoads.size();i++){
			if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
				this->adjacentRoads[i]->prevsRoad = this;
				this->adjacentRoads[i]->findFarm();
				return;
			}
		}
	}

	//если доступных дорог нет и есть предыдущая дорога - шагаем назад
	else if(countOfNonVisitedRoads == 0 && this->prevsRoad != nullptr){
		this->prevsRoad->findFarm();
		return;
	}

	//если доступных дорог нету и предыдущей дороги нету - то заканчиваем шагать в поисках фермы
	else if(countOfNonVisitedRoads == 0 && this->prevsRoad == nullptr){ //must be empty
	}
}

void Road::findEnemy(){
	if(!this->isVisited){
		this->isVisited = true;
		
		int countOfAdjacentFarms = 0;
		for(Cell* pCell : this->adjacentCells){
			if(pCell->cellSprite != nullptr){
				
				if(pCell->cellSprite->buildingType == BuildingType::Enemy){
					pCell->cellSprite->updateMethod = &Building::killEnemy;
				}	
			}
		}
	}

	int countOfNonVisitedRoads = 0;
	
	//получаем количество доступных дорог рядом
	for(int i = 0;i < this->adjacentRoads.size();i++){
		if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
			countOfNonVisitedRoads++;
		}
	}
	
	//если есть доступные дороги
	if(countOfNonVisitedRoads > 0){
		for(int i = 0;i < this->adjacentRoads.size();i++){
			if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
				this->adjacentRoads[i]->prevsRoad = this;
				this->adjacentRoads[i]->findEnemy();
				return;
			}
		}
	}

	//если доступных дорог нет и есть предыдущая дорога - шагаем назад
	else if(countOfNonVisitedRoads == 0 && this->prevsRoad != nullptr){
		this->prevsRoad->findEnemy();
		return;
	}

	//если доступных дорог нету и предыдущей дороги нету - то заканчиваем шагать в поисках фермы
	else if(countOfNonVisitedRoads == 0 && this->prevsRoad == nullptr){
	}	
}

void Road::findMarketsAndForts(){
	if(!this->isVisited){
		
		this->isVisited = true;
		
		int countOfAdjacentFarms = 0;
		
		for(Cell* pCell : this->adjacentCells){
			
			if(pCell->cellSprite != nullptr){
				
				BuildingType pType = pCell->cellSprite->buildingType;
				
				if((pType == BuildingType::Market || pType == BuildingType::Fort ) && !pCell->cellSprite->isCounted){
					
					pCell->cellSprite->isCounted = true;
					TempusImperium::goldCount+=2;
				}	
			}
		}
	}

	int countOfNonVisitedRoads = 0;
	
	//получаем количество доступных дорог рядом
	for(int i = 0;i < this->adjacentRoads.size();i++){
		if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
			countOfNonVisitedRoads++;
		}
	}
	
	//если есть доступные дороги
	if(countOfNonVisitedRoads > 0){
		for(int i = 0;i < this->adjacentRoads.size();i++){
			if(this->adjacentRoads[i]->isBuilded && !this->adjacentRoads[i]->isVisited){
				this->adjacentRoads[i]->prevsRoad = this;
				this->adjacentRoads[i]->findMarketsAndForts();
				return;
			}
		}
	}

	//если доступных дорог нет и есть предыдущая дорога - шагаем назад
	else if(countOfNonVisitedRoads == 0 && this->prevsRoad != nullptr){
		this->prevsRoad->findMarketsAndForts();
		return;
	}

	//если доступных дорог нету и предыдущей дороги нету - то заканчиваем шагать в поисках фермы
	else if(countOfNonVisitedRoads == 0 && this->prevsRoad == nullptr){
		return;
	}
}

void Road::findLongestPath(){

	if(!this->isVisited){
		
		this->isVisited = true;
		
		if(this->prevsRoad != nullptr){ //если дорога имеет ссылку на предыдущий участок
			this->roadLength += this->prevsRoad->roadLength;
		}
	}

	int countOfAdjacentsRoads = 0;
	
	//получаем список доступных непосещённых дорог
	for(Road* pRoad : this->adjacentRoads){
		if(!pRoad->isVisited && pRoad->isBuilded){
			countOfAdjacentsRoads++;
			pRoad->prevsRoad = this;
		}
	}

	//если количество доступных дорог нет
	if(countOfAdjacentsRoads == 0 && this->prevsRoad != nullptr){
		if(Road::longestRoad == nullptr){ //если длиннейшая дорога пустая - записываем туда эту дорогу
			Road::longestRoad = this;
		}
		if(Road::longestRoad != nullptr){ //если длиннейшая дорога не пустая - проверяем на длину с текущей дорогой
			if(Road::longestRoad->roadLength < this->roadLength){
				Road::longestRoad = this;
			}
		}
		this->prevsRoad->findLongestPath();

	}
	else if(countOfAdjacentsRoads == 0 && this->prevsRoad == nullptr){ //если закончили проходиться по всем дорогам, то начинаем заполнять последний кластер длиннейших дорог 
		Road::longestRoad->roadLength = 0;
		Road::longestRoad->highlightLongestRoad();
		Road::longestRoad->setFillColor(sf::Color::Green);
		return;
	}
	
	else if(countOfAdjacentsRoads > 0){
		for(Road* pRoad : this->adjacentRoads){
			if(pRoad->isBuilded && !pRoad->isVisited){
				pRoad->findLongestPath();
			}
		}
	}
}
void Road::highlightLongestRoad(){
	this->setFillColor(sf::Color::White);
	Road::longestRoadClusters[Road::longestRoadClusters.size()-1].push_back(this);
	Road::longestRoad->roadLength++;
	if(this->prevsRoad != nullptr){
		this->prevsRoad->highlightLongestRoad();
		return;
	}
	else{
		Road::longestRoad->roadLength--;
		this->setFillColor(sf::Color::Cyan);
		return;
	}
}