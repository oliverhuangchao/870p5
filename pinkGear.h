#ifndef pinkGear__H
#define pinkGear__H
#include <string>
#include "drawable.h"
//#include "chunk.h"
#include <list>

class pinkGear : public Drawable {
public:
  pinkGear(const std::string&,const int);
  pinkGear(const std::string&, const Vector2f& pos, const Vector2f& vel, const int);
  pinkGear(const std::string&, const Vector2f& pos, const Vector2f& vel, const Frame*,const int);
  pinkGear(const pinkGear& s);
  virtual ~pinkGear();
  pinkGear& operator=(const pinkGear&);

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual void update(Uint32 ticks, Drawable *BrotherpinkGear);

  int getMoveRange() const {return moveRange;}
  void setMoveRange(int x){moveRange = x;}
  int getDistance(const pinkGear*) const;

  bool getAlreadyHit() const {return alreadyHit;}
  void setAlreadyHit(bool x){alreadyHit = x;}
private:
  const Frame * frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
  int moveRange;
  int startX;
  int startY;
  bool alreadyHit;
  // the explosion part of this sprite
  //std::list<Chunk> chunks; // An ExplodingSprite is a list of sprite chunks
  //std::list<Chunk> freeList; // When a chunk gets out of range it goes here
};
#endif
