#ifndef Pixel_h
#define Pixel_h 1

#include "SiPM.h"
#include <vector>

class Pixel
{
  public:
    Pixel() { };
    Pixel(const unsigned int id);
    virtual ~Pixel();

  private:
    unsigned int fPixelId = 0;
  
};

#endif