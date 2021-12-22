#include "Materials.h"
// material properties (scintillation, Cherenkov, optics)

// array of photon energies in the red-UV range for scintillation processes
static G4double scinPhotonEnergy[] = {
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

G4int scinArrEntries = sizeof(scinPhotonEnergy)/sizeof(G4double);

static G4double scinRefIndex[] = {
	1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
	1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
	1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
	1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5,
	1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5, 1.5
	};

static G4double scinAbsLen[] = {
	4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 
	4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 
	4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 
	24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 
	24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 
	24.0*cm, 24.0*cm
  };

static G4double scinFastComp[] = {
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
	};

static G4double scinSlowComp[] = {
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};

static G4double scinOptSurfRefIndex[] = {
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
	1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
  };

static G4double scinOptSurfEff[] = {
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
	0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
  };

static G4double pmmaRefIndex[] = {
	1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
	1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
	1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
	1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
	1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60
  };

static G4double pmmaAbsLen[] = {
	10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m,
	10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m,
	10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 1.0*m,
	0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m,
	0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m
  };

static G4double pmmaEmission[] = {
	0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
	3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
	12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
	15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
	0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
  };


static G4double pethyRefIndex[] = {
	1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
	1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49
	};

static G4double pethyAbsLen[] = {
	20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
	20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
	20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
	20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
	20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m
	};

static G4double fpethyRefIndex[] = {
	1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
	1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42
	};

static G4double pmtPhotonEnergy[] = {2.00*eV, 4.00*eV};
static G4double pmtRefIndex[] = {1.47, 1.47};
G4int pmtArrEntries = sizeof(pmtPhotonEnergy) / sizeof(G4double);

static bool isInitialized = 0;
G4Element* Materials::elN;
G4Element* Materials::elO;
G4Element* Materials::elH;
G4Element* Materials::elC;
G4Element* Materials::elSi;
G4Element* Materials::elTi;
G4Element* Materials::elB;
G4Element* Materials::elNa;
	
G4Material* Materials::SiO2;
G4Material* Materials::TiO2;
G4Material* Materials::B2O2;
G4Material* Materials::Na2O;
G4Material* Materials::PPO;
G4Material* Materials::POPOP;
G4Material* Materials::Polystyrene;

// Materials
G4Material* Materials::Air;
G4Material* Materials::StdRock;
G4Material* Materials::ScinPlastic;
G4Material* Materials::ScinCoating;
G4Material* Materials::PMMA;
G4Material* Materials::Pethylene;
G4Material* Materials::FPethylene;
G4Material* Materials::Pyrex;
G4OpticalSurface* Materials::ScinOptSurf;

Materials::Materials() 
{
	if (isInitialized) {
		return;
	}
	
	isInitialized = true;
	
	CreateElements();
	CreateCompounds();
	CreateMaterials();

}

Materials::~Materials()
{

}

const G4double* 
Materials::GetScinPhotonEnergyArray() const { return scinPhotonEnergy; }

const G4double*
Materials::GetScinRefIndexArray() const { return scinRefIndex; }

void
Materials::CreateElements() 
{
	
	elN  = new G4Element("Nitrogen", "N", 7, 14.01 * g/mole);
	elO  = new G4Element("Oxygen", "O", 8, 16.00 * g/mole);
	elH  = new G4Element("Hydrogen", "H", 1, 1.01 * g/mole);
	elC  = new G4Element("Carbon", "C", 6, 12.0107 * g/mole);
	elSi = new G4Element("Silicon", "Si", 14, 28.09 * g/mole);
	elTi = new G4Element("Titanium", "Ti", 22, 47.867 * g/mole);
	elB  = new G4Element("Boron", "B", 5, 10.811 * g/mole);
	elNa = new G4Element("Sodium", "Na", 11, 22.98977 * g/mole);

}

void
Materials::CreateCompounds()
{
	// silicon dioxide for "quartz"
	SiO2 = new G4Material("SiO2", 2.65 * g/cm3, 2);
	SiO2->AddElement(elSi, 1);
	SiO2->AddElement(elO, 2);
	// titanium dioxide
	TiO2 = new G4Material("TiO2", 4.26 * g/cm3, 2);
	TiO2->AddElement(elTi, 1);
	TiO2->AddElement(elO, 2);
	// diboron dioxide
	B2O2 = new G4Material("B2O2", 2.23 * g/cm3, 2);
	B2O2->AddElement(elB, 2);
	B2O2->AddElement(elO, 2);
	// sodium oxide
	Na2O = new G4Material("Na2O", 2.23 * g/cm3, 2);
	Na2O->AddElement(elNa, 2);
	Na2O->AddElement(elO, 1);

	// Polystyrene for the scintillator bars Dow STYRON 663 W
  Polystyrene = new G4Material("Polystrene", 1.04 * g/cm3, 2);
  Polystyrene->AddElement(elC, 8);
  Polystyrene->AddElement(elH, 8);

	// dopant materials for scintillator bars
  /* Scintillator made of Polystyrene ( C6H5-CH-CH2 ) + PPO + POPOP
      Plastic:
      Is a replication on an aromatic ring (C6H5) with one out of the six H
      replaced by a CH bond with another CH2.
								CH2
                //
                CH
                |
               / \
       C6H5:  |   |
               \ /
  */

  // PPO:
	PPO = new G4Material("PPO", 1.060 * g/cm3, 4);
	PPO->AddElement(elC, 15);
	PPO->AddElement(elH, 11);
	PPO->AddElement(elN, 1);
	PPO->AddElement(elO, 1);
	// POPOP:
	POPOP = new G4Material("POPOP", 1.060 * g/cm3, 4);
	POPOP->AddElement(elC, 24);
	POPOP->AddElement(elH, 16);
	POPOP->AddElement(elN, 2);
	POPOP->AddElement(elO, 2);


}

void
Materials::CreateMaterials()
{

	// --------------------------------------------------------------------
	// Air
	// --------------------------------------------------------------------
	Air = new G4Material("Air", 1.29e-3 * g/cm3, 2);
	Air->AddElement(elN, 0.7);
	Air->AddElement(elO, 0.3);

	// --------------------------------------------------------------------
	// Rocks
	// --------------------------------------------------------------------
	StdRock = SiO2;

	// --------------------------------------------------------------------
	// Plastic Scintillator Bars
	// --------------------------------------------------------------------

	// Scintillator material:
	ScinPlastic = new G4Material("ScinPlastic", 1.060 * g/cm3, 3);
	ScinPlastic->AddMaterial(Polystyrene, 98.7*perCent);
	ScinPlastic->AddMaterial(PPO, 1*perCent);
	ScinPlastic->AddMaterial(POPOP, 0.3*perCent);

	// Scintillator Coating - 15% TiO2 and 85% polystyrene by weight.
	ScinCoating = new G4Material("ScinCoating", 1.52 * g/cm3, 2);
	ScinCoating->AddMaterial(TiO2, 15*perCent);
	ScinCoating->AddMaterial(Polystyrene, 85*perCent);

	// Add scintillator property table
	G4MaterialPropertiesTable* scinPT = new G4MaterialPropertiesTable();
	scinPT->AddProperty("RINDEX", scinPhotonEnergy, scinRefIndex, scinArrEntries);
  scinPT->AddProperty("ABSLENGTH", scinPhotonEnergy, scinAbsLen, scinArrEntries);
  scinPT->AddProperty("FASTCOMPONENT", scinPhotonEnergy, scinFastComp, scinArrEntries);
  scinPT->AddProperty("SLOWCOMPONENT", scinPhotonEnergy, scinSlowComp, scinArrEntries);
  scinPT->AddConstProperty("SCINTILLATIONYIELD", fScintillationYield);
  scinPT->AddConstProperty("RESOLUTIONSCALE", 1);
  scinPT->AddConstProperty("FASTTIMECONSTANT", fScinFastTime);
  scinPT->AddConstProperty("SLOWTIMECONSTANT", fScinSlowTime);
  scinPT->AddConstProperty("YIELDRATIO", 1);
  ScinPlastic->SetMaterialPropertiesTable(scinPT);

  // Add optical skin properties of scintillator (coating)
  ScinOptSurf = new G4OpticalSurface("ScinOptSurf", glisur, ground, dielectric_metal, fExtrusionPolish);

  G4MaterialPropertiesTable* scinOptSurfPT = new G4MaterialPropertiesTable();
  scinOptSurfPT->AddProperty("REFLECTIVITY", scinPhotonEnergy, scinOptSurfRefIndex, scinArrEntries);
  scinOptSurfPT->AddProperty("EFFICIENCY", scinPhotonEnergy, scinOptSurfEff, scinArrEntries);
  ScinOptSurf->SetMaterialPropertiesTable(scinOptSurfPT);

  // --------------------------------------------------------------------
	// PMMA (for WLS fibers)
	// --------------------------------------------------------------------
  

	PMMA = new G4Material("PMMA", 1.190 * g/cm3, 3);
	PMMA->AddElement(elC, 5);
	PMMA->AddElement(elH, 8);
	PMMA->AddElement(elO, 2);

	// Add properties to table
	G4MaterialPropertiesTable* pmmaPT = new G4MaterialPropertiesTable();
	pmmaPT->AddProperty("RINDEX", scinPhotonEnergy, pmmaRefIndex, scinArrEntries);
	pmmaPT->AddProperty("WLSABSLENGTH", scinPhotonEnergy, pmmaAbsLen, scinArrEntries);
	pmmaPT->AddProperty("WLSCOMPONENT", scinPhotonEnergy, pmmaEmission, scinArrEntries);
	pmmaPT->AddConstProperty("WLSTIMECONSTANT", fPMMADecayTime); // 
	PMMA->SetMaterialPropertiesTable(pmmaPT);

	/* WLS fibers are made of polymethylmethacrylate (PMMA)
		and a double cladding made of polyethylene and 
		fluorinated polyethylene

		PMMA: C5O2H8
		Pethylene: C2H4
	*/

	// Polyethylene (for internal cladding of WLS fibers)
	Pethylene = new G4Material("Pethylene", 1.200 * g/cm3, 2);
	Pethylene->AddElement(elC, 2);
	Pethylene->AddElement(elH, 4);

	// Add properties to table
	G4MaterialPropertiesTable* pethylenePT = new G4MaterialPropertiesTable();
	pethylenePT->AddProperty("RINDEX", scinPhotonEnergy, pethyRefIndex, scinArrEntries);
	pethylenePT->AddProperty("ABSLENGTH", scinPhotonEnergy, pethyAbsLen, scinArrEntries);
	Pethylene->SetMaterialPropertiesTable(pethylenePT);

	// Fluorinated Polyethylene (for external cladding of WLS fibers)
	FPethylene = new G4Material("FPethylene", 1.400 * g/cm3, 2);
	FPethylene->AddElement(elC, 2);
	FPethylene->AddElement(elH, 4);

	// Add properties to table
	G4MaterialPropertiesTable* fpethylenePT = new G4MaterialPropertiesTable();
	fpethylenePT->AddProperty("RINDEX", scinPhotonEnergy, fpethyRefIndex, scinArrEntries);
	fpethylenePT->AddProperty("ABSLENGTH", scinPhotonEnergy, pethyAbsLen, scinArrEntries);
	FPethylene->SetMaterialPropertiesTable(fpethylenePT);


	// --------------------------------------------------------------------
	// Borosilicate glass "Pyrex" (for SiPM & PMT active surfaces)
	// --------------------------------------------------------------------
	/*
    NOTE : The "face" is actually an ellipsoidal surface
    The active area (photocathode) covers 90% of it.
    The refractive index is the same as the dome
    refractive index. The absorption length is set
    so that no photons propagate through the
    photocathode.
  */

  // from PDG:
  // 80% SiO2 + 13% B2O2 + 7% Na2O
  // by fractional mass?

	Pyrex = new G4Material("Pyrex", 2.23 * g/cm3, 3);
	Pyrex->AddMaterial(SiO2, 0.80);
	Pyrex->AddMaterial(B2O2, 0.13);
	Pyrex->AddMaterial(Na2O, 0.07);

	// Add properties to table
	#warning "This properties are for PMTs! Check Pyrex optical properties for SiPMs!!"
	G4MaterialPropertiesTable* pyrexPT = new G4MaterialPropertiesTable();
	pyrexPT->AddProperty("RINDEX", pmtPhotonEnergy, pmtRefIndex, pmtArrEntries);
	Pyrex->SetMaterialPropertiesTable(pyrexPT);



}