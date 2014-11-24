#include <iostream>
#include <string>
#include <iomanip>
#include "sprite.h"
#include "gamedata.h"
#include "manager.h"
#include "drawable.h"
#include "pinkGear.h"
#include "Rayman.h"
#include "aaline.h"
#include "Bullet.h"
#include "explodingSprite.h"
#include <cmath>



bool getDistance(int a1,int a2, int b1, int b2, int value);

Manager::~Manager() { 
  // These deletions eliminate "definitely lost" and
  // "still reachable"s in Valgrind.
  for (unsigned i = 0; i < sprites.size(); ++i) {
    delete sprites[i];
  }
}

Manager::
Manager() :
env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
io( IOManager::getInstance() ),
clock( Clock::getInstance() ),
screen( io.getScreen() ),
world_back("back", Gamedata::getInstance().getXmlInt("back/factor") ),
world_front("front", Gamedata::getInstance().getXmlInt("front/factor") ),
viewport( Viewport::getInstance() ),
sprites(),
currentSprite(0),

makeVideo( false ),
eatStar( false ),
isShowHug ( true ),
singlePostion( 0 ),

stopWatch(true),
stopWatch_Begin(0),
stopWatch_End(0),
fistStartPos(0),
pinkGearStartPos(0),
fistReadyToTurn(false),
//alreadyHit(false),

frameCount( 0 ),
username(  Gamedata::getInstance().getXmlStr("username") ),
title( Gamedata::getInstance().getXmlStr("screenTitle") ),
frameMax( Gamedata::getInstance().getXmlInt("frameMax") )
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw string("Unable to initialize SDL: ");
    }
    SDL_WM_SetCaption(title.c_str(), NULL);
    atexit(SDL_Quit);

    // ----- set the Rayman -----
    sprites.push_back( new Rayman("Rayman","runRight", singlePostion) );
    ++singlePostion;

    //-------set the pinkGears-----------
    pinkGearStartPos = singlePostion;
    for (int i = 0; i < Gamedata::getInstance().getXmlInt("pinkGear/count"); i++){
      sprites.push_back( new pinkGear("pinkGear",singlePostion) );
      ++singlePostion;
    }
  
    viewport.setObjectToTrack(sprites[currentSprite]);
}

void Manager::draw() const {
  float linePos;
  const Uint32 BLUE = SDL_MapRGB(screen->format, 0, 0, 0xFF);

  world_back.draw();

  world_front.draw();

  for (unsigned i = 0; i < sprites.size(); ++i) {
    sprites[i]->draw();
  }
  
  linePos = 20;

  io.printMessageAt("press 'm' to show the huds", 20, linePos);
  linePos +=30; 

  if(isShowHug){
    //draw the contents
    
    io.printMessageValueAt("Seconds: ", clock.getSeconds(), 20, linePos);
    linePos +=30; 

    io.printMessageValueAt("fps: ", clock.getAvgFps(), 20, linePos);
    linePos +=30; 

    io.printMessageAt("Author: Chao Huang", 20, linePos);
    linePos +=30; 

    io.printMessageAt("Press T to switch sprites", 20, linePos);
    linePos +=30; 

    io.printMessageAt("Choose the pinkGear and press 'e'", 20, linePos);
    linePos +=30;

    io.printMessageAt("press 'a' to turn left", 20, linePos);
    linePos +=30;

    io.printMessageAt("press 'd' to turn right", 20, linePos);
    linePos +=30;

    io.printMessageAt("press 'w' to jump", 20, linePos);
    linePos +=30;

    io.printMessageAt("press 's' to crawl", 20, linePos);
    linePos +=30;

    io.printMessageAt("press 'f' to fight", 20, linePos);
    linePos +=30;

    io.printMessageAt("press 'r' to reset", 20, linePos);
    linePos +=30;

    //draw the lines
    float left = (float)Gamedata::getInstance().getXmlInt("myhud/start/left"); 
    float top = (float)Gamedata::getInstance().getXmlInt("myhud/start/top");
    float right = (float)Gamedata::getInstance().getXmlInt("myhud/start/right"); 
    float buttom = linePos + top;

    Draw_AALine(screen, left, top, left, buttom, 2.0f, BLUE);
    Draw_AALine(screen, left, buttom, right, buttom, 2.0f, BLUE);
    Draw_AALine(screen, right, buttom, right, top, 2.0f, BLUE);
    Draw_AALine(screen, right, top, left, top, 2.0f, BLUE);

  }

  viewport.draw();

  SDL_Flip(screen);
}

