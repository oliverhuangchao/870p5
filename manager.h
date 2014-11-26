#include <vector>
#include <SDL.h>
#include "ioManager.h"
#include "clock.h"
#include "world.h"
#include "viewport.h"
#include "drawable.h"
#include "health.h"
#include "sound.h"
class Manager {
public:
  Manager ();
  ~Manager ();
  void play();
  bool spriteConflict( Drawable* multi, Drawable* single );
  bool stopGame();
private:
  const bool env;
  const IOManager& io;
  Clock& clock;

  SDL_Surface * const screen;
  
  World world_back;
  World world_front;
  Health bar;
  SDLSound sound;
  Viewport& viewport;

  std::vector<Drawable*> sprites;
  std::vector<Drawable*> usedVector;
  std::vector<Drawable*> freeVector;

  int currentSprite;

  bool makeVideo;
  bool eatStar;//let the program know that the eagle now eating stars
  bool isShowHug;
  int singlePostion;//when the singlePost will start
  //int conflictScale;

  bool stopWatch;
  float stopWatch_Begin;//begin a stopwatch
  float stopWatch_End;//end a stopwatch


  int fistStartPos;//the first position for fist
  int pinkGearStartPos;//the first position for pinkGear
  bool fistReadyToTurn;//fist ready to turn
  bool onceTouch;//the fist alrealy hit the pinkGear, so the one fist can just hit once

  bool unlimitedMode;//mode: the pinkGear will not disapear

  int frameCount;
  const std::string username;
  const std::string title;
  const int frameMax;

  void draw() const;
  void update();

  Manager(const Manager&);
  Manager& operator=(const Manager&);
  void makeFrame();

};
