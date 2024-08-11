#ifndef G4ExActionInitialization_h
#define G4ExActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Event;
class G4ExDetectorConstruction;

class G4ExActionInitialization : public G4VUserActionInitialization
{
    public:
        G4ExActionInitialization(Event &aEvent);
        ~G4ExActionInitialization() override = default;

        void BuildForMaster() const override;
        void Build() const override;

    private:
        Event &fEvent;

};

#endif