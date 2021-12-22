#include "G4BarDetectorConstruction.h"

#include "Detector.h"
#include "Module.h"
#include "SiPM.h"
#include "Geometry.h"

#include "G4UnitsTable.hh"

#include <iostream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

using boost::property_tree::ptree;
using namespace std;


G4BarDetectorConstruction::G4BarDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4BarDetectorConstruction::~G4BarDetectorConstruction() 
	{ }

void
G4BarDetectorConstruction::CreateElements() 
{
	elN  = new G4Element("Nitrogen", "N", 7, 14.01 * g/mole);
	elO  = new G4Element("Oxygen", "O", 8, 16.00 * g/mole);
	elH  = new G4Element("Hydrogen", "H", 1, 1.01 * g/mole);
	elC  = new G4Element("Carbon", "C", 6, 12.0107 * g/mole);
	elSi = new G4Element("Silicon", "Si", 14, 28.09 * g/mole);
	elTi = new G4Element("Titanium", "Ti", 22, 47.867 * g/mole);
	elB  = new G4Element("Boron", "B", 5, 10.811 * g/mole);
  elNa = new G4Element("Sodium", "Na", 11, 22.98977 * g/mole);
	
	SiO2 = new G4Material("SiO2", 2.65 * g/cm3, 2);
	SiO2->AddElement(elSi, 1);
	SiO2->AddElement(elO, 2);
	// checkear densidad en datasheet
	TiO2 = new G4Material("TiO2", 4.26 * g/cm3, 2);
	TiO2->AddElement(elTi, 1);
	TiO2->AddElement(elO, 2);

	B2O2 = new G4Material("B2O2", 2.23 * g/cm3, 2);
	B2O2->AddElement(elB, 2);
  B2O2->AddElement(elO, 2);

  Na2O = new G4Material("Na2O", 2.23 * g/cm3, 2);
  Na2O->AddElement(elNa, 2);
  Na2O->AddElement(elO, 1);

}

void 
G4BarDetectorConstruction::CreateMaterials() 
{
	CreateAir();
	CreatePolystyrene();
	CreateScintillator();
  SetScinPropertyTable();
	CreateWLS();
	CreatePyrex();
}

G4VPhysicalVolume*
G4BarDetectorConstruction::CreateDetector()
{
	
	// create world
	solidWorld 	= new G4Box("World", fWorldSizeX/2, fWorldSizeY/2, fWorldSizeZ/2);
	logicWorld = new G4LogicalVolume(solidWorld, Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);
	
	// create scintillator bar
	G4ThreeVector  barPos = G4ThreeVector(0., 0., 0.);
	ostringstream barName;
	barName.str("");
  barName << "Casing";

  const double x = fBarLength/2;
  const double y = fBarWidth/2;   
  const double z = fBarThickness/2;
  const double t = fCasingThickness;

	solidCasing = new G4Box("Casing", x+t, y+t, z+t);
	logicCasing = new G4LogicalVolume(solidCasing, Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, barPos, logicCasing, "Casing", logicWorld, false, 0, fCheckOverlaps);

	solidCoat  	= new G4Box("BarCoating", x + fCoatingThickness, y + fCoatingThickness, z + fCoatingThickness);
	solidBar   	= new G4Box("BarScin", x, y, z);
	//solidHole   = new G4Tubs("BarHole", 0, fFiberRadius+fHoleThickness, x, 0, 360*deg);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, x - 2*fCoatingThickness - fPMTSizeZ, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, x - 2*fCoatingThickness - fPMTSizeZ, 0, 360*deg);
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, x - 2*fCoatingThickness - fPMTSizeZ, 0, 360*deg);
	solidPMT   	= new G4Box("PMT", fPMTSizeX, fPMTSizeY, fPMTSizeZ);

	// color definitions for visualization
  G4VisAttributes green(G4Colour::Green());
  G4VisAttributes blue(G4Colour::Blue());
  G4VisAttributes black(G4Colour::Black());

	G4RotationMatrix* rotationFiber = new G4RotationMatrix();
  G4RotationMatrix* rotationBot = new G4RotationMatrix();
  rotationFiber->rotateY(M_PI/2);

  logicCoat = new G4LogicalVolume(solidCoat, ScinCoat, "coat", 0, 0, 0);
  logicBar  = new G4LogicalVolume(solidBar, ScinMat, "bar", 0, 0, 0);
  //logicHole = new G4LogicalVolume(solidHole, Air, "hole", 0, 0, 0);
  logicClad2 = new G4LogicalVolume(solidClad2, FPethylene, "clad2", 0, 0, 0);
  logicClad2->SetVisAttributes(green);
  logicClad1 = new G4LogicalVolume(solidClad1, Pethylene, "clad1", 0, 0, 0);
  logicClad1->SetVisAttributes(green); 
  logicFiber = new G4LogicalVolume(solidFiber, PMMA, "fiber", 0, 0, 0);
  logicFiber->SetVisAttributes(green); 
  logicPMT = new G4LogicalVolume(solidPMT, Pyrex, "pmt", 0, 0, 0);
  logicPMT->SetVisAttributes(blue); 

  physCoat  = new G4PVPlacement(nullptr, barPos, logicCoat, "coat", logicCasing, false, 0, fCheckOverlaps);
  physBar   = new G4PVPlacement(nullptr, G4ThreeVector(), logicBar, "bar", logicCoat, false, 0, fCheckOverlaps);
  physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, 0), logicClad2, "clad2", logicBar, true, 0, fCheckOverlaps);
  physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, "clad1", logicClad2, false, 0, fCheckOverlaps);
  physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, "fiber", logicClad1, false, 0, fCheckOverlaps); 
  
  // place fiber extensions
  //physClad2Ext1 = new G4PVPlacement(nullptr, G4ThreeVector(fExtFiberPos, 0, 0), logicClad2Ext1, "clad2Ext1", logicHole, true, 0, fCheckOverlaps);
  //physClad2Ext2 = new G4PVPlacement(nullptr, G4ThreeVector(-1*fExtFiberPos, 0, 0), logicClad2Ext2, "clad2Ext2", logicHole, true, 0, fCheckOverlaps);
  
  //physClad1Ext1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1Ext1, "clad1Ext1", logicClad2Ext1, true, 0, fCheckOverlaps);
  //physClad1Ext2 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1Ext2, "clad1Ext2", logicClad2Ext2, true, 0, fCheckOverlaps);

  //physFiberExt1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiberExt1, "fiberExt1", logicClad1Ext1, true, 0, fCheckOverlaps);
  //physFiberExt2 = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiberExt2, "fiberExt2", logicClad1Ext2, true, 0, fCheckOverlaps);

  G4double fPMTPosX = x - 2*fCoatingThickness; 
  physPMT1  = new G4PVPlacement(rotationFiber, G4ThreeVector(fPMTPosX,  0, 0), logicPMT, "pmt_1", logicBar, false, 0, fCheckOverlaps);
  physPMT2  = new G4PVPlacement(rotationFiber, G4ThreeVector(-fPMTPosX, 0, 0), logicPMT, "pmt_2", logicBar, false, 1, fCheckOverlaps);

	return physWorld;
}



