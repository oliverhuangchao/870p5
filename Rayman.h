#ifndef Rayman__H
#define Rayman__H
#include <string>
#include <vector>
#include "drawable.h"

class Rayman : public Drawable {
public:
  Rayman(const std::string&, const std::string&, const int );
  Rayman(const Rayman&);
  virtual ~Rayman() { } 

  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual void update(Uint32 ticks,Drawable *);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }

  std::string getFrameName()const {return frameName;}
  inline void jumpFunction();
  

  void setIsCrawl(bool x){isCrawl = x;}

  void setIsJump(bool x){isJump = x;}

  void setFrameDirection(int x){frameDirection = x;}

  void setFaceDirection(int x){faceDirection = x;}

  int getFaceDirection() {return faceDirection;}

  void setIsFist(bool x){isFist = x;}

  void setFistDuration(float x){fistDuration = x;}
  

protected:
  std::vector<Frame *> frames;
  int worldWidth;
  int worldHeight;

  unsigned currentFrame;
  unsigned numberOfFrames;
  unsigned frameInterval;
  float timeSinceLastFrame;
  int frameWidth;
  int frameHeight;


  // -------------Rayman action variable----------------
  bool alreadyCrawling;
  float currentLandPos;
  bool currentLandPosSet;
  bool isReachTop;
  bool isCrawl;// whether rayman is alreadyCrawling
  bool isJump;
  int frameDirection;//set the current direction of rayman
      // 1 : right
      // -1: left
      // 0 : stay

  int faceDirection;// where is his face is
      // 1 : right
      // -1: left
  bool isFist;//whether rayman is fisting
  float fistDuration;//fist preparation time


  std::string frameName;

  void advanceFrame(Uint32 ticks);
};
#endif
