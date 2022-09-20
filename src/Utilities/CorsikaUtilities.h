// class intended to store useful functions related to Geant4
#ifndef CorsikaUtilities_h
#define CorsikaUtilities_h 1
#include "Particle.h"

namespace Corsika {
	// converters from CORSIKA to PDG particle codes

	Particle::Type CorsikaToPDG(const int corsikaCode);
}

#endif