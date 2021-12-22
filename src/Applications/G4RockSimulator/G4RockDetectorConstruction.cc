// implementation of the G4RockDetectorConstruction class
#include "G4RockDetectorConstruction.h"
#include "G4RockModuleAction.h"
#include "G4RockSiPMAction.h"
#include "G4RockFiberAction.h"

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


G4RockDetectorConstruction::G4RockDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4RockDetectorConstruction::~G4RockDetectorConstruction() 
	{ }

void
G4RockDetectorConstruction::CreateElements() 
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
G4RockDetectorConstruction::CreateMaterials() 
{
	CreateAir();
	CreateRock(); // por ahora es solo SiO2. eventualmente sera mas complejo por lo que conviene dejarlo afuera
	CreatePolystyrene();
	CreateScintillator();
  SetScinPropertyTable();
	CreateWLS();
	CreatePyrex();
}

G4VPhysicalVolume*
G4RockDetectorConstruction::CreateDetector()
{
	//CreateSolids();
	CreateWorld();
	CreateHall();
	ReadModuleList("./ModuleList.xml");
	return physWorld;
}

void
G4RockDetectorConstruction::ReadModuleList(string fModuleList)
{

	cout << "G4RockDetectorConstruction::ReadModuleList: Reading Module(s) position(s) from " << fModuleList << endl;
	Detector& detector = fEvent.GetDetector();
	int nModules = 0;

	// read XML with module positions
	ptree tree;
	read_xml(fModuleList, tree);
	for (const auto& i : tree.get_child("moduleList")) {		
		ptree subtree;
		string name;
		tie(name, subtree) = i;

		if (name != "module")
			continue;

		vector<double> modulePosition;
		string moduleIdstr = subtree.get<string>("<xmlattr>.id");
		int moduleId = stoi(moduleIdstr);

		// create and get module
		detector.MakeModule(moduleId);
		Module& module = detector.GetModule(moduleId);

		nModules+=1;
		
		
		for (const auto &v : subtree.get_child("")) {
			string label = v.first;
			if ( label != "<xmlattr>" ) {
					if (label != "barsInPanel") {
						double value = stod(v.second.data());
						string unit = v.second.get<string>("<xmlattr>.unit");
						modulePosition.push_back(value*G4UnitDefinition::GetValueOf(unit));
					} else {
						int nBarsInPanel = stoi(v.second.data());
						module.SetNBars(nBarsInPanel);
					}
				//cout << "The unit of " << label << " is " << unit << " " << G4UnitDefinition::GetValueOf(unit) << " " << cm << endl;
			} 			
		}



		//detector.SetNModules(nModules);
		//module.SetModulePosition(&modulePosition);
    module.SetModulePosition(modulePosition);
		
		// module properties
		fCasingThickness = module.GetCasingThickness() * mm;

		// scintillator bar properties
		fBarWidth  = module.GetBarWidth() * mm;
		fBarLength = module.GetNBars() * fBarWidth; 
		fBarThickness = module.GetBarThickness() * mm;
		fCoatingThickness = module.GetBarCoatingThickness() * mm;

		// fiber properties
		fCladdingThickness = module.GetCladdingThickness() * mm;
		fFiberRadius = module.GetFiberRadius() * mm;

		fCasingSizeX = fBarLength/2 + fCoatingThickness + fCasingThickness;
  	fCasingSizeY = fBarWidth/2 + fCoatingThickness;// + fCasingThickness;
  	fCasingSizeZ = fBarThickness/2 + fCoatingThickness;// + fCasingThickness;

  	SiPM& sipm = module.GetSiPM();
  	fSiPMSizeZ = sipm.GetSiPMWidth() * mm;
  	fSiPMSizeX = fSiPMSizeY = sipm.GetSiPMLength() * mm;
  	

		CreateModule(module);	
	}

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
G4RockDetectorConstruction::CreatePyrex()
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
  #warning "Check Pyrex (SiPM) optical properties!"
  G4double photonEnergy[] = {2.00*eV, 4.00*eV};
  //G4double pmtAbsorption[] = {0.0005*mm, 0.0005*mm};
  G4double pmtRefraction[] = {1.47, 1.47};
  G4int n = sizeof(photonEnergy)/sizeof(G4double);
  pmtfaceMPT->AddProperty("RINDEX", photonEnergy, pmtRefraction, n);
  // for now we use only RINDEX as i have no clue where to find/cross-check this number
  //pmtfaceMPT->AddProperty("ABSLENGTH", photonEnergy, pmtAbsorption, n);

  Pyrex->SetMaterialPropertiesTable(pmtfaceMPT);
}

