#ifndef G4HodoscopeActionInitialization_h
#define G4HodoscopeActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

class Event;
class G4HodoscopeDetectorConstruction;

class G4HodoscopeActionInitialization : public G4VUserActionInitialization
{
    public:
        G4HodoscopeActionInitialization(Event &aEvent);
        ~G4HodoscopeActionInitialization() override = default;

        void BuildForMaster() const override;
        void Build() const override;

    private:
        Event &fEvent;

};

#endif