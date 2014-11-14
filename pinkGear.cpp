#include <cmath>
#include "pinkGear.h"
#include "gamedata.h"
#include "frameFactory.h"

pinkGear::pinkGear(const std::string& name, const int order) :
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x")+rand()%100*5, 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")+rand()%100*5), 
           //Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX")+rand()%100*5, 
             //       Gamedata::getInstance().getXmlInt(name+"/speedY")+rand()%100*5),
           Vector2f(0, 0),
           order,
           Gamedata::getInstance().getXmlInt(name + "/width"),
           Gamedata::getInstance().getXmlInt(name + "/height")
           ),
  frame( FrameFactory::getInstance().getFrame(name) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( 100 ),
  startX (0),
  startY (0)
{ }

pinkGear::pinkGear(const string& n, const Vector2f& pos, const Vector2f& vel, const int order):
  Drawable(n, pos, vel,order, Gamedata::getInstance().getXmlInt(n + "/width"), Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( FrameFactory::getInstance().getFrame(n) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( 100 ),
  startX (0),
  startY (0)
{ }

pinkGear::pinkGear(const string& n, const Vector2f& pos, const Vector2f& vel,
               const Frame* frm,const int order):
  Drawable(n, pos, vel, order, Gamedata::getInstance().getXmlInt(n + "/width"),
           Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( frm ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( 100 ),
  startX (0),
  startY (0)
{ }

pinkGear::pinkGear(const pinkGear& s) :
  Drawable(s), 
  frame(s.frame),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( s.moveRange ),
  startX ( s.startX ),
  startY ( s.startY )
{ }

pinkGear& pinkGear::operator=(const pinkGear& rhs) {
  Drawable::operator=( rhs );
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  moveRange = rhs.moveRange;
  startX = rhs.startX;
  startY = rhs.startY;
  return *this;
}

void pinkGear::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

int pinkGear::getDistance(const pinkGear *obj) const { 
  return hypot(X()-obj->X(), Y()-obj->Y());
}

void pinkGear::update(Uint32 ticks) { 
  
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
/*
  if ( Y() < 0 ) {
    velocityY( abs( velocityY() ) );
  }
  if ( Y() > worldHeight-frameHeight ){
    velocityY( -abs( velocityY() ) );
  }

  if ( X() < 0 ){
    velocityX( abs( velocityX() ) );
  }
  if ( X() > worldWidth-frameWidth ){
    velocityX( -abs( velocityX() ) );
  } 
*/


  if(getIsMoved()){
    if(velocityX() == 0) velocityX(300);
    if( abs( X() - startX ) > 100 ) velocityX(-velocityX());
  }
  else{
    velocityX(0);
    startX = X();
  }



}


void pinkGear::update(Uint32 ticks, Drawable *BrotherpinkGear) { 
  
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
  
  float centerX = BrotherpinkGear->X() + BrotherpinkGear->getFrameWidth()/2;
  float centerY = BrotherpinkGear->Y() + BrotherpinkGear->getFrameHeight()/2;
  float theta;
  float radius = sqrt( pow( ( X()- centerX ), 2 ) + pow( ( Y()- centerY ), 2 ) );

  float fixRadius = -100 + sqrt( pow(BrotherpinkGear->getFrameWidth() ,2) + pow(BrotherpinkGear->getFrameHeight(), 2) ) ;

  if ( Y() > centerY && X() > centerX) {//4 area
    theta = atan((Y()-centerY)/(X()-centerX));

    X(fixRadius * cos(theta) + centerX );  Y(fixRadius * sin(theta) + centerY );
    velocityX(  1*sin(theta)*radius + BrotherpinkGear->velocityX());
    velocityY( -1*cos(theta)*radius + BrotherpinkGear->velocityY());
  }
  if ( Y() > centerY && X() < centerX) {//3 area
    theta = atan((Y() - centerY)/(centerX - X()));
    X(-1*fixRadius * cos(theta) + centerX);  Y(fixRadius * sin(theta) + centerY);
    velocityX( 1*sin(theta)*radius + BrotherpinkGear->velocityX());
    velocityY( 1*cos(theta)*radius + BrotherpinkGear->velocityY());
  }
  if ( Y() < centerY && X() < centerX) {//2 area
    theta = atan((centerY - Y())/(centerX - X()));
   X(-1*fixRadius * cos(theta) + centerX);    Y(-1*fixRadius * sin(theta) + centerY);
    velocityX( -1*sin(theta)*radius + BrotherpinkGear->velocityX());
    velocityY(  1*cos(theta)*radius + BrotherpinkGear->velocityY());
  }
  if ( Y() < centerY && X() > centerX) {//1 area
    theta = atan((centerY - Y())/(X() - centerX));
    X(fixRadius * cos(theta) + centerX);  Y(-1*fixRadius * sin(theta) + centerY);
    velocityX( -1*sin(theta)*radius + BrotherpinkGear->velocityX());
    velocityY( -1*cos(theta)*radius + BrotherpinkGear->velocityY());
  }
}

