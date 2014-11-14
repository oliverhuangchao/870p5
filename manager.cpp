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
isShowHug ( false ),
singlePostion( 0 ),
conflictScale( Gamedata::getInstance().getXmlInt("conflict") ),
stopWatch(true),
stopWatch_Begin(0),
stopWatch_End(0),
fistStartPos(0),

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

    //set the pinkGears
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

    io.printMessageAt("press 's' to crawling", 20, linePos);
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
  //std::cout<<clock.getSeconds()<<std::endl;
  //if (currentSprite == 0)
    
  Uint32 ticks = clock.getElapsedTicks();
  
  for (unsigned int i = 0; i < sprites.size(); ++i){ //the fist sprite should update another sprite
  //  sprites[i]->update(ticks);
    if (i >= fistStartPos && fistStartPos !=0)
      sprites[i]->update(ticks, sprites[0]);//fist update should added with
    else {
      sprites[i]->update(ticks);
      /*if (sprites[i]->X() == sprites[0]->X()) //still something wrong with it
        sprites.erase(sprites);*/
    }

  }

  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }


  viewport.update(); 
}

bool Manager::spriteConflict( Drawable* multi, Drawable* single ){
  if ( getDistance(multi->X()+multi->getFrameWidth(),multi->Y()+multi->getFrameHeight(),single->X(),single->Y(),conflictScale) && !single->getCatched() )
  {
    single->setCatched(true);
    single->setFrameFollower(multi->getFrameNumber());
    multi->setTotalFollowers(multi->getTotalFollowers()+1);
    return true;
  }
  else
    return false;
}

