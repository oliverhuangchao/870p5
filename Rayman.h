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
  //int getFrameDirection()const{return frameDirection;}
 // void setFrameDirection(int x){frameDirection = x;}

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


  // Rayman action
  bool alreadyCrawling;
  float currentLandPos;
  bool currentLandPosSet;
  bool isReachTop;
  //bool isCrawl;// whether rayman is crawling

  std::string frameName;

  void advanceFrame(Uint32 ticks);
};
#endif
