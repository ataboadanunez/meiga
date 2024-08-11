#ifndef G4MuDecActionInitialization_h
#define G4MuDecActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Event;
class G4MuDecDetectorConstruction;

class G4MuDecActionInitialization : public G4VUserActionInitialization
{
    public:
        G4MuDecActionInitialization(Event &aEvent);
        ~G4MuDecActionInitialization() override = default;

        void BuildForMaster() const override;
        void Build() const override;

    private:
        Event &fEvent;

};

#endif