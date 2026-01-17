#include <SFML/Graphics.hpp>
#include <math.h>
#include "Converter/Generated.hpp"
#include <iostream>
#include <windows.h>
#define Wheight 640
#define Wlength 960
#define PI 3.1415926535
using namespace std;

float DegToRad(float a){return a*M_PI/180;}
float fixAng(float a){if(a<0){ a+=360;}     if(a>360){a -=360;} return a;} 

float distance(float ax , float ay, float bx, float by){
    return (sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)));
}

class player {
    private:
        float px,py,pdx,pdy,pa;
        sf::CircleShape *P1;
        sf::RectangleShape *Pline;
    public:
    float elapsedtime , prevtime;
    player(int x=300,int y=300){
        px=x,py=y;
        P1=new sf::CircleShape(4.f);
        P1->setFillColor(sf::Color::Cyan);
        P1->setPosition(sf::Vector2f(px,py));
        Pline=new sf::RectangleShape({10.f,2.f});
        Pline->setFillColor(sf::Color::Cyan);
        Pline->setPosition(sf::Vector2f(px,py));
        pa=0;
        pdx=1,pdy=0;
    }
    sf::CircleShape* Cpointer(){return P1;}
    sf::RectangleShape* Rpointer(){return Pline;}
    void updatepos(){P1->setPosition(sf::Vector2f(px-4,py-4));  Pline->setPosition(sf::Vector2f(px,py)); Pline->setRotation(sf::degrees(pa));}
    void incpx(float i){px += 2*50*i*pdx*elapsedtime;}
    void A(){pa -= 2*25*elapsedtime; if(pa<0){pa += 360;} pdx = cos(pa*PI/180); pdy = sin(pa*PI/180);}
    void incpy(float i){py += 2*50*i*pdy*elapsedtime;}
    void D(){pa += 2*25*elapsedtime; if(pa>360){pa -= 360;} pdx = cos(pa*PI/180); pdy = sin(pa*PI/180);}
    float pang(){return pa;}
    float pxpos(){return px;}
    float pypos(){return py;}
    float pdxpos(){return pdx;}
    float pdypos(){return pdy;}
    void psetpos(int x,int y){px=x,py=y;}
    ~player(){delete P1;}
};


class Game  {
private:
    sf::RenderWindow *window;
    sf::Font font;
    sf::RectangleShape *BRect,*WRect;
    player *p1;
    int *mapdataWall,*mapF,*mapC,mapX,mapY,mapSize;
    sf::Clock *Clck;
    int state;
public:
    Game(int x=8 , int y=8);
    void Editmap(int *MapData , int Mode ); 
    void GameLoop();
    void Render();
    void Inputs();
    void Rays();
    void Win();
    void Lose();
    void setstate(int x){state=x;}
    int getstate(){return state;}
    player *pptr(){return p1;}
    ~Game(){
        delete p1; 
        delete Clck;
        delete []mapdataWall; 
        delete []mapF;
        delete []mapC;
        delete BRect;
        delete WRect;
        delete window;
    }
};


void Game::Win(){
    window->clear(sf::Color(50,255,50));
    font.openFromFile("Ariel2.ttf");
    sf::Text text(font); 
    text.setString("YOU WIN");
    text.setCharacterSize(100); 
    text.setFillColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);
    text.setPosition(sf::Vector2f(Wlength / 2.0f-100, Wheight / 2.0f-100));
    window->draw(text);
    window->display();
}

void Game::Lose(){
    window->clear(sf::Color(255,50,50));
    font.openFromFile("Ariel2.ttf");
    sf::Text text(font); 
    text.setString("YOU LOSE");
    text.setCharacterSize(100); 
    text.setFillColor(sf::Color::Black);
    text.setStyle(sf::Text::Bold);
    text.setPosition(sf::Vector2f(Wlength / 2.0f-100, Wheight / 2.0f-100));
    window->draw(text);
    window->display();
}



Game::Game(int x,int y){
    Clck= new sf::Clock;
    sf:: RenderWindow *Window = new sf::RenderWindow(sf::VideoMode({Wlength,Wheight}),"RayCaster Engine",sf::Style::Close);
    player *P1= new player(300,300);
    p1=P1;
    state=1;
    window=Window;
    p1->elapsedtime=0,p1->prevtime=0;
    mapX=x,mapY=y,mapSize=64;
    {
        mapdataWall = new int[x*y];
        for(int i=0;i<x*y;i++){
            mapdataWall[i]=0;
        }
        for(int i=0;i<y;i++){
            mapdataWall[i]=1;
            mapdataWall[y*(x-1)+i]=1;
        }
        for(int i=0;i<x;i++){
            mapdataWall[i*y]=1;
            mapdataWall[i*(y)+y-1]=1;
        }
    }
    {mapC = new int[x*y];
    
    for(int i=0;i<x*y;i++){
        mapC[i]=1;
    }}
    {mapF = new int[x*y];
    for(int i=0;i<x*y;i++){
        mapF[i]=2;
    }}
    BRect=new sf::RectangleShape({62.f,62.f});
    WRect=new sf::RectangleShape({62.f,62.f});
    BRect->setFillColor(sf::Color::Black);
    WRect->setFillColor(sf::Color::White);
}

