#include "UiElements.h"
#include "Pool.h"
#include <SFML/Graphics.hpp>
#include "Tile.h"
#include <iostream>
#include "TempusImperium.h"
#include "RoadCluster.h"

Title::Title(std::string textValue,int fontSize,sf::Color textColor){
	this->font.loadFromFile("Fonts/arialmt.ttf");//arial as default
	this->txt.setFont(this->font); 
	this->txt.setString(textValue);
	this->txt.setCharacterSize(fontSize);
	this->txt.setFillColor(textColor);
	this->txt.setOrigin(this->txt.getGlobalBounds().width/2,this->txt.getGlobalBounds().height/2);
	Pool::Pool::addToDrawPool(this);
}

Title::~Title(){}

void Title::draw(sf::RenderTarget &target, sf::RenderStates states) const{
	target.draw(this->txt,states);
}

void Button::updateStatus(){
	if(this->isClicked()){
		(this->*clickMethod)();
	}
}

//BUTTON SECTION
Button::~Button(){}

Button::Button(std::string buttonnString,int fontSize,int x,int y,int buttonWidth,int buttonHeight,bool isAdditional){
	this->isAdditional = isAdditional;
	//int rect background
	this->setSize(sf::Vector2f(buttonWidth,buttonHeight));
	this->setOrigin(buttonWidth/2,buttonHeight/2);
	this->setFillColor(sf::Color::White);
	//text block
	this->buttonTextFont.loadFromFile("Fonts/arialmt.ttf");//arial as default
	this->buttonText.setFont(this->buttonTextFont);
	this->buttonText.setString(buttonnString);
	this->buttonText.setFillColor(sf::Color::Red);
	this->buttonText.setCharacterSize(fontSize);
	this->buttonText.setOrigin(this->buttonText.getGlobalBounds().width/2,this->buttonText.getGlobalBounds().height/2);
	this->setPosition(x,y);
	this->buttonText.setPosition(x,y);
	Pool::Pool::addToPool(this);
	Pool::Pool::addToDrawPool(&(this->buttonText));
}

void Button::setMethod(void (Button::*pMethod)()){
	this->clickMethod = pMethod;
}

void Button::setButtonPosition(int x,int y){
	this->setPosition(x,y);
	this->buttonText.setPosition(x,y);
}

void Button::skipTurn(){
	
	TempusImperium::goToNextTurn();
}

void Button::buildTurn(){

	if(this->isAdditional && TempusImperium::goldCount < 2){
		return;
	}
	else if(this->isAdditional && TempusImperium::goldCount >1){
		TempusImperium::goldCount -=2;
	}

	TempusImperium::hideCurrentTurnUI();
	TempusImperium::createPrompt("Select road,where you will build",20);

	for(Road* pRoad : TempusImperium::roadVector){
		pRoad->isVisited = false;
		pRoad->prevsRoad = nullptr;//for test(?)
	}

	RoadCluster::initVectorClusters();
	//проходимся по каждой дороге
	for(auto currentRoad : TempusImperium::roadVector){
		//проверяем, построена ли дорога
		if(currentRoad->isBuilded && !currentRoad->isVisited){
			currentRoad->lookForNearesResources();			
			RoadCluster::createNewCluster();
		}
	}
	//for test block
	//скрываем основной UI
	//предлагаем выбрать дорогу вокруг которой будем строиться
	//подсвечиваем все кластеры кроме последнего, так как последний - пустой
	for(int i = 0;i < RoadCluster::roadClusterVector.size()-1;i++){
		for(int ii = 0;ii <RoadCluster::roadClusterVector[i]->clusterRoads.size();ii++){
			//подписываем каждую дорогу на событие при нажатии - выбрать кластер
			RoadCluster::roadClusterVector[i]->clusterRoads[ii]->setFillColor(sf::Color::Cyan);
			RoadCluster::roadClusterVector[i]->clusterRoads[ii]->updateVector.push_back(&Road::selectCluster);
		}
	}
}

