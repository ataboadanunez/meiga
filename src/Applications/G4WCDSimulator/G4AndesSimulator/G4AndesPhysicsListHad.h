#ifndef G4StationPhysicsListHad_h_
#define G4StationPhysicsListHad_h_

#include <G4VUserPhysicsList.hh>

class G4AndesPhysicsListHad : public G4VUserPhysicsList {

public:
  G4AndesPhysicsListHad();
  virtual ~G4AndesPhysicsListHad() = default;

protected:
  void ConstructParticle() override;
  void ConstructProcess() override;
  void SetCuts() override;

private:
  void SetupProcesses();
  void SetupProcessesHad();

  bool fFastCerenkov = false;

  // G4StationSimulator& fG4StationSimulator;

};



#endif