void Manager::makeFrame() {
  std::stringstream strm;
  strm << "frames/" << username<< '.' 
       << std::setfill('0') << std::setw(4) 
       << frameCount++ << ".bmp";
  std::string filename( strm.str() );
  std::cout << "Making frame: " << filename << std::endl;
  SDL_SaveBMP(screen, filename.c_str());
}

void Manager::update() {
  ++clock;
  world_back.update();
  world_front.update();
 
    
  Uint32 ticks = clock.getElapsedTicks();
  
  sprites[0]->update(ticks);//update our hero Rayman

  for (unsigned int i = 1; i < sprites.size(); ++i)// update pinkGear start @ 1 position
  {
    if(fistStartPos != 0 && i >= fistStartPos)//if fist exist and it is now update fist
    {  
      sprites[i]->update(ticks, sprites[0]);
      if(abs(sprites[i]->X() - sprites[0]->X()) > sprites[i]->getFistRange() ){
          fistReadyToTurn = true;
      }
      if(spriteConflict(sprites[i],sprites[0]) && fistReadyToTurn){// the fist gets back to Rayman
        sprites.erase(sprites.begin() + i);
        fistStartPos = 0;
        fistReadyToTurn = false;
        singlePostion--;
      }
    }
    else{
      sprites[i]->update(ticks);
      if (spriteConflict(sprites[i], sprites[fistStartPos]) 
          && !static_cast<pinkGear*>(sprites[i])-> getAlreadyHit()
          && fistStartPos != 0){// if the fist hit the sprite
        pinkGear *tmp = static_cast<pinkGear*>(sprites[i]);   
        sprites[i] = new ExplodingSprite( *tmp );
        static_cast<pinkGear*>(sprites[i])-> setAlreadyHit(true);
        delete tmp;
      }
    }
  }

  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }

  viewport.update(); 
}

bool Manager::spriteConflict( Drawable* multi, Drawable* single ){
  if ( getDistance(multi->X()+multi->getFrameWidth()/2, multi->Y()+multi->getFrameHeight()/2, single->X()+single->getFrameWidth()/2,single->Y() + single->getFrameHeight()/2, 100)){
   
    return true;
  }
  else
    return false;
}

bool getDistance(int x1,int y1, int x2, int y2, int value){// return the value 
  int result = sqrt( pow( (x1 - x2), 2 ) + pow( ( y1 - y2 ), 2 ) );
  if (result <= value)
    return true;
  else
    return false;
}

void Manager::play() {
  SDL_Event event;

  bool done = false;
  bool keyCatch = false;
  bool keepTouch = false;//keep touching the keyboard

  while ( not done ) {

    SDL_PollEvent(&event);
    Uint8 *keystate = SDL_GetKeyState(NULL);
    //const Uint8 *state = SDL_GetKeyboardState(NULL);
    if(event.type ==  SDL_QUIT) { done = true; break; }
//-------------------- key UP function Add here--------------
    if(event.type == SDL_KEYUP) { // if anykey is up then see which key

      keepTouch = false;
      keyCatch = false;
      if( !keystate[SDLK_w] ){//release w but keep a or d 
        if(currentSprite == 0){
            if( keystate[SDLK_a] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
              keepTouch = true;
            }
            if( keystate[SDLK_d] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
              keepTouch = true;
            }
        }
      }

      if( !keystate[SDLK_s] ){//release s but keep a or d
       if(currentSprite == 0){
          static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(false);
          if( keystate[SDLK_a] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
              keepTouch = true;
            }
          if( keystate[SDLK_d] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
              keepTouch = true;
            }
        }
      }

      if( keystate[SDLK_w] && !keystate[SDLK_a] && !keystate[SDLK_d] ){//release a or d but keep w
        if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(false);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            keepTouch = false;
        }
      }

      if( keystate[SDLK_s] && !keystate[SDLK_a] && !keystate[SDLK_d] ){//release a or d but keep s
        if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(false);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            keepTouch = false;
        }
      }

      if( !keystate[SDLK_f] && !stopWatch ){//release f 
        if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsFist(false);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            stopWatch_End = clock.getSeconds();
            keepTouch = false;
            stopWatch = true;
            if (static_cast<Rayman *>(sprites[currentSprite]) -> getFaceDirection() == 1){
              sprites.push_back( new Bullet("singleFistRight",singlePostion) );
              static_cast<Bullet *>(sprites[singlePostion]) -> setFaceDirection(1);
              sprites[singlePostion] -> X( sprites[currentSprite] -> X() + 50);
              sprites[singlePostion] -> Y( sprites[currentSprite] -> Y() + 80);
              sprites[singlePostion] -> velocityX( static_cast<Bullet*>(sprites[singlePostion]) -> getSpeed() );
              sprites[singlePostion] -> velocityY( 0 );
              fistStartPos = singlePostion;
              ++singlePostion;
            }
            else{
              sprites.push_back( new Bullet("singleFistLeft",singlePostion) );
              static_cast<Bullet *>(sprites[singlePostion]) -> setFaceDirection(-1);//set the fist direction
              sprites[singlePostion] -> X( sprites[currentSprite] -> X() + 50);
              sprites[singlePostion] -> Y( sprites[currentSprite] -> Y() + 80);
              sprites[singlePostion] -> velocityX( -1 * static_cast<Bullet*>(sprites[singlePostion]) -> getSpeed() );
              sprites[singlePostion] -> velocityY( 0 );
              fistStartPos = singlePostion;
              ++singlePostion;
            }
        }
      }


    }
