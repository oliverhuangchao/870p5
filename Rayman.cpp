#include "Rayman.h"
#include "gamedata.h"
#include "frameFactory.h"

void Rayman::advanceFrame(Uint32 ticks) {
	timeSinceLastFrame += ticks;
	if (timeSinceLastFrame > frameInterval) {
    currentFrame = (currentFrame+1) % numberOfFrames;
		timeSinceLastFrame = 0;
	}
}

Rayman::Rayman( const std::string& name, const std::string& second, const int order) :
  Drawable(name, 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x")+rand()%100*5, 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/speedX"),
                    Gamedata::getInstance().getXmlInt(name+"/speedY")),
           //Vector2f(0, 0),
           order,
           Gamedata::getInstance().getXmlInt(name +"/"+ second + "/width"),
           Gamedata::getInstance().getXmlInt(name +"/"+ second +"/height")
           ),
  frames( FrameFactory::getInstance().getFrames(name,second) ),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),

  currentFrame(0),
  numberOfFrames( Gamedata::getInstance().getXmlInt(name+"/"+ second +"/frames") ),
  frameInterval( Gamedata::getInstance().getXmlInt(name+"/"+ second +"/frameInterval") ),
  timeSinceLastFrame( 0 ),
  frameWidth(frames[0]->getWidth()),
  frameHeight(frames[0]->getHeight()),
  alreadyCrawling ( false ),
  currentLandPos( 0 ),
  currentLandPosSet( false ),
  isReachTop( false ),
  isCrawl( false ),
  isJump( false ),
  frameDirection( 0 ),
  faceDirection( 1 ),
  isFist( false ),
  fistDuration( 0 ),
  frameName( name )
{ }

Rayman::Rayman(const Rayman& s) :
  Drawable(s), 
  frames(s.frames),
  worldWidth( s.worldWidth ),
  worldHeight( s.worldHeight ),
  currentFrame(s.currentFrame),
  numberOfFrames( s.numberOfFrames ),
  frameInterval( s.frameInterval ),
  timeSinceLastFrame( s.timeSinceLastFrame ),
  frameWidth( s.frameWidth ),
  frameHeight( s.frameHeight ),
  alreadyCrawling(s.alreadyCrawling),
  currentLandPos(s.currentLandPos),
  currentLandPosSet(s.currentLandPosSet),
  isReachTop(s.isReachTop),
  isCrawl( s.isCrawl ),
  isJump( s.isJump ),
  frameDirection( s.frameDirection ),
  faceDirection( s.faceDirection ),
  isFist( s.isFist ),
  fistDuration( s.fistDuration ),
  frameName(s.frameName)
  { }

void Rayman::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frames[currentFrame]->draw(x, y);
}

void Rayman::update(Uint32 ticks) { 

  advanceFrame(ticks);
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
  
  switch (frameDirection){
      case 1: 
        if( !isCrawl && !isJump ){ // velocityX() != 500
          frames = FrameFactory::getInstance().getFrames(frameName, "runRight");
          velocityX(500);
          break;
        }
        if ( isCrawl ){
          frames = FrameFactory::getInstance().getFrames(frameName, "crawlRight");
          velocityX(500);
          break;
        }
        if ( isJump ){
           jumpFunction();
           break;
        }
        break;
      case -1:
        if( !isCrawl && !isJump ){ 
          frames = FrameFactory::getInstance().getFrames(frameName, "runLeft");
          velocityX(-500);
          break;
        }
        if ( isCrawl ){
          frames = FrameFactory::getInstance().getFrames(frameName, "crawlLeft");
          velocityX(-500);
          break;
        }
        if ( isJump ){
          jumpFunction();
           break;
        }
        break;
      case 0: //no moving action
        velocityX(0);
        if ( !isCrawl && !isJump && !isFist ){//keep standing
            if (faceDirection== -1) {
              frames = FrameFactory::getInstance().getFrames(frameName, "standLeft");
            }
            if (faceDirection== 1){
               frames = FrameFactory::getInstance().getFrames(frameName, "standRight"); 
            }
            alreadyCrawling = false;
            break;
        } 
        if ( isCrawl ){//keep crawling postion
            if(!alreadyCrawling){
                if (faceDirection== -1)
                    frames = FrameFactory::getInstance().getFrames(frameName, "standToLeftCrawl");
                if (faceDirection== 1)
                    frames = FrameFactory::getInstance().getFrames(frameName, "standToRightCrawl");
                alreadyCrawling = true; 
            }
            else{
                if (faceDirection== -1)
                    frames = FrameFactory::getInstance().getFrames(frameName, "keepLeftCrawl");
                if (faceDirection== 1)
                    frames = FrameFactory::getInstance().getFrames(frameName, "keepRightCrawl");
            }
            break;
        }
        if ( isJump ){//jump action
            jumpFunction();
            break;
        }
        if ( isFist ){//fist action
            if (faceDirection== -1){
                frames = FrameFactory::getInstance().getFrames(frameName, "fistLeft");
              }
            if (faceDirection== 1){
                frames = FrameFactory::getInstance().getFrames(frameName, "fistRight");
              }
            if (currentFrame == 8 && fistDuration != 3){ //total can insist 3 seconds
                currentFrame = 5;
            }
            break;
        }
        
        break;

      default:
        break;
   }
}

inline void Rayman::jumpFunction(){
  float spd = 700;
  if(!currentLandPosSet) {
      currentLandPos = Y();
      currentLandPosSet = true;
  }
    
  if(faceDirection== -1)
    frames = FrameFactory::getInstance().getFrames(frameName, "jumpLeft");
  if(faceDirection== 1)
    frames = FrameFactory::getInstance().getFrames(frameName, "jumpRight");

  if( abs(currentLandPos - Y()) > 300 ){
    velocityY(spd);
    isReachTop = true;
  }
  else{
    if(!isReachTop)
      velocityY(-spd);
  }

  if( Y() > currentLandPos ){
    //std::cout<<"finish jump"<<std::endl;
    setIsJump(false);
    isReachTop = false;
    currentLandPosSet = false;
    Y(currentLandPos);
    velocityY(0);
  }
}

void Rayman::update(Uint32 ticks, Drawable*) {
  std::cout<<ticks<<std::endl;
} 

