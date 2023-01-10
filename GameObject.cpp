#include "GameObject.h"
#include <stdio.h>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Pool.h"
#include "TempusImperium.h"

void GameObject::setTxt(std::string path,sf::IntRect rect){
	if(this->texture.loadFromFile(path,rect)){
	}
	else{
		std::cout << "Err load texture" << std::endl;
	}
}

GameObject::GameObject(std::string path,sf::IntRect rect = sf::IntRect(0,0,128,128)){
	this->setTxt(path,rect);
	this->setTexture(&(this->texture));
	this->setOrigin(16,16);

	Pool::Pool::addToPool(this);
}

GameObject::GameObject(std::string path){
	this->setSize(sf::Vector2f(32,32)); 
	this->setTxt(path,sf::IntRect(0,0,128,128));
	this->setTexture(&(this->texture)); //false - пока не ебу что делает
	this->setOrigin(16,16);

	Pool::Pool::addToPool(this);
}

GameObject::GameObject(){
	this->setSize(sf::Vector2f(32,32)); // 128px of original * 0.25 = 32px
	this->setOrigin(16,16); //center of GameObject
	Pool::Pool::addToPool(this);
}

/*bool GameObject::isClicked(){
	
	bool tmpBool = false;
	if( Pool::EventPool::event->type == sf::Event::MouseButtonPressed && !Pool::EventPool::clicked ){		
		
		if(this->getGlobalBounds().contains(TempusImperium::mousePos) ){
			Pool::EventPool::clicked = true;
			tmpBool = true;
			return tmpBool;//for test
		}
	}
	
	else if( Pool::EventPool::event->type == sf::Event::MouseButtonReleased && Pool::EventPool::clicked){		
		
		if(this->getGlobalBounds().contains(TempusImperium::mousePos) ){
			Pool::EventPool::clicked = false;
			tmpBool = false;
			return tmpBool;//for test
		}
	}
	return tmpBool;
}*/

bool GameObject::isClicked(){
	
	bool tmpBool = false;
	if( Pool::EventPool::event->type == sf::Event::MouseButtonPressed && !Pool::EventPool::clicked ){		
		
		if(this->getGlobalBounds().contains(TempusImperium::mousePos) ){
			Pool::EventPool::clicked = true;
			tmpBool = false;
		}
	}
	
	else if( Pool::EventPool::event->type == sf::Event::MouseButtonReleased && Pool::EventPool::clicked){		
		
		if(this->getGlobalBounds().contains(TempusImperium::mousePos) ){
			Pool::EventPool::clicked = false;
			tmpBool = true;
		}
	}
	return tmpBool;
}

void GameObject::setPos(float x,float y){

	this->setPosition(x,y);
}

GameObject::~GameObject()
{

}