void Game::Rays() {
    int vmt = 0, hmt = 0; 
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo = 0, yo = 0, disT;
    float disH, disV;
    
    float pa = p1->pang();
    float px = p1->pxpos();
    float py = p1->pypos();
    
    ra = fixAng(pa - 30); 

    for (r = 0; r < 120; r++) {
        dof = 0; disH = 1e30;
        float hx = px, hy = py;
        float aTan = -1 / tan(ra * PI / 180.0);
        if (ra > 180) { ry = (((int)py >> 6) << 6) - 0.0001; rx = (py - ry) * aTan + px; yo = -64; xo = -yo * aTan; }
        else if (ra < 180) { ry = (((int)py >> 6) << 6) + 64; rx = (py - ry) * aTan + px; yo = 64; xo = -yo * aTan; }
        else { rx = px; ry = py; dof = 20; }
        while (dof < 20) {
            mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
            if (mp >= 0 && mp < mapX*mapY && mapdataWall[mp] > 0) { hmt = mapdataWall[mp] - 1; hx = rx; hy = ry; disH = distance(px, py, hx, hy); dof = 20; }
            else { rx += xo; ry += yo; dof += 1; }
        }

        dof = 0; disV = 1e30;
        float vx = px, vy = py;
        float nTan = -tan(ra * PI / 180.0);
        if (ra > 90 && ra < 270) { rx = (((int)px >> 6) << 6) - 0.0001; ry = (px - rx) * nTan + py; xo = -64; yo = -xo * nTan; }
        else if (ra < 90 || ra > 270) { rx = (((int)px >> 6) << 6) + 64; ry = (px - rx) * nTan + py; xo = 64; yo = -xo * nTan; }
        else { rx = px; ry = py; dof = 20; }
        while (dof < 20) {
            mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
            if (mp >= 0 && mp < mapX*mapY && mapdataWall[mp] > 0) { vmt = mapdataWall[mp] - 1; vx = rx; vy = ry; disV = distance(px, py, vx, vy); dof = 20; }
            else { rx += xo; ry += yo; dof += 1; }
        }

        float shade = 1.0;
        int texNum;
        if (disV < disH) { rx = vx; ry = vy; disT = disV; shade = 0.5; texNum = vmt; }
        else { rx = hx; ry = hy; disT = disH; shade = 1.0; texNum = hmt; }

        float ca = fixAng(pa - ra); 
        disT = disT * cos(ca * PI / 180.0); 
        float lineH = (mapSize*640) / disT;
        float ty_step = 32.0 / (float)lineH;
        float ty_off = 0;
        if (lineH > 640) { ty_off = (lineH - 640) / 2.0; lineH = 640; }
        float lineO = 320 - lineH / 2;

        float tx;
        if (shade == 1.0) { tx = (int)(rx / 2.0) % 32; if (ra > 180) tx = 31 - tx; }
        else { tx = (int)(ry / 2.0) % 32; if (ra > 90 && ra < 270) tx = 31 - tx; }

        sf::RectangleShape rectangle({8.f, 1.f}); 
        float ty = ty_off * ty_step;
        for (int y = 0; y < (int)lineH; y++) {
            int pixel_y = (int)ty & 31;
            int tex_index = (texNum * 1024) + (pixel_y * 32 + (int)tx);
            float c1 = Img[3*tex_index] * shade; 
            float c2 = Img[3*tex_index+1] * shade; 
            float c3 = Img[3*tex_index+2] * shade; 
            rectangle.setFillColor(sf::Color( c1, c2, c3));
            rectangle.setPosition(sf::Vector2f(r * 8 , lineO + y));
            window->draw(rectangle);
            ty += ty_step;
        }

        float cosRa = cos(ra * PI / 180.0);
        float sinRa = sin(ra * PI / 180.0);
        float cosCa = cos(ca * PI / 180.0);

        for (int y = (int)(lineO + lineH); y < 640; y++) {
            float dy = y - 320.0;
            if (dy < 1) dy = 1;
            float dist = (320.0 * 32.0) / (dy * cosCa);

            float worldX = px + cosRa * dist * 2.0;
            float worldY = py + sinRa * dist * 2.0;
            int mX = (int)(worldX) >> 6; 
            int mY = (int)(worldY) >> 6;
            int mp_floor = mY * mapX + mX;

            int tx_f = (int)(worldX / 2.0) & 31;
            int ty_f = (int)(worldY / 2.0) & 31;
            int tex_pixel = ty_f * 32 + tx_f;

            int floorTex = 0, ceilTex = 1;
            if (mp_floor >= 0 && mp_floor < mapX*mapY) {
                floorTex = mapF[mp_floor];
                ceilTex = mapC[mp_floor];
            }

            float cf1 = Img[(floorTex * 1024 + tex_pixel)*3] ;
            float cf2 = Img[(floorTex * 1024 + tex_pixel)*3+1] ;
            float cf3 = Img[(floorTex * 1024 + tex_pixel)*3+2] ;
            rectangle.setFillColor(sf::Color(cf1, cf2, cf3));
            rectangle.setPosition(sf::Vector2f(r * 8, (float)y));
            window->draw(rectangle);
            
            float cc1 = Img[(ceilTex * 1024 + tex_pixel)*3] ;
            float cc2 = Img[(ceilTex * 1024 + tex_pixel)*3+1] ;
            float cc3 = Img[(ceilTex * 1024 + tex_pixel)*3+2] ;
            rectangle.setFillColor(sf::Color(cc1,cc2,cc3));
            rectangle.setPosition(sf::Vector2f(r * 8 , (float)(640 - y)));
            window->draw(rectangle);
        }
        ra = fixAng(ra + 0.5);
    }
}

