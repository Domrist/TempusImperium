#include "Pool.h"
#include "Grid.h"
#include "TempusImperium.h"

std::vector<sf::Drawable*> Pool::Pool::drawPool;
std::vector<GameObject*> Pool::Pool::statusPool;

sf::RenderWindow* Pool::Pool::renderWindow;
sf::Event* Pool::Pool::event;
sf::View* Pool::Pool::view;
bool Pool::EventPool::clicked = false;

void Pool::DrawPool::setRenderWindow(sf::RenderWindow *win){
	Pool::Pool::renderWindow = win;
}

void Pool::Pool::addToPool(GameObject *obj){	
	Pool::Pool::drawPool.push_back(obj);
	Pool::Pool::statusPool.push_back(obj);
}

void Pool::Pool::addToDrawPool(sf::Drawable* drawObj){
	Pool::Pool::drawPool.push_back(drawObj);
}

void Pool::Pool::updatePool(){}

void Pool::DrawPool::updateDrawPool()
{       
	//потом показываеем все объекты в пуле
	Pool::Pool::renderWindow->clear();
	Pool::Pool::renderWindow->setView(*(Pool::Pool::view));
	for(int i = 0;i < Pool::Pool::drawPool.size();i++){
		Pool::Pool::renderWindow->draw(*(Pool::Pool::drawPool[i]));
	}
	Pool::Pool::renderWindow->display();
}

void Pool::StatusPool::updateStatusPool(){
	
	TempusImperium::mousePos = Pool::Pool::renderWindow->mapPixelToCoords(sf::Mouse::getPosition(*(Pool::Pool::renderWindow)),*Pool::Pool::view);
	//сперва чистим пулл от пустых объектов
	for(int i = 0;i < Pool::Pool::statusPool.size();i++){
		if(Pool::Pool::statusPool[i] == nullptr){
			Pool::Pool::statusPool.erase(Pool::Pool::statusPool.begin() + i);
		}
	}
	Pool::Pool::statusPool.shrink_to_fit();
	//потом обновляем весь пулл
	for(int i = 0;i < Pool::Pool::statusPool.size();i++){
		if(Pool::Pool::statusPool[i] == nullptr || Pool::Pool::statusPool[i] == 0){
		}
		else{
			Pool::Pool::statusPool[i]->updateStatus();
		}
	}
}