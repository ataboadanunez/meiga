#include "Particle.h"

Particle::Particle(const int id, const double px,
                   const double py, const double pz,
                   const double x, const double y, const double z):
                  fId(id),
                  fPx(px),
                  fPy(py),
                  fPz(pz),
                  fX(x),
                  fY(y),
                  fZ(z)
                   { }

int
Particle::NucleusCode(const unsigned int charge,
                      const unsigned int atomicNumber)
  {
    return
        kNucleusBase
      + kChargeFactor       * charge
      + kAtomicNumberFactor * atomicNumber;
  }