void Game::Editmap(int *MapData , int Mode /*1 - wall , 2 - Floor , 3 - Celling*/){
    if(Mode==1){
        for(int i=0;i<mapX*mapY;i++){
            mapdataWall[i]=MapData[i];
        }
    }
    else if(Mode==2){
        for(int i=0;i<mapX*mapY;i++){
            mapF[i]=MapData[i];
        }
    }
    else if(Mode==3){
        for(int i=0;i<mapX*mapY;i++){
            mapC[i]=MapData[i];
        }
    }
}


void Game::GameLoop(){
    while (window->isOpen() && state == 1)
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
    // for(int i=0;i<mapY;i++){
    //     for(int j=0;j<mapX;j++){
    //         if(mapdataWall[i*mapX+j]==0){
    //             BRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
    //             window->draw(*BRect);
    //         }
    //         else if(mapdataWall[i*mapX+j]>0){
    //             WRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
    //             window->draw(*WRect);
    //         }
    //     }
    // }
    Rays();
    p1->prevtime=p1->elapsedtime;
    p1->elapsedtime=Clck->getElapsedTime().asSeconds();

    Clck->restart();

    // p1->updatepos();
    // window->draw(*p1->Cpointer());
    // window->draw(*p1->Rpointer());
    window->display();
    cout<<p1->elapsedtime;
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
            if(mapdataWall[ipy*mapX + ipx_sub_xo]==0){p1->incpx(-1);}
            if(mapdataWall[ipy_sub_yo*mapX + ipx]==0){p1->incpy(-1);} 
            if(mapF[ipy*mapX + ipx]==5){state=2;}
            if(mapF[ipy*mapX + ipx]==4){state=3;}
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
            if(mapdataWall[ipy*mapX + ipx_add_xo]==0){p1->incpx(1);}
            if(mapdataWall[ipy_add_yo*mapX + ipx]==0){p1->incpy(1);} 
            if(mapF[ipy*mapX + ipx]==5){state=2;}
            if(mapF[ipy*mapX + ipx]==4){state=3;}

    }
    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)){
        if(p1->pdxpos()<0){xo = -20;}else{xo=20;}
        if(p1->pdypos()<0){yo = -20;}else{yo=20;}
        ipx=p1->pxpos()/mapSize; ipx_add_xo=(p1->pxpos()+xo)/mapSize; 
        ipy=p1->pypos()/mapSize; ipy_add_yo=(p1->pypos()+yo)/mapSize; 
        if(mapdataWall[ipy_add_yo*mapX+ipx_add_xo]==4){mapdataWall[ipy_add_yo*mapX+ipx_add_xo]=0;} 
    }
}


int main()
{
    Game G2(20,20);
    do{
    G2.setstate(1);
    (G2.pptr())->psetpos(96,96);
    {int Map[400]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,0,0,0,1,
                  1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,1,1,1,0,1,
                  1,0,1,0,0,0,1,0,2,1,0,0,1,0,1,0,0,0,0,1,
                  1,4,1,0,1,0,1,0,1,0,0,1,0,0,1,0,1,1,0,1,
                  1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,0,1,1,0,1,
                  1,0,0,0,1,0,1,0,1,0,0,0,0,0,1,0,0,0,0,1,
                  1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,0,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,1,
                  1,0,1,1,1,0,1,0,1,0,1,1,1,1,1,1,1,1,0,1,
                  1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
                  1,0,1,1,1,0,1,0,1,1,1,0,1,1,0,1,1,1,0,1,
                  1,0,1,0,1,0,1,0,1,0,1,0,0,1,0,1,0,0,0,1,
                  1,0,1,0,1,0,1,0,1,0,1,0,0,0,0,1,0,1,0,1,
                  1,0,1,0,1,0,1,0,1,0,1,1,1,0,0,1,0,1,0,1,
                  1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,1,
                  1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,
                  1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    G2.Editmap(Map,1);}
   {int Map[400]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    G2.Editmap(Map,3);}
    {int Map[400]={6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,0,6,6,6,6,6,5,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,4,4,4,4,4,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,5,5,6,
                   6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,};

    G2.Editmap(Map,2);}
    G2.GameLoop();
    if(G2.getstate()==3){G2.Win();}
    else if(G2.getstate()==2){G2.Lose();}
    Sleep(5000);}while(G2.getstate()!=3);
    return 0;
}