//buttons method block
void Button::digTurn(){
	if(this->isAdditional && TempusImperium::goldCount < 2){
		return;
	}
	else if(this->isAdditional && TempusImperium::goldCount >1){
		TempusImperium::goldCount -=2;
	}
	TempusImperium::createPrompt("Select cell to dig",13);
	for(GameObject* tmpLake : TempusImperium::lakeVector){
		
		Cell* pCell = (Cell*)tmpLake->parentObject;
		
		for(int i = 0;i < pCell->adjacentCells.size();i++){
			if(pCell->adjacentCells[i]->cellSprite == nullptr){
				pCell->adjacentCells[i]->setDig();
			}
		}
	}
	TempusImperium::hideCurrentTurnUI();
}

void Button::buildRoadTurn(){

	if(this->isAdditional && TempusImperium::goldCount < 2){
		return;
	}
	else if(this->isAdditional && TempusImperium::goldCount >1){
		TempusImperium::goldCount -=2;
	}
	TempusImperium::createPrompt("Select road to build",24);
	for(auto road : TempusImperium::roadVector ){
		road->updateVector.push_back(&Road::buildRoad);
	}
	// объёбка с пропаданием элементов из пула
	//в тайле дороги после нажатия делать переицициализацию элементов
	TempusImperium::hideCurrentTurnUI();
	
}

void Button::activateTurn(){
	
	if(this->isAdditional && TempusImperium::goldCount < 2){
		return;
	}
	else if(this->isAdditional && TempusImperium::goldCount >1){
		TempusImperium::goldCount -=2;
	}
	TempusImperium::createPrompt("Select building to activate",24);
	for(Building* farm : TempusImperium::farmVector){
		farm->updateMethod = &Building::activateFarm;
	}
	for(Building* market : TempusImperium::marketVector){
		market->updateMethod = &Building::activateMarket;
	}
	for(Building* fort : TempusImperium::fortVector){
		//fort->updateVector.push_back(&Building::activateFort);
		fort->updateMethod = &Building::activateFort;
	}
	for(Building* castle : TempusImperium::castleVector){
		castle->updateVector.push_back(&Building::activateCastle); //потом заменить на установку метода не через добавление вектор методов
		castle->updateMethod = &Building::activateCastle;
	}

	TempusImperium::hideCurrentTurnUI();
}
//end buttons method block
//0 - лево, 1 - верх, 2 - право, 3 - низ
int Button::getBorderPosition(int direction){
	switch(direction){
		case 0:
			return this->getPosition().x - this->getGlobalBounds().width/2;
			break;
		case 1:
			return this->getPosition().y + this->getGlobalBounds().height/2;
			break;
		case 2:
			return this->getPosition().x + this->getGlobalBounds().width/2;
			break;
		case 3: 
			return this->getPosition().y - this->getGlobalBounds().height/2;
			break; 
	}
	return 0;
}

//0 - лево, 1 - верх, 2 - право, 3 - низ
void Button::setAdjacentButton(Button* pButton,int direction,int shift = 0){
	
	float pButtonShiftX = pButton->getGlobalBounds().width/2;
	float pButtonShiftY = pButton->getGlobalBounds().height/2;

	int thisX = this->getPosition().x;
	int thisY = this->getPosition().y;

	switch(direction){
		case 0:
				pButton->setButtonPosition(this->getBorderPosition(direction)-pButtonShiftX - shift,thisY);
			break;
		case 1:
				pButton->setButtonPosition(thisX,this->getBorderPosition(direction) - pButtonShiftY - shift);
			break;
		case 2:
				pButton->setButtonPosition(this->getBorderPosition(direction)+pButtonShiftX + shift,thisY);
			break;
		case 3:
				pButton->setButtonPosition(thisX,this->getBorderPosition(direction) + pButtonShiftY + shift);
			break;
	}
}

