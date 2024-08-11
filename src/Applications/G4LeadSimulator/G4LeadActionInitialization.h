#ifndef G4LeadActionInitialization_h
#define G4LeadActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Event;
class G4LeadDetectorConstruction;

class G4LeadActionInitialization : public G4VUserActionInitialization
{
    public:
        G4LeadActionInitialization(Event &aEvent);
        ~G4LeadActionInitialization() override = default;

        void BuildForMaster() const override;
        void Build() const override;

    private:
        Event &fEvent;

};

#endif