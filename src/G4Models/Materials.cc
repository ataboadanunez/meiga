#include "Materials.h"

Materials::Materials() 
{
	CreateElements();
	CreateCompunds();
}

Materials::~Materials()
{

}

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
Materials::CreateCompunds()
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

	// air
	Air = new G4Material("Air", 1.29e-3 * g/cm3, 2);
	Air->AddElement(elN, 0.7);
	Air->AddElement(elO, 0.3);

	// standard rock
	StdRock = SiO2;

	// Scintillator material:
	PlasticScint = new G4Material("PlasticScint", 1.060 * g/cm3, 3);
	PlasticScint->AddMaterial(Polystyrene, 98.7*perCent);
	PlasticScint->AddMaterial(PPO, 1*perCent);
	PlasticScint->AddMaterial(POPOP, 0.3*perCent);

	// Scintillator Coating - 15% TiO2 and 85% polystyrene by weight.
	ScintCoating = new G4Material("ScintCoating", 1.52 * g/cm3, 2);
	ScintCoating->AddMaterial(TiO2, 15*perCent);
	ScintCoating->AddMaterial(Polystyrene, 85*perCent);

}