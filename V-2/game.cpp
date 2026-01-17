#include <SFML/Graphics.hpp>
#include <cmath>
#include "player.hpp"
#include "textures.c"

#define Wheight 512
#define Wlength 1024
#define Rheight 120
#define Rlength 360

// Texture size and FOV
#define Texsize 32
#define FOV 60
using namespace std;

float distance(float ax , float ay, float bx, float by){
    return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}
float DegToRad(float a){return a*M_PI/180;}
float fixAng(float a){if(a<0){ a+=360;}     if(a>360){a -=360;} return a;} 

class Game  {
private:
    sf::RenderWindow *window;
    sf::RectangleShape *BRect,*WRect;
    player *p1;
    int **mapW;
    int mapX,mapY,mapSize;
public:
    Game(int x=8,int y=8 , int recsize=62);
    void Editmap(int *MapData , int size); 
    void GameLoop();
    void Render();
    void Inputs();
    void Rays();
    ~Game(){
        delete p1; 
        delete []mapW; 
        delete BRect;
        delete WRect;
        delete window;
    }
};

void Game::Rays(){
    float px = p1->pxpos(), py = p1->pypos(), pa = p1->pang();
    float ra = fixAng(pa - (FOV / 2.0));
    sf::RectangleShape pixel({8.f,8.f});

    for(int r=0 ; r<FOV;r++){
        //Horizontal line check
        int dof=0;
        float disH=1e30 , hx = px , hy = py;
        float aTan=-1/tan(DegToRad(ra));
        float rx,ry,xo,yo;
        if(ra>180){
            ry=(((int)py>>6)<<6)-0.0000001;
            rx=((py-ry)*aTan + px);
            yo=-mapSize; xo= -yo * aTan;
        }
        else if(ra<180){
            ry=(((int)py>>6)<<6)+mapSize;
            rx=(py-ry)*aTan +px;
            yo = mapSize; xo = -yo * aTan;
        }
        else{rx = px ; ry = py ; dof = 8;}

        while(dof<8){
            int mx = (int)(rx) >> 6, my = (int)(ry) >> 6, mp = my * mapX + mx;
            if (mp >= 0 && mp <mapX*mapY  && mapW[mx][my] > 0) {
                hx = rx; hy = ry; disH = distance(px, py, hx, hy); dof = 8;
            } else { rx += xo; ry += yo; dof++; }
        }

        //Vertical Line Check
        dof = 0;
        float disV = 1e30, vx = px, vy = py;
        float nTan = -tan(DegToRad(ra));

        if (ra > 90 && ra < 270) { 
            rx = (((int)px >> 6) << 6) - 0.0001; 
            ry = (px - rx) * nTan + py; 
            xo = -mapSize; yo = -xo * nTan;
        } else if (ra < 90 || ra > 270) { // Looking Right
            rx = (((int)px >> 6) << 6) + mapSize; 
            ry = (px - rx) * nTan + py; 
            xo = mapSize; yo = -xo * nTan;
        } else { rx = px; ry = py; dof = 8; }

        while (dof < 8) {
            int mx = (int)(rx) >> 6, my = (int)(ry) >> 6, mp = my * mapX + mx;
            if (mp >= 0 && mp < 64 && mapW[mx][my] > 0) {
                vx = rx; vy = ry; disV = distance(px, py, vx, vy); dof = 8;
            } else { rx += xo; ry += yo; dof++; }
        }

        float disT, shade = 1.0;
        if (disV < disH) { rx = vx; ry = vy; disT = disV; shade = 0.5; } 
        else { rx = hx; ry = hy; disT = disH; shade = 1.0; }

        float ca = fixAng(pa - ra);
        disT = disT * cos(DegToRad(ca));

        
        float lineH = (mapSize * Rheight) / disT;
        float lineO = 160 - lineH / 2;
        float ty_step = (float)Texsize/ lineH;
        float ty_off = 0;
        if (lineH > Rheight) { ty_off = (lineH - Texsize) / 2.0; lineH = Rheight; }
        
        float ty = ty_off * ty_step;
        float tx = (shade == 1.0) ? (int)(rx / 2.0) % Texsize : (int)(ry / 2.0) % Texsize;

        for (int y = 0; y < lineH; y++) {
            int pixel_idx = ((int)ty * Texsize) + (int)tx;
            float colorVal = All_Textures[pixel_idx] * shade;
            pixel.setFillColor(sf::Color(255 * colorVal, 255 * colorVal, 255 * colorVal));
            pixel.setPosition(sf::Vector2f(r * 8 + 530, lineO + y));
            window->draw(pixel);
            ty += ty_step;
        }

        
        for (int y = lineO + lineH; y < Rheight; y++) {
            float dy = y - (Rheight / 2.0);
            float degRa = DegToRad(ra);
            float cosCa = cos(DegToRad(ca));
            
            float floorDist = (160.0 * 32.0) / (dy * cosCa);
            float ftx = px / 2.0 + cos(degRa) * floorDist;
            float fty = py / 2.0 + sin(degRa) * floorDist;

            float c = All_Textures[((int)(fty) & 31) * 32 + ((int)(ftx) & 31)] * 0.7;
            pixel.setFillColor(sf::Color(255 * c, 255 * c, 255 * c));
            pixel.setPosition(sf::Vector2f(r * 8 + 530, lineO + y));
            window->draw(pixel);
        }

        ra=ra+1;
    }
}

