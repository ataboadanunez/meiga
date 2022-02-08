#ifndef WCD_h
#define WCD_h 1

// framework classes
#include "Event.h"
#include "Detector.h"
#include "Materials.h"

// geant4 classes
#include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;

class WCD {
  // description

public:
  static void BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps = false);

private:
  WCD();
  virtual ~WCD();




};

#endif