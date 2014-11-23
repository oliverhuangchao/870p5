#include <cmath>
#include "pinkGear.h"
#include "gamedata.h"
#include "frameFactory.h"

pinkGear::~pinkGear(){}

pinkGear::pinkGear(const std::string& name, const int order) :
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x")+rand()%100*5, 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
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
  moveRange( Gamedata::getInstance().getXmlInt(name+"/moveRange") ),
  startX( Gamedata::getInstance().getXmlInt(name+"/startLoc/x")),
  startY( Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
  alreadyHit(false)
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
  startY (0),
  alreadyHit(false)
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
  startY (0),
  alreadyHit(false)
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
  startY ( s.startY ),
  alreadyHit(false)
{}

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
  alreadyHit = rhs.alreadyHit;
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

  if(getIsMoved()){
    if(velocityY() == 0) velocityY(300);
    if( abs( Y() - startY ) > moveRange ) velocityY(-velocityY());
  }
  else{
    velocityY(0);
    startY = Y();
  }
}


void pinkGear::update(Uint32 ticks, Drawable *BrotherpinkGear) {   
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
}