// ------------------ kEY Down Here------------- 
    if(event.type == SDL_KEYDOWN) {
      keepTouch = true; 

      if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
        done = true;
        break;
      }

      if (!keyCatch) //keyboard touch once function
      {
        switch (event.key.keysym.sym){
          case SDLK_t:
            currentSprite = (currentSprite+1) % sprites.size();
            viewport.setObjectToTrack(sprites[currentSprite]);
            break;
          case SDLK_l: clock.toggleSloMo();
            break;
          case SDLK_p:
            if ( clock.isPaused() ) clock.unpause();
            else clock.pause();
            break;
          case SDLK_F4:
            std::cout << "Making video frames" << std::endl;
            makeVideo = true;
            break;
          case SDLK_e:
            if (currentSprite > 0)
                sprites[currentSprite] -> setIsMoved(! sprites[currentSprite] -> getIsMoved());
            break;
          case SDLK_m:
            isShowHug = !isShowHug;
            break;
          case SDLK_w: 
            if(currentSprite == 0)
              static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            break;
          case SDLK_r:
            if(fistStartPos != 0){
              for (int i = pinkGearStartPos;i<fistStartPos;i++){
                Drawable *tmp = sprites[i];   
                sprites[i] = new pinkGear("pinkGear", i);
                delete tmp;
              }
            }
            else{
              for (int i = pinkGearStartPos;i<sprites.size();i++){
                Drawable *tmp = sprites[i];   
                sprites[i] = new pinkGear("pinkGear", i);
                delete tmp;
              }
            }
            //std::cout<<"reset"<<std::endl;
            break;
          default:
            break;
        }
        keyCatch = true;
      }

      if ( keystate[SDLK_a] &&  keepTouch) { //keep running to left 
        if (currentSprite == 0){
          static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
          static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
        }
      }

      if ( keystate[SDLK_d] &&  keepTouch) { //keep runing to right
        if (currentSprite == 0){
          static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
          static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
        }
      }
      if ( keystate[SDLK_s] && keepTouch) {//keep crawing position
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
          }
      }

      if ( keystate[SDLK_s] && keystate[SDLK_d] ) {//right crawing
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
          }
      }
      if ( keystate[SDLK_s] && keystate[SDLK_a] ) {//left crawing
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
          }
      }

      if ( keystate[SDLK_w] && keystate[SDLK_d] ) {//right jumping
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
          }
      }
      if ( keystate[SDLK_w] && keystate[SDLK_a] ) {//left jumping
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
          }
      }

      if ( keystate[SDLK_f] && keepTouch ) {//left jumping
          if (stopWatch){
            stopWatch_Begin = clock.getSeconds();
            stopWatch = false;
          }

          if( currentSprite == 0 ){
            static_cast<Rayman *>(sprites[currentSprite]) -> setFistDuration( clock.getSeconds() - stopWatch_Begin );
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsFist(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(false);
          }
      }
    }

    if (currentSprite == 0 && !keepTouch){
          static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
    }
    
    draw();
    update();
  }
}

bool Manager::stopGame(){
  for (int i = singlePostion ; i< (int)sprites.size(); i++){
    if (sprites[i]->getFrameFollower() == -1)
      return false;
  }
  return true;
}
