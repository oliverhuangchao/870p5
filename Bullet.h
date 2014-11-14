#ifndef BULLET__H
#define BULLET__H
#include <string>
#include "drawable.h"

class Bullet : public Drawable {
public:
  Bullet(const std::string&,const int);
  Bullet(const std::string&, const Vector2f& pos, const Vector2f& vel, const int);
  Bullet(const std::string&, 
         const Vector2f& pos, const Vector2f& vel, const Frame*,const int);
  Bullet(const Bullet& s);
  virtual ~Bullet() { } 
  Bullet& operator=(const Bullet&);

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;

  virtual void update(Uint32 ticks);

  virtual void update(Uint32 ticks, Drawable *BrotherBullet);


  float getFistRange()const{return fistRange;}
  void setFistRange(float x){fistRange = x;}
private:
  const Frame * frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;

  float fistRange;//fist range

  int getDistance(const Bullet*) const;
};
#endif