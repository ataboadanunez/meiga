#ifndef G4MDetectorConstruction_h
#define G4MDetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4PVPlacement.hh"

class Event;

class G4MDetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        G4MDetectorConstruction(Event& aEvent);
        virtual ~G4MDetectorConstruction() { };
        virtual G4VPhysicalVolume* Construct();

    private:
        G4PVPlacement*   fPhysWorld  = nullptr;
};

#endif