Game::Game(int x,int y, int recsize){
    sf:: RenderWindow *Window = new sf::RenderWindow(sf::VideoMode({Wlength,Wheight}),"RayCaster Engine",sf::Style::Close);
    player *P1= new player(300,300);
    p1=P1;
    window=Window;
    mapX=x,mapY=y,mapSize=recsize+2;
    mapW = new int*[x];
    for(int i=0;i<x;i++){
        mapW[i] = new int[y]{};
    }
    for(int i=0;i<mapX;i++){
        mapW[i][0]=1;
        mapW[i][mapY-1]=1;
    }
    for(int i=0;i<mapY;i++){
        mapW[0][i]=1;
        mapW[mapX-1][i]=1;
    }

    BRect=new sf::RectangleShape({static_cast<float>(recsize),static_cast<float>(recsize)});
    WRect=new sf::RectangleShape({static_cast<float>(recsize),static_cast<float>(recsize)});
    BRect->setFillColor(sf::Color::Black);
    WRect->setFillColor(sf::Color::White);
}

void Game::Editmap(int *MapData , int size){
    for(int i=0; i<size; i++){
        int row = i / mapY;   
        int col = i % mapY;   
        if(row < mapX && col < mapY){
            mapW[row][col] = MapData[i];
        }
    }
}

void Game::GameLoop(){
    while (window->isOpen())
    {
        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window->close();
            
        }
        Inputs();
        Render();
    }
}

void Game::Render(){
    window->clear(sf::Color(100,100,100));
    for(int i=0;i<mapY;i++){
        for(int j=0;j<mapX;j++){
            if(mapW[i][j]==0){
                BRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
                window->draw(*BRect);
            }
            else if(mapW[i][j]>0){
                WRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
                window->draw(*WRect);
            }
        }
    }
    p1->updatepos();
    window->draw(*p1->Cpointer());
    window->draw(*p1->Rpointer());
    window->display();
}

void Game::Inputs(){
    int xo=0; if(p1->pdxpos()<0){xo = -10;}else{xo=10;}
    int yo=0; if(p1->pdypos()<0){yo = -10;}else{yo=10;}
    int ipx=p1->pxpos()/mapSize; int ipx_add_xo=(p1->pxpos()+xo)/mapSize; int ipx_sub_xo=(p1->pxpos()-xo)/mapSize; 
    int ipy=p1->pypos()/mapSize; int ipy_add_yo=(p1->pypos()+yo)/mapSize; int ipy_sub_yo=(p1->pypos()-yo)/mapSize; 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
        p1->A();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
       p1->D();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
            if(mapW[ipy][ipx_sub_xo]==0){p1->incpx(-1);}
            if(mapW[ipy_sub_yo][ipx]==0){p1->incpy(-1);} 
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
            if(mapW[ipy][ipx_add_xo]==0){p1->incpx(1);}
            if(mapW[ipy_add_yo][ipx]==0){p1->incpy(1);} 
    }
}

//-----------------------------------------------------------------------------------------
int main()
{
    Game G2;
    G2.GameLoop();
}