void
G4BarDetectorConstruction::CreateAir()
{
	Air = new G4Material("Air", 1.29e-3 * g/cm3, 2);
	Air->AddElement(elN, 0.7);
	Air->AddElement(elO, 0.3);

	// Fill material properties table for air
	G4MaterialPropertiesTable* airMPT = new G4MaterialPropertiesTable();

	G4double photonEnergy[] = 
	{	2.00*eV,2.03*eV,2.06*eV,2.09*eV,2.12*eV,
		2.15*eV,2.18*eV,2.21*eV,2.24*eV,2.27*eV,
		2.30*eV,2.33*eV,2.36*eV,2.39*eV,2.42*eV,
		2.45*eV,2.48*eV,2.51*eV,2.54*eV,2.57*eV,
		2.60*eV,2.63*eV,2.66*eV,2.69*eV,2.72*eV,
		2.75*eV,2.78*eV,2.81*eV,2.84*eV,2.87*eV,
		2.90*eV,2.93*eV,2.96*eV,2.99*eV,3.02*eV,
		3.05*eV,3.08*eV,3.11*eV,3.14*eV,3.17*eV,
		3.20*eV,3.23*eV,3.26*eV,3.29*eV,3.32*eV,
		3.35*eV,3.38*eV,3.41*eV,3.44*eV,3.47*eV};

	const G4int nEntries = sizeof(photonEnergy)/sizeof(G4double);

	G4double refractiveIndex[] =
	{ 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00,
		1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00, 1.00};

	assert(sizeof(refractiveIndex) == sizeof(photonEnergy));
	airMPT->AddProperty("RINDEX", photonEnergy, refractiveIndex, nEntries);
	Air->SetMaterialPropertiesTable(airMPT);

}


void
G4BarDetectorConstruction::CreatePolystyrene()
{
	// Polystyrene for the scintillator bars Dow STYRON 663 W
  Polystyrene = new G4Material("Polystrene", 1.04 * g/cm3, 2);
  Polystyrene->AddElement(elC, 8);
  Polystyrene->AddElement(elH, 8);
}

void
G4BarDetectorConstruction::CreateScintillator()
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
G4BarDetectorConstruction::SetScinPropertyTable()
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
    4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm,
    4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm,
    4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm, 4.0*cm,
    24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm,
    24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm, 24.0*cm
  };

  assert(sizeof(absLengthScin) == n);

  G4double fastCompScin[] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
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
G4BarDetectorConstruction::CreateWLS() 
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
G4BarDetectorConstruction::CreatePyrex()
{
  // Borosilicate glass (Pyrex) for the active pmt faces
  //----------------------------------------------------
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

  // Fill material properties table for pmt face
  G4MaterialPropertiesTable* pmtfaceMPT = new G4MaterialPropertiesTable();

  // pmt face rindex
  G4double photonEnergy[] = {2.00*eV, 4.00*eV};
  //G4double pmtAbsorption[] = {0.0005*mm, 0.0005*mm};
  G4double pmtRefraction[] = {1.47, 1.47};
  G4int n = sizeof(photonEnergy)/sizeof(G4double);
  pmtfaceMPT->AddProperty("RINDEX", photonEnergy, pmtRefraction, n);
  // for now we use only RINDEX as i have no clue where to find/cross-check this number
  //pmtfaceMPT->AddProperty("ABSLENGTH", photonEnergy, pmtAbsorption, n);

  Pyrex->SetMaterialPropertiesTable(pmtfaceMPT);
}



G4VPhysicalVolume* 
G4BarDetectorConstruction::Construct() 
{

	//SetDetectorParameters();

	if (!physWorld) {
		CreateElements();
		CreateMaterials();
		return CreateDetector();
	}
	return physWorld;

}
