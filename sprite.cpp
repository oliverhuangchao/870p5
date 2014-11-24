#include <cmath>
#include "sprite.h"
#include "gamedata.h"
#include "frameFactory.h"

Sprite::Sprite(const std::string& name, const int order) :
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x")+rand()%100*5, 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")+rand()%100*5), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX")+rand()%100*5, 
                    Gamedata::getInstance().getXmlInt(name+"/speedY")+rand()%100*5),
           order,
           Gamedata::getInstance().getXmlInt(name + "/width"),
           Gamedata::getInstance().getXmlInt(name + "/height")
           ),
  frame( FrameFactory::getInstance().getFrame(name) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Sprite::Sprite(const string& n, const Vector2f& pos, const Vector2f& vel, const int order):
  Drawable(n, pos, vel,order, Gamedata::getInstance().getXmlInt(n + "/width"), Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( FrameFactory::getInstance().getFrame(n) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Sprite::Sprite(const string& n, const Vector2f& pos, const Vector2f& vel,
               const Frame* frm,const int order):
  Drawable(n, pos, vel, order, Gamedata::getInstance().getXmlInt(n + "/width"),
           Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( frm ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

// prepare for chunk sprite
Sprite::Sprite(const string& n, const Vector2f& pos, const Vector2f& vel, const Frame* frm):
  Drawable(n, pos, vel, 100, Gamedata::getInstance().getXmlInt(n + "/width"),
           Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( frm ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }


Sprite::Sprite(const Sprite& s) :
  Drawable(s), 
  frame(s.frame),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Sprite& Sprite::operator=(const Sprite& rhs) {
  Drawable::operator=( rhs );
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

void Sprite::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

int Sprite::getDistance(const Sprite *obj) const { 
  return hypot(X()-obj->X(), Y()-obj->Y());
}

void Sprite::update(Uint32 ticks) { 
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
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
}


void Sprite::update(Uint32 ticks, Drawable *BrotherSprite) { 
  
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
  
  float centerX = BrotherSprite->X() + BrotherSprite->getFrameWidth()/2;
  float centerY = BrotherSprite->Y() + BrotherSprite->getFrameHeight()/2;
  float theta;
  float radius = sqrt( pow( ( X()- centerX ), 2 ) + pow( ( Y()- centerY ), 2 ) );

  float fixRadius = -100 + sqrt( pow(BrotherSprite->getFrameWidth() ,2) + pow(BrotherSprite->getFrameHeight(), 2) ) ;

  if( Y() > centerY && X() > centerX) {//4 area
    theta = atan((Y()-centerY)/(X()-centerX));

    X(fixRadius * cos(theta) + centerX );  Y(fixRadius * sin(theta) + centerY );
    velocityX(  1*sin(theta)*radius + BrotherSprite->velocityX());
    velocityY( -1*cos(theta)*radius + BrotherSprite->velocityY());
  }
  if ( Y() > centerY && X() < centerX) {//3 area
    theta = atan((Y() - centerY)/(centerX - X()));
    X(-1*fixRadius * cos(theta) + centerX);  Y(fixRadius * sin(theta) + centerY);
    velocityX( 1*sin(theta)*radius + BrotherSprite->velocityX());
    velocityY( 1*cos(theta)*radius + BrotherSprite->velocityY());
  }
  if ( Y() < centerY && X() < centerX) {//2 area
    theta = atan((centerY - Y())/(centerX - X()));
   X(-1*fixRadius * cos(theta) + centerX);    Y(-1*fixRadius * sin(theta) + centerY);
    velocityX( -1*sin(theta)*radius + BrotherSprite->velocityX());
    velocityY(  1*cos(theta)*radius + BrotherSprite->velocityY());
  }
  if ( Y() < centerY && X() > centerX) {//1 area
    theta = atan((centerY - Y())/(X() - centerX));
    X(fixRadius * cos(theta) + centerX);  Y(-1*fixRadius * sin(theta) + centerY);
    velocityX( -1*sin(theta)*radius + BrotherSprite->velocityX());
    velocityY( -1*cos(theta)*radius + BrotherSprite->velocityY());
  }
}

