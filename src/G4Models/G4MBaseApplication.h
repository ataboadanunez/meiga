#ifndef G4MBASEAPPLICATION_H
#define G4MBASEAPPLICATION_H

#include <sstream>

#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

class Event;

class G4MBaseApplication {

public:
    virtual ~G4MBaseApplication() = default;

    virtual void Initialize(Event &aEvent, std::string aFileName);
    virtual bool RunSimulation(Event &aEvent) = 0;
    virtual void WriteEventInfo(Event &aEvent);

    void SetupManagers(Event &aEvent, G4UImanager &aUIManager, G4VisManager &aVisManager);
};

#endif // G4MBASEAPPLICATION_H
