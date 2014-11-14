#include "frameFactory.h"
#include "extractSurface.h"
#include "ioManager.h"
#include "vector2f.h"

FrameFactory::~FrameFactory() {
  std::cout << "Deleting FrameFactory" << std::endl;
  std::map<std::string, SDL_Surface*>::iterator itSurf = surfaces.begin();
  while ( itSurf != surfaces.end() ) {
    SDL_FreeSurface( itSurf->second );
    ++itSurf;
  }
  std::map<std::string, std::vector<SDL_Surface*> >::iterator surfaces = multiSurfaces.begin();
  while ( surfaces != multiSurfaces.end() ) {
    for (unsigned int i = 0; i < surfaces->second.size(); ++i) {
      SDL_FreeSurface( surfaces->second[i] );
    }
    ++surfaces;
  }
  std::map<std::string, Frame*>::iterator frame = frames.begin();
  while ( frame != frames.end() ) {
    delete frame->second;
    ++frame;
  }

  std::map<std::string, std::vector<Frame*> >::iterator frames = multiFrames.begin();
  while ( frames != multiFrames.end() ) {
    for (unsigned int i = 0; i < frames->second.size(); ++i) {
      delete frames->second[i];
    }
    ++frames;
  }
}

FrameFactory& FrameFactory::getInstance() {
  static FrameFactory factory;
  return factory;
}

Frame* FrameFactory::getFrame(const std::string& name) {
  std::map<std::string, Frame*>::const_iterator pos = frames.find(name); 
  if ( pos == frames.end() ) {
    SDL_Surface * const surface =
      IOManager::getInstance().loadAndSet(
          gdata.getXmlStr(name+"/file"),
          gdata.getXmlBool(name+"/transparency"));
    surfaces[name] = surface;
    Frame * const frame =new Frame(name, surface);
    frames[name] = frame;
    return frame;
  }
  else {
    return pos->second;
  }
}


std::vector<Frame*> FrameFactory::getFrames(const std::string& name, const std::string& secondName) {
  // First search map to see if we've already made it:
  std::map<std::string, std::vector<Frame*> >::const_iterator 
    pos = multiFrames.find(name+secondName); 
  if ( pos != multiFrames.end() ) {
    return pos->second;
  }

  // It wasn't in the map, so we have to make the vector of Frames:
  SDL_Surface* surface = IOManager::
  getInstance().loadAndSet(gdata.getXmlStr(name+"/"+secondName+"/file"), true);
  unsigned numberOfFrames = gdata.getXmlInt(name+"/"+secondName+"/frames");
  std::vector<Frame*> frames;
  std::vector<SDL_Surface*> surfaces;
  //frames.reserve(numberOfFrames);
  Uint16 srcX = gdata.getXmlInt(name+"/srcX");
  Uint16 srcY = gdata.getXmlInt(name+"/srcY");
  Uint16 width = gdata.getXmlInt(name+"/"+secondName+"/width");
  Uint16 height = gdata.getXmlInt(name+"/"+secondName+"/height");

  SDL_Surface* surf;
  frames.clear();
  for (unsigned i = 0; i < 30; ++i) {//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    unsigned frameX = (i%numberOfFrames) * width + srcX;
   surf = ExtractSurface::getInstance().
               get(surface, width, height, frameX, srcY); 
    surfaces.push_back( surf );
    frames.push_back( new Frame(name, secondName, surf) );
  }
  SDL_FreeSurface(surface);
  multiSurfaces[name+secondName] = surfaces;
  multiFrames[name+secondName] = frames;
  return frames;
}



// if the frame want to change size use this function
std::vector<Frame*> FrameFactory::changeFrames(const std::string& name, const std::string& secondName, double zoomInteger) {
  // First search map to see if we've already made it:

  std::map<std::string, std::vector<Frame*> >::iterator pos = multiFrames.find(name
    + secondName);
  if ( pos != multiFrames.end()) {
        return pos->second;
  }
 
  // It wasn't in the map, so we have to make the vector of Frames:
  SDL_Surface* surface = IOManager::
     getInstance().loadAndSet(gdata.getXmlStr(name+"/"+secondName+"/file"), true);
  unsigned numberOfFrames = gdata.getXmlInt(name+"/"+secondName+"/frames");

  std::vector<Frame*> frames;
  std::vector<SDL_Surface*> surfaces;
  frames.reserve(numberOfFrames);

  Uint16 srcX = gdata.getXmlInt(name+"/srcX");
  Uint16 srcY = gdata.getXmlInt(name+"/srcY");
  Uint16 width = gdata.getXmlInt(name+"/"+secondName+"/width");
  Uint16 height = gdata.getXmlInt(name+"/"+secondName+"/height");

  SDL_Surface* surf;
  SDL_Surface* tmp_surf;

  for (unsigned i = 0; i < numberOfFrames; ++i) {
    unsigned frameX = i * width + srcX;

    surf = ExtractSurface::getInstance().
                 get(surface, width, height, frameX, srcY); 
    
    tmp_surf = zoomSurface(surf, zoomInteger, zoomInteger, SMOOTHING_ON);
    surfaces.push_back( tmp_surf );//newSurf
    frames.push_back( new Frame(name, secondName,  tmp_surf, zoomInteger) );//draw
    SDL_FreeSurface(surf);
  }


  SDL_FreeSurface(surface);
  
  multiSurfaces[name+secondName] = surfaces;
  multiFrames[name+secondName] = frames;
  return frames;
}
