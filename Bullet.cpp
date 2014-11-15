#include <cmath>
#include "Bullet.h"
#include "gamedata.h"
#include "frameFactory.h"
#include <iostream>

Bullet::Bullet(const std::string& name, const int order) :
  Drawable(name,
           Vector2f(0, 0), 
           Vector2f(0, 0),
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

Bullet::Bullet(const string& n, const Vector2f& pos, const Vector2f& vel, const int order):
  Drawable(n, pos, vel,order, Gamedata::getInstance().getXmlInt(n + "/width"), Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( FrameFactory::getInstance().getFrame(n) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Bullet::Bullet(const string& n, const Vector2f& pos, const Vector2f& vel,
               const Frame* frm,const int order):
  Drawable(n, pos, vel, order, Gamedata::getInstance().getXmlInt(n + "/width"),
           Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( frm ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Bullet::Bullet(const Bullet& s) :
  Drawable(s), 
  frame(s.frame),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height"))
{ }

Bullet& Bullet::operator=(const Bullet& rhs) {
  Drawable::operator=( rhs );
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  return *this;
}

void Bullet::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

int Bullet::getDistance(const Bullet *obj) const { 
  return hypot(X()-obj->X(), Y()-obj->Y());
}

void Bullet::update(Uint32 ticks) { 
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
}

void Bullet::update(Uint32 ticks, Drawable *BrotherBullet){
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
// still something wrong with this part
/*
  float deltaY = Y() + getFrameHeight()/2 - (BrotherBullet->Y() + BrotherBullet->getFrameHeight()/2);
  float deltaX = X() + getFrameWidth()/2  - (BrotherBullet->X() + BrotherBullet->getFrameWidth()/2); 

  float alpha = atan( abs(deltaY) / abs(deltaX) );
  std::cout<<deltaX<<'\t'<<deltaY<<'\t'<<alpha<<std::endl;
*/
  float alpha = atan( abs( Y() - (BrotherBullet->Y() + 100) ) / ( X() - (BrotherBullet->X() + 100) ) );
  if ( abs( X() - BrotherBullet-> X() ) > getFistRange() ) 
  {

    if( velocityX() > 0 && X() > BrotherBullet->X() )
    {
      frame = FrameFactory::getInstance().getFrame("singleFistLeft");
      velocityY(300 * sin(alpha));
      velocityX(-300 * cos(alpha));
    }
    if( velocityX() < 0 && X() < BrotherBullet->X() )
    {
      frame = FrameFactory::getInstance().getFrame("singleFistRight");
      velocityY(-300 * sin(alpha));
      velocityX(300 * cos(alpha));
    }
  }
}