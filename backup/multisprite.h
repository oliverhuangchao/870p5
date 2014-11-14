#ifndef MULTISPRITE__H
#define MULTISPRITE__H
#include <string>
#include <vector>
#include "drawable.h"

class MultiSprite : public Drawable {
public:
  MultiSprite(const std::string&, const int );
  MultiSprite(const MultiSprite&);
  virtual ~MultiSprite() { } 

  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual void update(Uint32 ticks,Drawable *);
  virtual const Frame* getFrame() const { 
    return frames[currentFrame]; 
  }

/*void setHungry(const bool x){frameHungry = x;}

bool getHungry() const {return frameHungry;}*/
std::string getFrameName()const {return frameName;}


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




  //bool frameHungry;
  std::string frameName;

  void advanceFrame(Uint32 ticks);
};
#endif
