#include <SFML/Graphics.hpp>
#include <math.h>
#include "Converter/Generated.c"
#include "textures.c"
#define Wheight 512
#define Wlength 1024
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
    void incpx(float i){px += i*pdx;}
    void A(){pa -= 1; if(pa<0){pa += 360;} pdx = cos(pa*PI/180); pdy = sin(pa*PI/180);}
    void incpy(float i){py += i*pdy;}
    void D(){pa += 1; if(pa>360){pa -= 360;} pdx = cos(pa*PI/180); pdy = sin(pa*PI/180);}
    float pang(){return pa;}
    float pxpos(){return px;}
    float pypos(){return py;}
    float pdxpos(){return pdx;}
    float pdypos(){return pdy;}
    ~player(){delete P1;}
};


class Game  {
private:
    sf::RenderWindow *window;
    sf::RectangleShape *BRect,*WRect;
    player *p1;
    int *mapdataWall,mapX,mapY,mapSize;
public:
    Game();
    void Editmap(int MapData[64] ); 
    void GameLoop();
    void Render();
    void Inputs();
    void Rays();
    ~Game(){
        delete p1; 
        delete []mapdataWall; 
        delete BRect;
        delete WRect;
        delete window;
    }
};



void Game::Rays(){
    int vmt=0,hmt=0;
    int r,mx,my,mp,dof,disT;
    float rx,ry,ra,xo=0,yo=0;
    float pa=p1->pang();
    float px=p1->pxpos();
    float py=p1->pypos();
    ra=pa-30;
    for(r=0;r<60;r++){
        //Horizontal Check
        if(ra < 0) ra += 360;
        if(ra >= 360) ra -= 360;
        dof=0;
        float disH=10000000,hx=px,hy=py;
        float aTan=0;
        if(ra!=0&&ra!=180&&ra!=360){ aTan=-1/tan(ra*PI/180);}
        if(ra>180){ry=(((int)py>>6)<<6)-0.0001; rx = (py-ry)*aTan+px; yo=-64; xo=-yo*aTan;}
        if(ra<180){ry=(((int)py>>6)<<6)+64; rx = (py-ry)*aTan+px; yo=64; xo=-yo*aTan;}
        if(ra==0 || ra==180 || ra==360){rx=px; ry=py; dof=8;}
        while(dof<8){
            mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*8+mx;
            if(mp>0 && mp<8*8 && mapdataWall[mp]>0){vmt = mapdataWall[mp]-1;dof=8; hx=rx,hy=ry,disH=distance(px,py,rx,ry);}
            else{rx += xo; ry += yo; dof += 1;}
        }
        
        //Vertical Check
        dof=0;
        float disV=10000000,vx=px,vy=py;
        float nTan=0;
        if(ra!=90&&ra!=270){ nTan=-tan(ra*PI/180);}
        if(ra<270 && ra>90){rx=(((int)px>>6)<<6)-0.0001; ry = (px-rx)*nTan+py; xo=-64; yo=-xo*nTan;}
        if(ra<90 ||ra>270){rx=(((int)px>>6)<<6)+64; ry = (px-rx)*nTan+py; xo=64; yo=-xo*nTan;}
        if(ra==90 || ra==270 ){rx=px; ry=py; dof=8;}
        while(dof<8){
             mx=(int)(rx)>>6; my=(int)(ry)>>6; mp=my*8+mx;
            if(mp>0 && mp<8*8 && mapdataWall[mp]>0){hmt = mapdataWall[mp]-1;dof=8; vx=rx,vy=ry,disV=distance(px,py,rx,ry);}
            else{rx += xo; ry += yo; dof += 1;}
        }
        float shade=1;
        if(disV<disH){hmt=vmt; shade= 0.5; rx=vx,ry=vy; disT=disV;}
        else{rx=hx,ry=hy; disT=disH; shade = 1;}
        std::array line={
            sf::Vertex{sf::Vector2f(px,py)},
            sf::Vertex{sf::Vector2f(rx,ry)}
        };
        line[0].color = sf::Color::Cyan;
        line[1].color = sf::Color::Red;
        window->draw(line.data(), line.size(), sf::PrimitiveType::Lines);

        //3D Rendering
        
        float ca=pa-ra; if(ca<0){ca += 360;} if(ca>360){ca -= 360;} disT = disT*cos(ca*PI/180);
        float lineH=(mapSize*320)/disT; 
        float  ty_off=0;
        float ty_step=32.0/lineH;
        if(lineH>320){ty_off=(lineH-320)/2;lineH=320;}
        float lineO=160-lineH/2;
        float ty=ty_off*ty_step+((int)(hmt)*32); 
        float tx;
        if(shade==1){tx=(int)(rx/2.0)%32; if(ra>180){tx=31-tx;}}
        else{tx=(int)(ry/2.0)%32; if(ra>90  && ra<270){tx=31-tx;}}
         sf::RectangleShape rectangle({8.f,8.f});   
        for(int y=0;y<lineH;y++){
            float c=All_Textures[(int)(((int)ty)*32) + (int)(tx)]*shade;
           
            // rectangle.setFillColor(sf::Color::Red);
            if(disV<disH){ rectangle.setFillColor(sf::Color(255*c,255*c,255*c));}
            else{rectangle.setFillColor(sf::Color(255*c,255*c,255*c));}
            rectangle.setPosition(sf::Vector2f(r*8+530,lineO+y));      
            window->draw(rectangle);
            ty+=ty_step;
        }

        //draw floors
        for(int y=lineO+lineH;y<320;y++){
            float dy=y-(320/2.0),raFix=cos(pa-ra);
            tx=px/2 + cos(ra)*158*32/dy/raFix;
            ty=py/2 + sin(ra)*158*32/dy/raFix;
            float c=All_Textures[((int)(ty)&31)*32 + ((int)(tx)&31)]*0.7;
            
            rectangle.setFillColor(sf::Color(255*c,255*c,255*c));
            rectangle.setPosition(sf::Vector2f(r*8+530,y));      
            window->draw(rectangle);
        }

        ra += 1;
    }
    
}


