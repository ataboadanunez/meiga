// implementation of the G4ExDetectorConstruction class
#include "G4ExDetectorConstruction.h"
#include "G4ExModuleAction.h"
#include "G4ExSiPMAction.h"
#include "G4ExFiberAction.h"

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


G4ExDetectorConstruction::G4ExDetectorConstruction(Event& theEvent) : 
	G4VUserDetectorConstruction(),
	fEvent(theEvent)
{ 
}

G4ExDetectorConstruction::~G4ExDetectorConstruction() 
	{ }

void
G4ExDetectorConstruction::CreateElements() 
{
  // example function to define elements and materials

	elN  = new G4Element("Nitrogen", "N", 7, 14.01 * g/mole);
	elO  = new G4Element("Oxygen", "O", 8, 16.00 * g/mole);
	elH  = new G4Element("Hydrogen", "H", 1, 1.01 * g/mole);
	elSi = new G4Element("Silicon", "Si", 14, 28.09 * g/mole);
  
  Air = new G4Material("Air", 1.29e-3 * g/cm3, 2);
  Air->AddElement(elN, 0.7);
  Air->AddElement(elO, 0.3); 
	
  // SiO2 = standard rock or quartz
	SiO2 = new G4Material("SiO2", 2.65 * g/cm3, 2);
	SiO2->AddElement(elSi, 1);
	SiO2->AddElement(elO, 2);

}


G4VPhysicalVolume*
G4ExDetectorConstruction::CreateDetector()
{

	CreateWorld();
	//PlaceDetector();
  /***
   The function PlaceDetector() should be a replacement of ReadModuleList in G4ExSimulator.
   It should call an specific detector class (type) from Framework/Detector/ and place it at a given
   position in a given mother volume.

  ***/
	return physWorld;
}

void
G4ExDetectorConstruction::CreateWorld()
{

	solidWorld 	= new G4Box("World", fWorldSizeX, fWorldSizeY, fWorldSizeZ);
	logicWorld = new G4LogicalVolume(solidWorld, Air, "World");
	physWorld	 =  new G4PVPlacement(nullptr, G4ThreeVector(), "World", logicWorld, 0, false, 0, fCheckOverlaps);

}


G4VPhysicalVolume* 
G4ExDetectorConstruction::Construct() 
{

	//SetDetectorParameters();

	if (!physWorld) {
		CreateElements();
		return CreateDetector();
	}
	return physWorld;

}

