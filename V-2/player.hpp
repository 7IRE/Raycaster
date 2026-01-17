#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <cmath>

class player {
    private:
        float px,py,pdx,pdy,pa;
        //Draw Player
        sf::CircleShape *Pcirc;
        sf::RectangleShape *Pline;
    public:

    player(int x=300,int y=300);
    sf::CircleShape* Cpointer();
    sf::RectangleShape* Rpointer();
    void updatepos();
    void incpx(float i);
    void A();
    void incpy(float i);
    void D();
    float pang();
    float pxpos();
    float pypos();
    float pdxpos();
    float pdypos();
    ~player();
};



#endif