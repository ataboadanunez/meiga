#ifndef Particle_h
#define Particle_h 1

#include <iostream>
#include <string>
#include <map>
#include <vector>
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

		enum Component {
			eUnknown = 0,
			eElectromagnetic = 1,
			eMuonic = 2,
			eHadronic = 3,
			eMuonDecay = 4,
			// for iterating over enum elements
			eEnd
		};

		Particle() { ; }
		// constructor using momentum
		Particle(int id, std::vector<double>& position, std::vector<double>& momentum);

		int GetParticleId() { return fId; }
		void SetParticleId(const int id) { fId = id; }
		// calculate particle code for a nucleus given (A, Z)
		static int NucleusCode(const unsigned int theCharge, const unsigned int theAtomicNumber);

		// type and components
		void InsertComponent(const Particle::Type type, const Particle::Component comp);
		void InitComponentMap();
		Particle::Component GetComponent(const Particle::Type type);
		std::string GetComponentName(Particle::Component type);

		// particle momentum
		const std::vector<double>& GetDirection() const { return fDirection; }
		void SetDirection(const std::vector<double>& direction) { fDirection = direction; }
		void SetMomentum(const std::vector<double>& momentum);
		double GetMomentum() const { return fMomentum; }
		
		// particle mass
		void InsertParticleMass(const Particle::Type id, const double mass);
		static void InitParticleMassMap();
		void SetMass(const int id);
		double GetMass() const { return fMass; }	
		
		// particle energy
		void SetKineticEnergy(const double ke) { fKineticEnergy = ke; }
		double GetKineticEnergy() { return fKineticEnergy; }
		double GetTotalEnergy() const { return fKineticEnergy + GetMass(); }
		void SetTotalEnergy(const double totE) { fKineticEnergy = totE - GetMass(); }
		
		// particle position
		std::vector<double>& GetParticlePosition() { return fPosition; }
		// CAVEAT: this sould be part of Detector SimData
		// particle injection position
		const std::vector<double>& GetInjectionPosition() const { return fInjectionPosition; }
		void SetInjectionPosition(const std::vector<double>& injPos) { fInjectionPosition = injPos; }

		void SetZenith(const double zenith) { fZenith = zenith; }
		double GetZenith() const { return fZenith; }
		void SetAzimuth(const double azimuth) { fAzimuth = azimuth; }
		double GetAzimuth() const { return fAzimuth; }



	private:
 		
 		int fId;
 		double fMass = 0;
 		double fMomentum = 0;
 		double fKineticEnergy = 0;
 		double fZenith = 0;
 		double fAzimuth = 0;
 		
 		std::vector<double> fPosition;
  	std::vector<double> fDirection;
  	std::vector<double> fInjectionPosition;
  	static std::map<int, double> gParticleMassMap;
  	static std::map<Particle::Type, Particle::Component> gComponentMap;
		// particle properties for mass
};

#endif 
