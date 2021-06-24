// implementation of the G4RockDetectorConstruction class

#include "G4RockDetectorConstruction.h"

G4RockDetectorConstruction::G4RockDetectorConstruction() : 
	G4VUserDetectorConstruction()
	{ 
	}

G4RockDetectorConstruction::~G4RockDetectorConstruction() 
	{ }

void
G4RockDetectorConstruction::CreateElements() 
{
	elN = new G4Element("Nitrogen", "N", 7, 14.01 * g/mole);
	elO = new G4Element("Oxygen", "O", 8, 16.00 * g/mole);
	elH = new G4Element("Hydrogen", "H", 1, 1.01 * g/mole);
	elC = new G4Element("Carbon", "C", 6, 12.0107 * g/mole);
	elSi = new G4Element("Silicon", "Si", 14, 28.09 * g/mole);
	elTi = new G4Element("Titanium", "Ti", 22, 47.867 * g/mole);
	
	SiO2 = new G4Material("SiO2", 2.65 * g/cm3, 2);
	SiO2->AddElement(elSi, 1);
	SiO2->AddElement(elO, 2);
	// checkear densidad en datasheet
	TiO2 = new G4Material("TiO2", 4.26 * g/cm3, 2);
	TiO2->AddElement(elTi, 1);
	TiO2->AddElement(elO, 2);

}

void 
G4RockDetectorConstruction::CreateMaterials() 
{
	CreateAir();
	CreateRock(); // por ahora es solo SiO2. eventualmente sera mas complejo por lo que conviene dejarlo afuera
	CreatePolystyrene();
	CreateScintillator();
  SetScinPropertyTable();
	CreateWLS();
}

G4VPhysicalVolume*
G4RockDetectorConstruction::CreateDetector()
{
	CreateSolids();
	CreateWorld();
	CreateHall();
	AssembleDetector();
	return physWorld;
}

void
G4RockDetectorConstruction::CreateAir()
{
	Air = new G4Material("Air", 1.29e-3 * g/cm3, 2);
	Air->AddElement(elN, 0.7);
	Air->AddElement(elO, 0.3);

}

void
G4RockDetectorConstruction::CreateRock()
{
	/* por ahora la roca solo esta compuesta por SiO2
		 pero eventualmente tendra otros componentes y densidad variable
	*/
	RockMaterial = SiO2;
}

void
G4RockDetectorConstruction::CreatePolystyrene()
{
	// Polystyrene for the scintillator bars Dow STYRON 663 W
  Polystyrene = new G4Material("Polystrene", 1.04 * g/cm3, 2);
  Polystyrene->AddElement(elC, 8);
  Polystyrene->AddElement(elH, 8);
}

void
G4RockDetectorConstruction::CreateScintillator()
{
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
    // Scintillator material:
    ScinMat = new G4Material("ScinMat", 1.060 * g/cm3, 3);
    ScinMat->AddMaterial(Polystyrene, 98.7*perCent);
    ScinMat->AddMaterial(PPO, 1*perCent);
    ScinMat->AddMaterial(POPOP, 0.3*perCent);
    

    // Scintillator Coating - 15% TiO2 and 85% polystyrene by weight.
    ScinCoat = new G4Material("ScinCoat", 1.52 * g/cm3, 2);
    ScinCoat->AddMaterial(TiO2, 15*perCent);
    ScinCoat->AddMaterial(Polystyrene, 85*perCent);

}

