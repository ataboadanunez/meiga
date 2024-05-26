#ifndef G4MBASEAPPLICATION_H
#define G4MBASEAPPLICATION_H

#include <sstream>

class Event;

class G4MBaseApplication {

public:
    virtual ~G4MBaseApplication() = default;

    virtual void Initialize(Event &aEvent, std::string aFileName);
    virtual bool RunSimulation(Event &aEvent) = 0;
    virtual void WriteEventInfo(Event &aEvent);
};

#endif // G4MBASEAPPLICATION_H
