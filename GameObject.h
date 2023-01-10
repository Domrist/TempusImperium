#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>

class GameObject : public sf::RectangleShape
{
	public:
		GameObject* parentObject;
		GameObject* childObject;
		GameObject(std::string path,sf::IntRect rect);		
		GameObject(std::string path);
		GameObject();
		~GameObject();
		void setTxt(std::string path,sf::IntRect rect);
		void setPos(float x,float y);
		sf::Texture texture;
		virtual void updateStatus() =0;
		bool isClicked();
};

#endif