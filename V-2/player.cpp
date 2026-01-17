#include "player.hpp"
#define speed 0.1
#define rotation 0.1
const double Deg2PI = M_PI/180.0;
const double PI2Deg = 180.0/M_PI;

player::player(int x,int y){
    px=x,py=y;
    Pcirc=new sf::CircleShape(4.f);
    Pcirc->setFillColor(sf::Color::Cyan);
    Pcirc->setPosition(sf::Vector2f(px,py));
    Pline=new sf::RectangleShape({10.f,2.f});
    Pline->setFillColor(sf::Color::Cyan);
    Pline->setPosition(sf::Vector2f(px,py));
    pa=0;
    pdx=1,pdy=0;
}

sf::CircleShape* player::Cpointer(){return Pcirc;}

sf::RectangleShape* player::Rpointer(){return Pline;}

void player::updatepos(){
    Pcirc->setPosition(sf::Vector2f(px-4,py-4));  
    Pline->setPosition(sf::Vector2f(px,py)); 
    Pline->setRotation(sf::degrees(pa));
}

void player::incpx(float i){
    px += speed*i*pdx;
}

void player::A(){
    pa -= rotation;
    if(pa<0){
        pa += 360;
    }
    pdx = cos(pa*Deg2PI); 
    pdy = sin(pa*Deg2PI);
}

void player::incpy(float i){
    py += speed*i*pdy;
}

void player::D(){
    pa += rotation;
    if(pa>360){
        pa -= 360;
    } 
    pdx = cos(pa*Deg2PI); 
    pdy = sin(pa*Deg2PI);
}

float player::pang(){return pa;}

float player::pxpos(){return px;}

float player::pypos(){return py;}

float player::pdxpos(){return pdx;}

float player::pdypos(){return pdy;}

player::~player(){delete Pcirc;}