bool getDistance(int a1,int a2, int b1, int b2, int value){
  int result = sqrt( pow( (a1 - b1), 2 ) + pow( ( a2 - b2 ), 2 ) );
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
//-------------------- key up --------------
    if(event.type == SDL_KEYUP) { // if anykey is up then see which key

      keepTouch = false;
      keyCatch = false;
      if( !keystate[SDLK_w] ){//release w but keep a or d 
        if(currentSprite == 0){
            if( keystate[SDLK_a] ){
              sprites[currentSprite] -> setFrameDirection(-1);
              sprites[currentSprite] -> setFaceDirection(-1);
              keepTouch = true;
            }
            if( keystate[SDLK_d] ){
              sprites[currentSprite] -> setFrameDirection(1);
              sprites[currentSprite] -> setFaceDirection(1);
              keepTouch = true;
            }
        }
      }

      if( !keystate[SDLK_s] ){//release s but keep a or d
       if(currentSprite == 0){
          sprites[currentSprite] -> setIsCrawl(false);
          if( keystate[SDLK_a] ){
              sprites[currentSprite] -> setFrameDirection(-1);
              sprites[currentSprite] -> setFaceDirection(-1);
              keepTouch = true;
            }
          if( keystate[SDLK_d] ){
              sprites[currentSprite] -> setFrameDirection(1);
              sprites[currentSprite] -> setFaceDirection(1);
              keepTouch = true;
            }
        }
      }

      if( keystate[SDLK_w] && !keystate[SDLK_a] && !keystate[SDLK_d] ){//release a or d but keep w
        if(currentSprite == 0){
            sprites[currentSprite] -> setIsJump(true);
            sprites[currentSprite] -> setIsCrawl(false);
            sprites[currentSprite] -> setFrameDirection(0);
            keepTouch = false;
        }
      }

      if( keystate[SDLK_s] && !keystate[SDLK_a] && !keystate[SDLK_d] ){//release a or d but keep s
        if(currentSprite == 0){
            sprites[currentSprite] -> setIsCrawl(true);
            sprites[currentSprite] -> setIsJump(false);
            sprites[currentSprite] -> setFrameDirection(0);
            keepTouch = false;
        }
      }

      if( !keystate[SDLK_f] && !stopWatch ){//release f 
        if(currentSprite == 0){
            sprites[currentSprite] -> setIsFist(false);
            sprites[currentSprite] -> setFrameDirection(0);
            stopWatch_End = clock.getSeconds();
            keepTouch = false;
            stopWatch = true;
            //std::cout<<stopWatch_End - stopWatch_Begin<<std::endl;
            if (sprites[currentSprite] -> getFaceDirection() == 1){
              sprites.push_back( new Bullet("singleFistRight",singlePostion) );
              sprites[singlePostion] -> X( sprites[currentSprite] -> X() + 100);
              sprites[singlePostion] -> Y( sprites[currentSprite] -> Y() + 50);
              sprites[singlePostion] -> velocityX( 500 );
              sprites[singlePostion] -> velocityY( 0 );
              fistStartPos = singlePostion;
              ++singlePostion;
            }
            else{
              // equal to -1
              sprites.push_back( new Bullet("singleFistLeft",singlePostion) );
              sprites[singlePostion] -> X( sprites[currentSprite] -> X() + 100);
              sprites[singlePostion] -> Y( sprites[currentSprite] -> Y() + 50);
              sprites[singlePostion] -> velocityX( -500 );
              sprites[singlePostion] -> velocityY( 0 );
              fistStartPos = singlePostion;
              ++singlePostion;
            }
        }
      }


    }
// ------------------ key down ------------- 
    if(event.type == SDL_KEYDOWN) {
      
      keepTouch = true; 

      if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
        done = true;
        break;
      }

      if (keystate[SDLK_t] && !keyCatch) {
        keyCatch = true;
        currentSprite = (currentSprite+1) % sprites.size();
        viewport.setObjectToTrack(sprites[currentSprite]);
      }

      if (keystate[SDLK_l] && !keyCatch) {
        keyCatch = true;
        clock.toggleSloMo();
      }

      if (keystate[SDLK_p] && !keyCatch) {
        keyCatch = true;
        if ( clock.isPaused() ) clock.unpause();
        else clock.pause();
      }

      if (keystate[SDLK_F4] && !makeVideo) {
        std::cout << "Making video frames" << std::endl;
        makeVideo = true;
      }

      if(keystate[SDLK_e]  && !keyCatch) {// let the gear move
        if (currentSprite > 0){
          sprites[currentSprite] -> setIsMoved(! sprites[currentSprite] -> getIsMoved());
          keyCatch = true;
        }
      }
      if ( keystate[SDLK_m] &&  !keyCatch) { //show the huds 
        keyCatch = true;
        isShowHug = !isShowHug;
      }
      
      if ( keystate[SDLK_a] &&  keepTouch) { //keep running to left 
        if (currentSprite == 0){
          sprites[currentSprite] -> setFrameDirection(-1);
          sprites[currentSprite] -> setFaceDirection(-1);
        }
      }

      if ( keystate[SDLK_d] &&  keepTouch) { //keep runing to right
        if (currentSprite == 0){
          sprites[currentSprite] -> setFrameDirection(1);
          sprites[currentSprite] -> setFaceDirection(1);
        }
      }

      if ( keystate[SDLK_w] && !keyCatch) {// jump once
          keyCatch = true;
          if(currentSprite == 0){
            sprites[currentSprite] -> setIsJump(true);
          }
      }

      if ( keystate[SDLK_s] && keepTouch) {//keep crawing position
          if(currentSprite == 0){
            sprites[currentSprite] -> setIsCrawl(true);
          }
      }

      if ( keystate[SDLK_s] && keystate[SDLK_d] ) {//right crawing
          if(currentSprite == 0){
            sprites[currentSprite] -> setIsCrawl(true);
            sprites[currentSprite] -> setFrameDirection(1);
            sprites[currentSprite] -> setFaceDirection(1);
          }
      }
      if ( keystate[SDLK_s] && keystate[SDLK_a] ) {//left crawing
          if(currentSprite == 0){
            sprites[currentSprite] ->setIsCrawl(true);
            sprites[currentSprite] -> setFrameDirection(-1);
            sprites[currentSprite] -> setFaceDirection(-1);
          }
      }

      if ( keystate[SDLK_w] && keystate[SDLK_d] ) {//right jumping
          if(currentSprite == 0){
            sprites[currentSprite] -> setIsJump(true);
            sprites[currentSprite] -> setFrameDirection(1);
            sprites[currentSprite] -> setFaceDirection(1);
          }
      }
      if ( keystate[SDLK_w] && keystate[SDLK_a] ) {//left jumping
          if(currentSprite == 0){
            sprites[currentSprite] ->setIsJump(true);
            sprites[currentSprite] -> setFrameDirection(-1);
            sprites[currentSprite] -> setFaceDirection(-1);
          }
      }

      if ( keystate[SDLK_w] && keystate[SDLK_a] ) {//left jumping
          if(currentSprite == 0){
            sprites[currentSprite] ->setIsJump(true);
            sprites[currentSprite] -> setFrameDirection(-1);
            sprites[currentSprite] -> setFaceDirection(-1);
          }
      }
      if ( keystate[SDLK_f] && keepTouch ) {//left jumping
          if (stopWatch){
            stopWatch_Begin = clock.getSeconds();
            stopWatch = false;
          }

          if( currentSprite == 0 ){
            sprites[currentSprite] -> setFistDuration( clock.getSeconds() - stopWatch_Begin );
            sprites[currentSprite] -> setFrameDirection(0);
            sprites[currentSprite] -> setIsFist(true);
            sprites[currentSprite] -> setIsCrawl(false);
          }
      }
    }
    if (currentSprite == 0 && !keepTouch){
          sprites[currentSprite] -> setFrameDirection(0);
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