void Button::endCollectResources(){
	//отписываем все ресурсы от траты

	//в зависимости от собранных ресурсов,
	//показываем кнопки с 
	TempusImperium::deletePromptTitle();
	
	int tmpRockCount = TempusImperium::currentRoadCluster->rocksCount;
	int tmpTreeCount = TempusImperium::currentRoadCluster->treesCount;
	
	std::vector<BuildingType> tmpAvailableBuilding;
	//пока для теста всё делаем бесплатным
	//потом попозже, ближе под релиз сделать там где требуются монеты
	//добавить траты монет
	if(tmpTreeCount >= 1){ 
		tmpAvailableBuilding.push_back(BuildingType::Farm);
	}
	if(tmpTreeCount > 1 && TempusImperium::goldCount >=2 ){ 
		tmpAvailableBuilding.push_back(BuildingType::Market);
	}
	if(tmpTreeCount >=1 && tmpRockCount >= 1 && TempusImperium::goldCount > 2){
		tmpAvailableBuilding.push_back(BuildingType::Fort);	
	}
	if(tmpRockCount > 1 && TempusImperium::goldCount > 2){
		tmpAvailableBuilding.push_back(BuildingType::Castle);	
	}

	for(int i = 0;i < TempusImperium::treeVector.size();i++){
		TempusImperium::treeVector[i]->updateMethod = nullptr;
	}
	for(int i = 0;i < TempusImperium::rockVector.size();i++){
		TempusImperium::rockVector[i]->updateMethod = nullptr;
	}
	

	//в аргумент пихаем вектор строений, доступных для постройки
	TempusImperium::initBuildTurnAvailableBuildings(tmpAvailableBuilding);
}

void Button::buildFarm(){
	
	TempusImperium::tmpGameObject = new Building(BuildingType::Farm);
	TempusImperium::tmpGameObject->setPosition(1000,1000);
	TempusImperium::deletePromptTitle();
	TempusImperium::createPrompt("Select cell to build farm",22);
	
	TempusImperium::deleteButtonFromPool(TempusImperium::buildFarmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildMarketButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildFortmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildCastleButton);

	for(int i = 0;i < TempusImperium::grid.size();i++ ){
		if(TempusImperium::grid[i]->cellSprite == nullptr){
			TempusImperium::grid[i]->setFillColor(sf::Color::Green);
			TempusImperium::grid[i]->methodVector.push_back(&Cell::setBuilding);
		}
	}
}

void Button::buildMarket(){

	TempusImperium::goldCount-=2;
	TempusImperium::tmpGameObject = new Building(BuildingType::Market);
	TempusImperium::tmpGameObject->setPosition(1000,1000);
	TempusImperium::deletePromptTitle();
	TempusImperium::createPrompt("Select cell to build market",22);

	TempusImperium::deleteButtonFromPool(TempusImperium::buildFarmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildMarketButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildFortmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildCastleButton);
	
	for(int i=0;i<TempusImperium::currentRoadCluster->clusterRoads.size();i++){
		for(int ii=0;ii<TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells.size();ii++){
			if(TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite == nullptr){
				TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->setFillColor(sf::Color::Green);
				TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->methodVector.push_back(&Cell::setBuilding);
			}
		}
	}
}

void Button::buildFort(){

	TempusImperium::goldCount-=2;
	TempusImperium::tmpGameObject = new Building(BuildingType::Fort);
	TempusImperium::tmpGameObject->setPosition(1000,1000);

	TempusImperium::deletePromptTitle();
	TempusImperium::createPrompt("Select cell to build fort",22);

	TempusImperium::deleteButtonFromPool(TempusImperium::buildFarmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildMarketButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildFortmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildCastleButton);

	for(int i=0;i<TempusImperium::currentRoadCluster->clusterRoads.size();i++){
		for(int ii=0;ii<TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells.size();ii++){
			if(TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite == nullptr){
				TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->setFillColor(sf::Color::Green);
				TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->methodVector.push_back(&Cell::setBuilding);
			}
		}
	}
}

void Button::buildCastle(){

	TempusImperium::goldCount-=2;
	TempusImperium::tmpGameObject = new Building(BuildingType::Castle);
	TempusImperium::tmpGameObject->setPosition(1000,1000);
	TempusImperium::deletePromptTitle();
	TempusImperium::createPrompt("Select cell to build castle",22);

	TempusImperium::deleteButtonFromPool(TempusImperium::buildFarmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildMarketButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildFortmButton);
	TempusImperium::deleteButtonFromPool(TempusImperium::buildCastleButton);

	for(int i=0;i<TempusImperium::currentRoadCluster->clusterRoads.size();i++){
		for(int ii=0;ii<TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells.size();ii++){
			if(TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->cellSprite == nullptr){
				TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->setFillColor(sf::Color::Green);
				TempusImperium::currentRoadCluster->clusterRoads[i]->adjacentCells[ii]->methodVector.push_back(&Cell::setBuilding);
			}
		}
	}
}