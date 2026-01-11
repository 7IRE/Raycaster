#include <SFML/Graphics.hpp>
#include <math.h>
#define Wheight 512
#define Wlength 1024
#define PI 3.1415926535
using namespace std;


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
    void incpx(float i){px += i*pdx*0.01;}
    void A(){pa -= 0.01; if(pa<0){pa += 360;} pdx = cos(pa*PI/180); pdy = sin(pa*PI/180);}
    void incpy(float i){py += i*pdy*0.01;}
    void D(){pa += 0.01; if(pa>360){pa -= 360;} pdx = cos(pa*PI/180); pdy = sin(pa*PI/180);}
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
    int *mapdata,mapX,mapY,mapSize;
public:
    Game();
    void Editmap(int MapData[64] ); 
    void GameLoop();
    void Render();
    void Inputs();
    void Rays();
    ~Game(){
        delete p1; 
        delete []mapdata; 
        delete BRect;
        delete WRect;
        delete window;
    }
};



void Game::Rays(){
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
            if(mp>0 && mp<8*8 && mapdata[mp]==1){dof=8; hx=rx,hy=ry,disH=distance(px,py,rx,ry);}
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
            if(mp>0 && mp<8*8 && mapdata[mp]==1){dof=8; vx=rx,vy=ry,disV=distance(px,py,rx,ry);}
            else{rx += xo; ry += yo; dof += 1;}
        }
        if(disV<disH){rx=vx,ry=vy; disT=disV;}
        else{rx=hx,ry=hy; disT=disH;}
        std::array line={
            sf::Vertex{sf::Vector2f(px,py)},
            sf::Vertex{sf::Vector2f(rx,ry)}
        };
        line[0].color = sf::Color::Cyan;
        line[1].color = sf::Color::Red;
        window->draw(line.data(), line.size(), sf::PrimitiveType::Lines);

        //3D Rendering
        float ca=pa-ra; if(ca<0){ca += 360;} if(ca>360){ca -= 360;} disT = disT*cos(ca*PI/180);
        float lineH=(mapSize*320)/disT; if(lineH>320){lineH=320;}
        float lineO=160-lineH/2;
        sf::RectangleShape rectangle({8.f,lineH});  
        // rectangle.setFillColor(sf::Color::Red);
        if(disV<disH){rectangle.setFillColor(sf::Color(255,0,0));}
        else{rectangle.setFillColor(sf::Color(255,100,100));}
        rectangle.setPosition(sf::Vector2f(r*8+530,lineO));      
        window->draw(rectangle);
        ra += 1;
    }
    
}


Game::Game(){
    sf:: RenderWindow *Window = new sf::RenderWindow(sf::VideoMode({Wlength,Wheight}),"RayCaster Engine",sf::Style::Close);
    player *P1= new player(300,300);
    p1=P1;
    window=Window;
    mapX=8,mapY=8,mapSize=64;
    mapdata = new int[64];
    int Mtemp[64] = {1,1,1,1,1,1,1,1,
                     1,0,1,0,0,0,0,1,
                     1,0,1,0,0,0,0,1,
                     1,0,1,0,0,0,0,1,
                     1,0,1,0,0,0,0,1,
                     1,0,0,0,0,1,0,1,
                     1,0,0,0,0,0,0,1,
                     1,1,1,1,1,1,1,1};
    for(int i=0;i<64;i++){
        mapdata[i]=Mtemp[i];
    }
    BRect=new sf::RectangleShape({62.f,62.f});
    WRect=new sf::RectangleShape({62.f,62.f});
    BRect->setFillColor(sf::Color::Black);
    WRect->setFillColor(sf::Color::White);
}


void Game::Editmap(int MapData[64]){
    for(int i=0;i<64;i++){
        mapdata[i]=MapData[i];
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
            if(mapdata[i*mapX+j]==0){
                BRect->setPosition(sf::Vector2f(j*mapSize+1,i*mapSize+1));
                window->draw(*BRect);
            }
            else if(mapdata[i*mapX+j]==1){
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
            if(mapdata[ipy*mapX + ipx_sub_xo]==0){p1->incpx(-1);}
            if(mapdata[ipy_sub_yo*mapX + ipx]==0){p1->incpy(-1);} 
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)){
            if(mapdata[ipy*mapX + ipx_add_xo]==0){p1->incpx(1);}
            if(mapdata[ipy_add_yo*mapX + ipx]==0){p1->incpy(1);} 
    }
}


int main()
{
    Game G2;
    G2.GameLoop();
}
