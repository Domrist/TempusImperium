#ifndef UIELEMENTS_H
#define UIELEMENTS_H

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include <iostream>


class Title : public sf::Drawable
{
	public:
		sf::Text txt;
		sf::Font font;
		Title(std::string textValue,int fontSize,sf::Color textColor);
		~Title();
		void draw (sf::RenderTarget &target, sf::RenderStates states) const;
};

class Button: public GameObject
{
	public:
		sf::Font buttonTextFont;
		sf::Text buttonText;
		
		void (Button::*clickMethod)();
		
		Button(std::string buttonnString,int fontSize,int x,int y,int buttonWidth,int buttonHeight,bool isAdditional = false);

		bool isAdditional;

		~Button();
		void updateStatus();
		void setButtonPosition(int x,int y);
		
		void skipTurn();
		void buildTurn();
		void activateTurn();
		void digTurn();
		void buildRoadTurn();
		void setMethod(void (Button::*pMethod)());

		//0 - лево, 1 - верх, 2 - право, 3 - низ
		int getBorderPosition(int direction);
		//0 - лево, 1 - верх, 2 - право, 3 - низ
		void setAdjacentButton(Button* pButton,int direction,int shift);

		void endCollectResources();

		void buildFarm();
		void buildMarket();
		void buildFort();
		void buildCastle();
};

#endif