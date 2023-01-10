#ifndef TEMPUSIMPERIUM_H
#define TEMPUSIMPERIUM_H

#include "Grid.h"
#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <thread>

#include "UiElements.h"
#include "RoadCluster.h"

#include "Building.h"

enum ActionType{
	SkipTurn,
	Build,
	Activate,
	Dig,
	BuildRoad
};

class Background : public GameObject{
	public:
		void updateStatus();
		Background(std::string path,sf::IntRect rect = sf::IntRect(0,0,128,128));
};

class TempusImperium
{
	public:
		static void initGrid();
		static void initUserData();
		static void clearData();

		static int dateMas[10];

		static std::vector<Cell*> grid;
		
		static std::vector<Building*> enemyVector; //вот что за хуйня
		static std::vector<Building*> lakeVector;   //с этими двумя строчками?
		static std::vector<Road*> roadVector;
		static std::vector<Building*> farmVector;
		static std::vector<Building*> marketVector;
		static std::vector<Building*> fortVector;
		static std::vector<Building*> castleVector;
		static std::vector<Building*> treeVector;
		static std::vector<Building*> rockVector;
		
		static sf::Vector2f mousePos;

		static bool isNextTurn;

		static Background* backgroundGrid;
		static Building* tmpGameObject;
		static std::thread* myThread;
		static void getPlayersSetStartTileInput(std::vector<std::vector<Cell*>> &colRow,int f,int ff);

		static void setActivateAllBuildings();//потом удалить, запихнул метод в кнопку
		static void deactivateAllBuildings();

		//init data block
		static void initDateMas();
		
		static void setQueryAction(int dateQuery[10]);
		
		static void setAdjacentsCell();

		static void initHorizontalRoads();
		static void initVerticalRoads();
		static void setAdajcentsRoads();
		static void setAdjacentCellToRoads();

		static void initEnemy();
		//end init data block

		//game data
		static int currentRound;
		static int currentTurn;
		static int goldCount;
		static int currentQueryActionIndex;
		static std::vector<ActionType> queryAction;

		//ui block

		static void initGameUI();
		static void deinitialiseGameUI();
		
		static void hideCurrentTurnUI();
		static void deinitAdditionalActionButton();
		static void initAdditionalActionButton();
		static void updateUIStatus();

		static void createPrompt(std::string prompString,int fontSize);
		static void deletePromptTitle();

		static Title* promptTitle;
		
		static Title* currentRoundTitle;
		static Title* currentTurnTitle;
		static Title* currentMainActionTitle;
		static Title* currentGoldCountTitle;
		static Title* additionallMovesTitle;


		static void deleteButtonFromPool();
		static void deleteButtonFromPool(Button* &pButton);
		static void deleteTitleFromDrawPool(Title* &toDelete);
		//game control

		static Button* skipButton;
		static Button* activateButton;
		static Button* buildRoadButton;
		static Button* buildBuildingButton;
		static Button* digButton;
		static Button* currentActionButton;

		static Button* endCollectResources;

		static void deactivateAllRoads();
		static void deactivateAllCells();

		//---------------------------//
		static Button* buildFarmButton;
		static Button* buildMarketButton;
		static Button* buildFortmButton;
		static Button* buildCastleButton;

		static void buildFarm();
		static void buildMarket();
		static void buildFort();
		static void buildCastle();
		//---------------------------//

		//game logic
		static RoadCluster* currentRoadCluster;
		static bool isCurrentClusterContainsResources();
		static void goToNextTurn();
		static void initCollectResources();

		static void initBuildTurnAvailableBuildings(std::vector<BuildingType> buildingTyppeVector);
		static void acitvateEveryEnemy();

		static void calculateFinalScore();

		static void calculateLongestPath();
};

#endif