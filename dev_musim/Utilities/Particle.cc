#include "Particle.h"

int
Particle::NucleusCode(const unsigned int charge,
                      const unsigned int atomicNumber)
  {
    return
      	kNucleusBase
      + kChargeFactor       * charge
      + kAtomicNumberFactor * atomicNumber;
  }