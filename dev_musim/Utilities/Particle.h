#ifndef Particle_h
#define Particle_h 1

#include <iostream>
#include <string>

// start using namespaces?

class Particle {

public:

	// useful definitions for nucleus code calculation
	static const int kNucleusBase        = 1000000000;
  static const int kNucleusMax         = 1000100300;
  static const int kAtomicNumberFactor = 1;
  static const int kAtomicNumberMask   = 1000;
  static const int kChargeFactor       = 1000;
  static const int kChargeMask         = 1000000;

  // PDG encoding
	enum Type {
			eUndefined = 0,
      eElectron = 11, ePositron = -11,
      eNuElectron = 12, eAntiNuElectron = -12,
      eMuon = 13, eAntiMuon = -13,
      eNuMuon = 14, eAntiNuMuon = -14,
      eTau = 15, eAntiTau = -15,
      eNuTau = 16, eAntiNuTau = -16,
      ePhoton = 22,
      // PDG codes 81-100 are reserved for special/pseudo particles
      eDecayedMuon = 96, eDecayedAntiMuon = 95,
      ePiZero = 111,
      eRhoZero = 113,
      ePiPlus = 211, ePiMinus = -211,
      eRhoPlus = 213, eRhoMinus = -213,
      eEta = 221,
      eOmegaMeson = 223,
      eKaon0L = 130, eKaon0S = 310,
      eKaonStarZero = 313, eAntiKaonStarZero = -313,
      eKaonPlus = 321, eKaonMinus = -321,
      eKaonStarPlus = 323, eAntiKaonStarMinus = -323,
      ePhi = 333,
      eDeltaMinus = 1114, eAntiDeltaPlus = -1114,
      eNeutron = 2112, eAntiNeutron = -2112,
      eDeltaZero = 2114, eAntiDeltaZero = -2114,
      eProton = 2212, eAntiProton = -2212,
      eDeltaPlus = 2214, eAntiDeltaMinus = -2214,
      eDeltaPlusPlus = 2224, eAntiDeltaMinusMinus = -2224,
      eLambda = 3122, eAntiLambda = -3122,
      eSigmaPlus = 3222, eSigmaZero = 3212, eSigmaMinus = 3112,
      eAntiSigmaPlus = -3222, eAntiSigmaZero = -3212, eAntiSigmaMinus = -3112,
      eXiZero = 3322, eXiMinus = 3312,
      eAntiXiZero = -3322, eAntiXiMinus = -3312,
      eOmegaMinus = 3332, eAntiOmegaMinus = -3332,
      eLambdac = 4122,
      eIron = 1000026056
  };

  /// Constructor using momentum
  Particle() {;}
  Particle(const int id, const double px, const double py, const double pz, const double x, const double y, const double z);
  /*
  Particle(const int type, 
  				 const Source& source, const Point& position,
           const Vector& momentum, const TimeInterval& time,
           const double weight);
	*/

  /// Calculate particle code for a nucleus given (A, Z)
  static int NucleusCode(const unsigned int theCharge, const unsigned int theAtomicNumber);
  ////Getters
  int GetParticleId() const { return fId; }
  double GetParticleMomentumX() const { return fPx; }
  double GetParticleMomentumY() const { return fPy; }
  double GetParticleMomentumZ() const { return fPz; }
  double GetParticlePositionX()  const { return fX; }
  double GetParticlePositionY()  const { return fY; }
  double GetParticlePositionZ()  const { return fY; }
  private:
  int fId;
  double fPx, fPy, fPz, fX, fY, fZ;
};

#endif 
