#ifndef ReadParticleFile_h
#define ReadParticleFile_h

#include "Event.h"
#include <string>

/* 
	Declaration of ReadParticleFile class.
	This class is intended to hold methods / functions 
	related to read the input file with particles information.

	Author: Alvaro Taboada
*/

class Event;

class ReadParticleFile 
{
  public:
	  ReadParticleFile();
	  virtual ~ReadParticleFile();

    static Event EventFileReader(const std::string &fileName);




};

#endif