void
G4RockDetectorConstruction::CreateWorld()
{

	solidWorld 	= new G4Box("World", fWorldSizeX, fWorldSizeY, fWorldSizeZ);
	logicWorld = new G4LogicalVolume(solidWorld, Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);
	//logicWorld->SetVisAttributes(G4VisAttributes::Invisible);							
}

void
G4RockDetectorConstruction::CreateHall()
{
	// roca y aire (eventualmente Rock sera "ground"... y el hall sera un hall de verdad... 1:35AM)
	solidRock  	= new G4Tubs("Rock", 0, fRockRadius, fRockDepth, 0, 360*deg);
	solidHall  	= new G4Box("Hall", fHallSizeX, fHallSizeY, fHallSizeZ);
	G4VisAttributes brown(G4Colour::Brown());
	logicRock = new G4LogicalVolume(solidRock, RockMaterial, "Rock");
	logicRock->SetVisAttributes(brown);
	physRock 	=  new G4PVPlacement(nullptr, G4ThreeVector(), logicRock, "Rock", logicWorld, false, 0, fCheckOverlaps);
	logicHall = new G4LogicalVolume(solidHall, Air, "Hall");
	physHall 	=  new G4PVPlacement(nullptr, G4ThreeVector(fHallPosX, fHallPosY, fHallPosZ), logicHall, "Hall", logicRock, false, 0, fCheckOverlaps);

}

