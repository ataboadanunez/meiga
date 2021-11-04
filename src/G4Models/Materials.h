#ifndef Materials_h
#define Materials_h 1

#include "globals.hh"
#include "G4Element.hh"
#include "G4ElementTable.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

class Materials {
	
	/* 
		this class is intended to hold all definitions and properties related to the different materials used in the Geant4 simulations
	

		author: alvaro taboada
	*/

public:
	Materials();
	virtual ~Materials();

	// single elements
	G4Element* elN = nullptr;
	G4Element* elO = nullptr;
	G4Element* elH = nullptr;
	G4Element* elC = nullptr;
	G4Element* elSi = nullptr;
	G4Element* elTi = nullptr;
	G4Element* elB  = nullptr;
	G4Element* elNa = nullptr;
	// compounds
	G4Material* SiO2 = nullptr;
	G4Material* TiO2 = nullptr;
	G4Material* B2O2 = nullptr;
	G4Material* Na2O = nullptr;
	G4Material* PPO = nullptr;
	G4Material* POPOP = nullptr;
	G4Material* Polystyrene = nullptr;

	// Materials
	G4Material* Air = nullptr;
	G4Material* StdRock = nullptr;
	G4Material* PlasticScint = nullptr;
	G4Material* ScintCoating = nullptr;
	G4Material* PMMA = nullptr;
	G4Material* Pethylene = nullptr;
	G4Material* FPethylene = nullptr;
	G4Material* Pyrex = nullptr;

	

private:

	void CreateElements(); // function to create individual elements
	void CreateCompunds(); // function to create compunds like polystyrene, standard rock, etc...
	void CreateMaterials();
	
	// material properties (scintillation, Cherenkov, optics)

	// array of photon energies in the red-UV range
	G4double photonEnergy[] = {
		2.00*eV, 2.03*eV, 2.06*eV, 2.09*eV, 2.12*eV,
		2.15*eV, 2.18*eV, 2.21*eV, 2.24*eV, 2.27*eV,
		2.30*eV, 2.33*eV, 2.36*eV, 2.39*eV, 2.42*eV,
		2.45*eV, 2.48*eV, 2.51*eV, 2.54*eV, 2.57*eV,
		2.60*eV, 2.63*eV, 2.66*eV, 2.69*eV, 2.72*eV,
		2.75*eV, 2.78*eV, 2.81*eV, 2.84*eV, 2.87*eV,
		2.90*eV, 2.93*eV, 2.96*eV, 2.99*eV, 3.02*eV,
		3.05*eV, 3.08*eV, 3.11*eV, 3.14*eV, 3.17*eV,
		3.20*eV, 3.23*eV, 3.26*eV, 3.29*eV, 3.32*eV,
		3.35*eV, 3.38*eV, 3.41*eV, 3.44*eV, 3.47*eV
  };

  G4int n = sizeof(photonEnergy)/sizeof(G4double);
  
  G4double refIndexScin[] = {
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50,
		1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50, 1.50
  };





};



#endif