Game::Game(){
    sf:: RenderWindow *Window = new sf::RenderWindow(sf::VideoMode({Wlength,Wheight}),"RayCaster Engine",sf::Style::Close);
    player *P1= new player(300,300);
    p1=P1;
    window=Window;
    mapX=8,mapY=8,mapSize=64;
    mapdataWall = new int[64];
    int Mtemp[64] = {1,1,1,1,1,1,1,1,
                     1,0,1,0,0,0,0,1,
                     1,0,1,0,0,0,0,3,
                     1,0,1,0,0,0,0,2,
                     1,0,1,0,0,0,0,1, 
                     1,0,0,0,0,1,0,1,
                     4,0,0,0,0,0,0,1,
                     1,1,1,1,1,1,1,1};
    for(int i=0;i<64;i++){
        mapdataWall[i]=Mtemp[i];
    }
    BRect=new sf::RectangleShape({62.f,62.f});
    WRect=new sf::RectangleShape({62.f,62.f});
    BRect->setFillColor(sf::Color::Black);
    WRect->setFillColor(sf::Color::White);
}


void Game::Editmap(int MapData[64]){
    for(int i=0;i<64;i++){
        mapdataWall[i]=MapData[i];
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
            if(mapdataWall[i*mapX+j]==0){
                BRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
                window->draw(*BRect);
            }
            else if(mapdataWall[i*mapX+j]>0){
                WRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
                window->draw(*WRect);
            }
        }
    }
    Rays();
    p1->updatepos();
    window->draw(*p1->Cpointer());
    window->draw(*p1->Rpointer());
    window->display();
}

void Game::Inputs(){
    int xo=0; if(p1->pdxpos()<0){xo = -10;}else{xo=10;}
    int yo=0; if(p1->pdypos()<0){yo = -10;}else{yo=10;}
    int ipx=p1->pxpos()/64; int ipx_add_xo=(p1->pxpos()+xo)/64; int ipx_sub_xo=(p1->pxpos()-xo)/64; 
    int ipy=p1->pypos()/64; int ipy_add_yo=(p1->pypos()+yo)/64; int ipy_sub_yo=(p1->pypos()-yo)/64; 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)){
        p1->A();
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)){
       p1->D();
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)){
            if(mapdataWall[ipy*mapX + ipx_sub_xo]==0){p1->incpx(-1);}
            if(mapdataWall[ipy_sub_yo*mapX + ipx]==0){p1->incpy(-1);} 
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
            if(mapdataWall[ipy*mapX + ipx_add_xo]==0){p1->incpx(1);}
            if(mapdataWall[ipy_add_yo*mapX + ipx]==0){p1->incpy(1);} 
    }
    else if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)){
        if(p1->pdxpos()<0){xo = -20;}else{xo=20;}
        if(p1->pdypos()<0){yo = -20;}else{yo=20;}
        ipx=p1->pxpos()/64; ipx_add_xo=(p1->pxpos()+xo)/64; 
        ipy=p1->pypos()/64; ipy_add_yo=(p1->pypos()+yo)/64; 
        if(mapdataWall[ipy_add_yo*mapX+ipx_add_xo]==4){mapdataWall[ipy_add_yo*mapX+ipx_add_xo]=0;} 
    }
}


int main()
{
    Game G2;
    G2.GameLoop();
}