void 
G4RockDetectorConstruction::CreateModule(Module& module)
{

	G4SDManager* const sdMan = G4SDManager::GetSDMpointer();

	// module position
	G4ThreeVector  modulePos = Geometry::ToG4Vector(module.GetModulePosition(), 1.);
	int moduleId = module.GetId();
	int nBars = module.GetNBars();

	cout << "G4RockDetectorConstruction::CreateModule: Creating detector with id " << moduleId << ", with " << nBars << " bars per panel at position (x, y, z) = (" << modulePos[0] / cm << ", " << modulePos[1] / cm << ", " << modulePos[2] / cm << ") cm." << endl;

	// define a enclosure volume that contains the components of the module
	// this is just to ease the module construction...not a proper volume
	ostringstream nameModule;
	nameModule.str("");
  nameModule << "Casing" << '_' << moduleId;
  const double x = fBarLength/2;
  const double y = fBarWidth/2 * nBars; // number of bars on each panel
  const double z = fBarThickness/2 * 2; // 2 = number of panels
  const double t = fCasingThickness;

	solidCasing = new G4Box("Casing", x+t, y+t, z+t);
	logicCasing = new G4LogicalVolume(solidCasing, Air, "Casing", 0, 0, 0);
	physCasing  = new G4PVPlacement(nullptr, modulePos, logicCasing, "Casing", logicHall, false, moduleId, fCheckOverlaps);
	
	solidCoat  	= new G4Box("BarCoating", fCoatingThickness + fBarLength/2, fCoatingThickness + fBarWidth/2, fCoatingThickness + fBarThickness/2);
	solidBar   	= new G4Box("BarScin", fBarLength/2, fBarWidth/2, fBarThickness/2);
	solidFiber  = new G4Tubs("Fiber", 0, fFiberRadius - 2*fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad1  = new G4Tubs("Clad1", 0, fFiberRadius - fCladdingThickness, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidClad2  = new G4Tubs("Clad2", 0, fFiberRadius, fBarLength/2-2*fCoatingThickness-fSiPMSizeZ, 0, 360*deg);
	solidSiPM   = new G4Box("SiPM", fSiPMSizeX, fSiPMSizeY, fSiPMSizeZ);

	// registration
	G4RockModuleAction* const moduleCasingSD = new G4RockModuleAction("/Module/" + nameModule.str(), moduleId, fEvent);
  sdMan->AddNewDetector(moduleCasingSD);
  logicCasing->SetSensitiveDetector(moduleCasingSD);

	// color definitions for visualization
  G4VisAttributes green(G4Colour::Green());
  G4VisAttributes blue(G4Colour::Blue());
  G4VisAttributes black(G4Colour::Black());

	G4RotationMatrix* rotationFiber = new G4RotationMatrix();
  G4RotationMatrix* rotationBot = new G4RotationMatrix();
  rotationFiber->rotateY(M_PI/2);
  rotationBot->rotateZ(M_PI/2);

  /****
    coordinates and orientation of module components
    bar length 		along x-axis
    bar width 		along y-axis
    bar thickness along z-axis
    
    in the (MxN) = (2x2) configuration, 2 bars will be placed
    along the y axis (M) and 2 along the z axis (N). therefore
    bars should be created within the loop and placed accordingly

    position of top and bottom bars are set wrt module origin

  */

  G4double fPosTopX = 0;
  G4double fPosTopY = fCasingSizeY;
  G4double fPosTopZ = fCasingSizeZ;
  
  G4double fPosBotX = fCasingSizeY;
  G4double fPosBotY = 0;
  G4double fPosBotZ = -fCasingSizeZ;
  // place fiber at the top of the bar (x-axis)
  // place SiPM at the end of the bar (z-axis)
  G4double fFiberTopPosX = fBarLength/2 - 2*fCoatingThickness;
  G4double fFiberTopPosZ = -fBarThickness/2 + fCoatingThickness + fFiberRadius + fCladdingThickness;
  G4double fFiberBotPosZ = fFiberTopPosZ;

  // bars of the top panel
  for (G4int i=0; i<nBars; ++i) {

		/* 
			update bar placement for nBars>2...
			maybe create consecutive bars starting form a given point, i.e.,
			barPos = MoulePosition - nBars * barLength (?) or smth like this...
		*/

		unsigned int barId = i;
	  G4int inc = pow(-1,i);//(1+2*i);
	  G4double yp = inc*fPosTopY;

	  string nameBarCoating = "BarCoating_"+to_string(barId);
  	string nameBarScin = "BarScin_"+to_string(barId);
  	string nameClad2 = "FiberClad2_"+to_string(barId);
  	string nameClad1 = "FiberClad1_"+to_string(barId);
  	string nameFiber = "Fiber_"+to_string(barId);
  	string nameSiPM = "SiPM_"+to_string(barId);

  	// register SiPM in the detector class
  	module.MakeSiPM(barId);

	  logicCoat = new G4LogicalVolume(solidCoat, ScinCoat, nameBarCoating, 0, 0, 0);
	  logicBar  = new G4LogicalVolume(solidBar, ScinMat, nameBarScin, 0, 0, 0);
	  logicClad2 = new G4LogicalVolume(solidClad2, FPethylene, nameClad2, 0, 0, 0);
	  logicClad2->SetVisAttributes(green);
	  logicClad1 = new G4LogicalVolume(solidClad1, Pethylene, nameClad1, 0, 0, 0);
	  logicClad1->SetVisAttributes(green); 
	  logicFiber = new G4LogicalVolume(solidFiber, PMMA, nameFiber, 0, 0, 0);
	  logicFiber->SetVisAttributes(green); 
	  logicSiPM = new G4LogicalVolume(solidSiPM, Pyrex, nameSiPM, 0, 0, 0);
	  logicSiPM->SetVisAttributes(blue); 

	  // top panel upper direction
	  physCoat  = new G4PVPlacement(nullptr, G4ThreeVector(fPosTopX, yp, fPosTopZ), logicCoat, 
	  	nameBarCoating, logicCasing, false, barId, fCheckOverlaps);
	  physBar   = new G4PVPlacement(nullptr, G4ThreeVector(), logicBar, 
	  	nameBarScin, logicCoat, false, barId, fCheckOverlaps);
	  physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberTopPosZ), logicClad2, 
	  	nameClad2, logicBar, true, barId, fCheckOverlaps);
	  physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
	  	nameClad1, logicClad2, false, barId, fCheckOverlaps);
	  physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
	  	nameFiber, logicClad1, false, barId, fCheckOverlaps); 
	  physSiPM  = new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberTopPosX, 0, fFiberTopPosZ), logicSiPM, 
	  	nameSiPM, logicBar, false, barId, fCheckOverlaps);

	  // registration of SiPM
		G4RockSiPMAction* const SiPMTopSD = new G4RockSiPMAction("/Module/" + nameModule.str() + "/" + nameSiPM, moduleId, barId, fEvent);
		sdMan->AddNewDetector(SiPMTopSD);
		logicSiPM->SetSensitiveDetector(SiPMTopSD);

		
  }

  
  // bars of the bottom panel
  for (G4int i=nBars; i<2*nBars; ++i) {

  	unsigned int barId = i;
  	G4int inc = pow(-1,i);
	  G4double xp = inc*fPosBotX;
    
    string nameBarCoating = "BarCoating_"+to_string(barId);
  	string nameBarScin = "BarScin_"+to_string(barId);
  	string nameClad2 = "FiberClad2_"+to_string(barId);
  	string nameClad1 = "FiberClad1_"+to_string(barId);
  	string nameFiber = "Fiber_"+to_string(barId);
  	string nameSiPM = "SiPM_"+to_string(barId);

  	// register SiPM in the detector class
  	module.MakeSiPM(barId);

	  logicCoat = new G4LogicalVolume(solidCoat, ScinCoat, nameBarCoating, 0, 0, 0);
	  logicBar  = new G4LogicalVolume(solidBar, ScinMat, nameBarScin, 0, 0, 0);
	  logicClad2 = new G4LogicalVolume(solidClad2, FPethylene, nameClad2, 0, 0, 0);
	  logicClad2->SetVisAttributes(green);
	  logicClad1 = new G4LogicalVolume(solidClad1, Pethylene, nameClad1, 0, 0, 0);
	  logicClad1->SetVisAttributes(green); 
	  logicFiber = new G4LogicalVolume(solidFiber, PMMA, nameFiber, 0, 0, 0);
	  logicFiber->SetVisAttributes(green); 
	  logicSiPM = new G4LogicalVolume(solidSiPM, Pyrex, nameSiPM, 0, 0, 0);
	  logicSiPM->SetVisAttributes(blue);

    // upper direction
    physCoat  = new G4PVPlacement(rotationBot, G4ThreeVector(xp, fPosBotY, fPosBotZ), logicCoat, 
    	nameBarCoating, logicCasing, false, barId, fCheckOverlaps);
    physBar   = new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), logicBar,
     nameBarScin, logicCoat, false, barId, fCheckOverlaps);
    physClad2 = new G4PVPlacement(rotationFiber, G4ThreeVector(0, 0, fFiberBotPosZ), logicClad2, 
    	nameClad2, logicBar, true, barId, fCheckOverlaps);
    physClad1 = new G4PVPlacement(nullptr, G4ThreeVector(), logicClad1, 
    	nameClad1, logicClad2, false, barId, fCheckOverlaps);
    physFiber = new G4PVPlacement(nullptr, G4ThreeVector(), logicFiber, 
    	nameFiber, logicClad1, false, barId, fCheckOverlaps); 
    physSiPM  = new G4PVPlacement(rotationFiber, G4ThreeVector(fFiberTopPosX, 0, fFiberBotPosZ), logicSiPM, 
    	nameSiPM, logicBar, false, barId, fCheckOverlaps);

    // registration of SiPM
		G4RockSiPMAction* const SiPMBotSD = new G4RockSiPMAction("/Module/" + nameModule.str() + "/" + nameSiPM, moduleId, barId, fEvent);
  	sdMan->AddNewDetector(SiPMBotSD);
  	logicSiPM->SetSensitiveDetector(SiPMBotSD);
	
  }
  
}



G4VPhysicalVolume* 
G4RockDetectorConstruction::Construct() 
{

	//SetDetectorParameters();

	if (!physWorld) {
		CreateElements();
		CreateMaterials();
		return CreateDetector();
	}
	return physWorld;

}