void
G4RockDetectorConstruction::SetScinPropertyTable()
{	
	// from Auger's UMD construction
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

  assert(sizeof(refIndexScin) == n);

  G4double absLengthScin[] = {
    1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm,
    1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm,
    1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm, 1.0*cm,
    20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm,
    20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm, 20.0*cm
  };

  assert(sizeof(absLengthScin) == n);

  G4double fastCompScin[] = {
    0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
  };
  assert(sizeof(fastCompScin) == n);

 	G4double slowCompScin[] = {
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  assert(sizeof(slowCompScin) == n);

  // Add properties into table
  G4MaterialPropertiesTable* scinPT = new G4MaterialPropertiesTable();
  scinPT->AddProperty("RINDEX", photonEnergy, refIndexScin, n);
  scinPT->AddProperty("ABSLENGTH", photonEnergy, absLengthScin, n);
  scinPT->AddProperty("FASTCOMPONENT", photonEnergy, fastCompScin, n);
  scinPT->AddProperty("SLOWCOMPONENT", photonEnergy, slowCompScin, n);
  scinPT->AddConstProperty("SCINTILLATIONYIELD", fScintillationYield);
  scinPT->AddConstProperty("RESOLUTIONSCALE", 1);
  scinPT->AddConstProperty("FASTTIMECONSTANT", 5*ns);
  scinPT->AddConstProperty("SLOWTIMECONSTANT", 50*ns);
  scinPT->AddConstProperty("YIELDRATIO", 1);

  ScinMat->SetMaterialPropertiesTable(scinPT);

  // Add optical skin properties of scintillator
  G4double extrusion_polish = 1;
  ScinOptSurf = new G4OpticalSurface("ScinOptSurf", glisur, ground, dielectric_metal, extrusion_polish);

  G4double scinRefl[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  };
  
  assert(sizeof(scinRefl) == n);

  G4double scinEff[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  };
  
  assert(sizeof(scinEff) == n);

  G4MaterialPropertiesTable* scinOptSurfPT = new G4MaterialPropertiesTable();
  scinOptSurfPT->AddProperty("REFLECTIVITY", photonEnergy, scinRefl, n);
  scinOptSurfPT->AddProperty("EFFICIENCY", photonEnergy, scinEff, n);

  ScinOptSurf->SetMaterialPropertiesTable(scinOptSurfPT);
  new G4LogicalSkinSurface("ScinBarCoating", logicCoat, ScinOptSurf);

}

void
G4RockDetectorConstruction::CreateWLS() 
{
	/* WLS fibers are made of polymethylmethacrylate (PMMA)
		and a double cladding made of polyethylene and 
		fluorinated polyethylene

		PMMA: C5O2H8
		Pethylene: C2H4
	*/

	PMMA = new G4Material("PMMA", 1.190 * g/cm3, 3);
	PMMA->AddElement(elC, 5);
	PMMA->AddElement(elH, 8);
	PMMA->AddElement(elO, 2);

	Pethylene = new G4Material("Pethylene", 1.200 * g/cm3, 2);
	Pethylene->AddElement(elC, 2);
	Pethylene->AddElement(elH, 4);

	FPethylene = new G4Material("FPethylene", 1.400 * g/cm3, 2);
	FPethylene->AddElement(elC, 2);
	FPethylene->AddElement(elH, 4);

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

  // PMMA

  G4double refIndexWLS[] = {
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60,
    1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60, 1.60
  };

  assert(sizeof(refIndexWLS) == n);

  G4double absLengthWLS[] = {
		10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m,
		10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m,
		10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 10.00*m, 1.0*m,
		0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m,
		0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m, 0.0001*m
  };

  assert(sizeof(absLengthWLS) == n);

  G4double emissionWLS[] = {
    0.05, 0.10, 0.30, 0.50, 0.75, 1.00, 1.50, 1.85, 2.30, 2.75,
    3.25, 3.80, 4.50, 5.20, 6.00, 7.00, 8.50, 9.50, 11.1, 12.4,
    12.9, 13.0, 12.8, 12.3, 11.1, 11.0, 12.0, 11.0, 17.0, 16.9,
    15.0, 9.00, 2.50, 1.00, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00,
    0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00
  };

 	assert(sizeof(emissionWLS) == n);

 	// Add properties to table
  G4MaterialPropertiesTable* WLSFiberPT = new G4MaterialPropertiesTable();
  WLSFiberPT->AddProperty("RINDEX", photonEnergy, refIndexWLS, n);
  WLSFiberPT->AddProperty("WLSABSLENGTH", photonEnergy, absLengthWLS, n);
  WLSFiberPT->AddProperty("WLSCOMPONENT", photonEnergy, emissionWLS, n);
  WLSFiberPT->AddConstProperty("WLSTIMECONSTANT", 2.7*ns); // according to datasheet BCF-92 WLS fiber
  //WLSFiberPT->AddConstProperty("WLSTIMECONSTANT", 20*ns); // according to AMIGA detector construction in Offline


  PMMA->SetMaterialPropertiesTable(WLSFiberPT);

  //  Polyethylene (Clad1)

  G4double refIndexClad1[] = {
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49,
    1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49, 1.49
  };

  assert(sizeof(refIndexClad1) == n);

  G4double absLengthClad[] = {
    20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
    20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
    20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
    20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m,
    20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m, 20.0*m
  };
  
  assert(sizeof(absLengthClad) == n);

  // Add properties to table
  G4MaterialPropertiesTable*  clad1PT = new G4MaterialPropertiesTable();
  clad1PT->AddProperty("RINDEX", photonEnergy, refIndexClad1, n);
  clad1PT->AddProperty("ABSLENGTH", photonEnergy, absLengthClad, n);

  Pethylene->SetMaterialPropertiesTable(clad1PT);

  // Fluorinated Polyethylene

  G4double refIndexClad2[] = {
    1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
    1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
    1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
    1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42,
    1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42, 1.42
  };

  assert(sizeof(refIndexClad1) == n);

  // Add properties to table
  G4MaterialPropertiesTable* clad2PT = new G4MaterialPropertiesTable();
  clad2PT->AddProperty("RINDEX", photonEnergy, refIndexClad2, n);
  clad2PT->AddProperty("ABSLENGTH", photonEnergy, absLengthClad, n);

  FPethylene->SetMaterialPropertiesTable(clad2PT);

}

void
G4RockDetectorConstruction::CreateSolids()
{

	solidWorld 	= new G4Box("World", fWorldSizeX, fWorldSizeY, fWorldSizeZ);
	solidRock  	= new G4Box("Rock", fRockSizeX, fRockSizeY, fRockSizeZ);
	solidHall  	= new G4Box("Hall", fHallSizeX, fHallSizeY, fHallSizeZ);
  solidCasing = new G4Box("Casing", fBarWidth/2+fCasingThickness, fBarThickness/2+fCasingThickness, fBarLength/2+fCasingThickness);
	solidCoat  	= new G4Box("BarCoating", fBarWidth/2, fBarThickness/2, fBarLength/2);
	solidBar   	= new G4Box("BarScin", fBarWidth/2-fCoatingThickness, fBarThickness/2-fCoatingThickness, fBarLength/2-fCoatingThickness);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidSiPM   = new G4Box("SiPM", fSiPMSizeX, fSiPMSizeY, fSiPMSizeZ);
  // create second SiPM volume to be placed on the oposite side
  solidSiPM_back  = new G4Box("SiPM_back", fSiPMSizeX, fSiPMSizeY, fSiPMSizeZ);

}

void
G4RockDetectorConstruction::CreateWorld()
{
	logicWorld = new G4LogicalVolume(solidWorld, Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);
	//logicWorld->SetVisAttributes(G4VisAttributes::Invisible);							
}

void
G4RockDetectorConstruction::CreateHall()
{
	// roca y aire (eventualmente Rock sera "ground"... y el hall sera un hall de verdad... 1:35AM)
	G4VisAttributes brown(G4Colour::Brown());
	logicRock = new G4LogicalVolume(solidRock, RockMaterial, "Rock");
	logicRock->SetVisAttributes(brown);
	physRock 	=  new G4PVPlacement(nullptr, G4ThreeVector(), logicRock, "Rock", logicWorld, false, 0, fCheckOverlaps);
	logicHall = new G4LogicalVolume(solidHall, Air, "Hall");
	physHall 	=  new G4PVPlacement(nullptr, G4ThreeVector(), logicHall, "Hall", logicRock, false, 0, fCheckOverlaps);

}

void
G4RockDetectorConstruction::AssembleDetector()
{
	// logical volumes and placement of detector components
	G4RotationMatrix* rotation = new G4RotationMatrix();
  rotation->rotateX(M_PI);
  logicCasing = new G4LogicalVolume(solidCasing, Air, "Casing", 0, 0, 0);
  physCasing  = new G4PVPlacement(rotation, G4ThreeVector(), logicCasing, "Casing", logicHall, false, 0, fCheckOverlaps);
	logicCoat = new G4LogicalVolume(solidCoat, ScinCoat, "BarCoating", 0, 0, 0);
	physCoat 	= new G4PVPlacement(rotation, G4ThreeVector(), logicCoat, "BarCoating", logicCasing, false, 0, fCheckOverlaps);
	logicBar 	= new G4LogicalVolume(solidBar, ScinMat, "BarScin", 0, 0, 0);
	physBar 	= new G4PVPlacement(rotation, G4ThreeVector(), logicBar, "BarScin", logicCoat, false, 0, fCheckOverlaps);

	logicFiber = new G4LogicalVolume(solidFiber, PMMA, "WLSFiber", 0, 0, 0);
	G4VisAttributes green(G4Colour::Green());
	logicFiber->SetVisAttributes(green);
	logicClad1 = new G4LogicalVolume(solidClad1, Pethylene, "FiberClad1", 0, 0, 0);
	logicClad1->SetVisAttributes(green);
	logicClad2 = new G4LogicalVolume(solidClad2, FPethylene, "FiberClad2", 0, 0, 0);
	logicClad2->SetVisAttributes(green);
	G4VisAttributes blue(G4Colour::Blue());
	logicSiPM = new G4LogicalVolume(solidSiPM, Air, "SiPM", 0, 0, 0);
	logicSiPM->SetVisAttributes(blue);
  G4VisAttributes black(G4Colour::Black());
  logicSiPM_back = new G4LogicalVolume(solidSiPM_back, Air, "SiPM_back", 0, 0, 0);
  logicSiPM_back->SetVisAttributes(black);

	// place fiber at the top of the bar (y-axis)
  
	G4double y = fBarThickness/2 - fCoatingThickness - fFiberRadius - fCladdingThickness;
	physClad2 = new G4PVPlacement(rotation, G4ThreeVector(0, y, 0), logicClad2, "FiberClad2", logicBar, true, 0, fCheckOverlaps);
	physClad1 = new G4PVPlacement(rotation, G4ThreeVector(), logicClad1, "FiberClad1", logicClad2, false, 0, fCheckOverlaps);
	physFiber = new G4PVPlacement(rotation, G4ThreeVector(), logicFiber, "WLSFiber", logicClad1, false, 0, fCheckOverlaps);

	G4double z = fBarLength/2 - 2*fCoatingThickness;
	physSiPM  = new G4PVPlacement(rotation, G4ThreeVector(0, y, z), logicSiPM, "SiPM", logicBar, false, 0, fCheckOverlaps);
  physSiPM_back  = new G4PVPlacement(rotation, G4ThreeVector(0, y, -z), logicSiPM_back, "SiPM_back", logicBar, false, 0, fCheckOverlaps);
  


}

G4VPhysicalVolume* 
G4RockDetectorConstruction::Construct() {

	if (!physWorld) {
		CreateElements();
		CreateMaterials();
		return CreateDetector();
	}
	return physWorld;

}
