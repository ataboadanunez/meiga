#include "Particle.h"
#include "PhysicsConstants.h"

#include <sstream>
#include <cmath>

// particle mass defined as static
std::map<int, double> Particle::gParticleMassMap;
std::map<Particle::Type, Particle::Component> Particle::gComponentMap;

Particle::Particle(int id, std::vector<double>& position, std::vector<double>& momentum) :
  fId(id),
  fPosition(position),
  fDirection(momentum)
  {
  	SetMass(id);
    SetMomentum(momentum);
  }


void
Particle::SetMass(const int id)
{
  if (gParticleMassMap.empty())
    InitParticleMassMap();

  fMass = gParticleMassMap[id];
}

void
Particle::InsertParticleMass(const Particle::Type id, const double mass)
  {
    const auto ite = gParticleMassMap.insert(std::make_pair(id, mass));
    if (!ite.second) {
      std::ostringstream err;
      err << "Particle with Corsika id " << id << " already inserted.";
      throw err;
    }
  }

void
Particle::InitParticleMassMap()
{ 
  // Particle ID is the CORSIKA ID
  gParticleMassMap.emplace(1, 0);
  gParticleMassMap.emplace(2,  kElectronMass);
  gParticleMassMap.emplace(3,  kElectronMass);
  gParticleMassMap.emplace(5, kMuonMass);
  gParticleMassMap.emplace(6, kMuonMass);
  gParticleMassMap.emplace(7, kPiZeroMass);
  gParticleMassMap.emplace(8, kPiChargedMass);
  gParticleMassMap.emplace(9, kPiChargedMass);
  gParticleMassMap.emplace(10, kKaonZeroMass);
  gParticleMassMap.emplace(11, kKaonChargedMass);
  gParticleMassMap.emplace(12, kKaonChargedMass);
  gParticleMassMap.emplace(13, kNeutronMass);
  gParticleMassMap.emplace(14, kProtonMass);
  gParticleMassMap.emplace(15, kProtonMass);
  gParticleMassMap.emplace(16, kKaonZeroMass);
  gParticleMassMap.emplace(17, kEtaMass);
  gParticleMassMap.emplace(18, kLambdaMass);
  gParticleMassMap.emplace(19, kSigmaPlusMass);
  gParticleMassMap.emplace(20, kSigmaZeroMass);
  gParticleMassMap.emplace(21, kSigmaMinusMass);
  gParticleMassMap.emplace(22, kXiZeroMass);
  gParticleMassMap.emplace(23, kXiMinusMass);
  gParticleMassMap.emplace(24, kOmegaMinusMass);
  gParticleMassMap.emplace(25, kNeutronMass);
  gParticleMassMap.emplace(26, kLambdaMass);
  gParticleMassMap.emplace(27, kSigmaPlusMass);
  gParticleMassMap.emplace(28, kSigmaZeroMass);
  gParticleMassMap.emplace(29, kSigmaMinusMass);
  gParticleMassMap.emplace(30, kXiZeroMass);
  gParticleMassMap.emplace(31, kXiMinusMass);
  gParticleMassMap.emplace(32, kOmegaMinusMass);

  gParticleMassMap.emplace(49, kPhiMass);

  gParticleMassMap.emplace(50, kOmegaMesonMass);
  gParticleMassMap.emplace(51, kRhoZeroMass);
  gParticleMassMap.emplace(52, kRhoChargedMass);
  gParticleMassMap.emplace(53, kRhoChargedMass);
  gParticleMassMap.emplace(54, kDeltaMass);
  gParticleMassMap.emplace(55, kDeltaMass);
  gParticleMassMap.emplace(56, kDeltaMass);
  gParticleMassMap.emplace(57, kDeltaMass);
  gParticleMassMap.emplace(58, kDeltaMass);
  gParticleMassMap.emplace(59, kDeltaMass);
  gParticleMassMap.emplace(60, kDeltaMass);
  gParticleMassMap.emplace(61, kDeltaMass);
  gParticleMassMap.emplace(62, kKaonStarZeroMass);
  gParticleMassMap.emplace(63, kKaonStarChargedMass);
  gParticleMassMap.emplace(64, kKaonStarChargedMass);
  gParticleMassMap.emplace(65, kKaonStarZeroMass);
  gParticleMassMap.emplace(66, 0.);
  gParticleMassMap.emplace(67, 0.);
  gParticleMassMap.emplace(68, 0.);
  gParticleMassMap.emplace(69, 0.);

  gParticleMassMap.emplace(71, kEtaMass);
  gParticleMassMap.emplace(72, kEtaMass);
  gParticleMassMap.emplace(73, kEtaMass);
  gParticleMassMap.emplace(74, kEtaMass);

  gParticleMassMap.emplace(95, kMuonMass);
  gParticleMassMap.emplace(96, kMuonMass);
  
}

void
Particle::SetMomentum(const std::vector<double>& momentum)
{
  const double px = momentum[0];
  const double py = momentum[1];
  const double pz = momentum[2];
  const double mass = GetMass();
  
  fDirection = momentum;
  fMomentum = sqrt(px*px + py*py + pz*pz);
  fKineticEnergy = sqrt(fMomentum*fMomentum + mass*mass) - mass;  
}


int
Particle::NucleusCode(const unsigned int charge,
                      const unsigned int atomicNumber)
{
  return kNucleusBase + kChargeFactor * charge + kAtomicNumberFactor * atomicNumber;
}


void
Particle::InsertComponent(const Particle::Type type, const Particle::Component comp)
{
  const auto ite = gComponentMap.insert(std::make_pair(type, comp));
  if (!ite.second) {
    std::ostringstream err;
    err << "Particle with type " << type << " already inserted.";
    throw err;
  }

}

void 
Particle::InitComponentMap()
{
  InsertComponent(Type::eUndefined, Component::eUnknown);
  // electromagnetic component
  InsertComponent(Type::ePhoton, Component::eElectromagnetic);
  InsertComponent(Type::eElectron, Component::eElectromagnetic);
  InsertComponent(Type::ePositron, Component::eElectromagnetic);
  // muonic component
  InsertComponent(Type::eMuon, Component::eMuonic);
  InsertComponent(Type::eAntiMuon, Component::eMuonic);
  InsertComponent(Type::eDecayedMuon, Component::eMuonic);
  InsertComponent(Type::eDecayedAntiMuon, Component::eMuonic);
  // hadronic component 
  InsertComponent(Type::ePiZero, Component::eHadronic);
  InsertComponent(Type::ePiPlus, Component::eHadronic);
  InsertComponent(Type::ePiMinus, Component::eHadronic);
  InsertComponent(Type::eKaon0L, Component::eHadronic);
  InsertComponent(Type::eKaonPlus, Component::eHadronic);
  InsertComponent(Type::eKaonMinus, Component::eHadronic);
  
  InsertComponent(Type::eNeutron, Component::eHadronic);
  InsertComponent(Type::eProton, Component::eHadronic);
  InsertComponent(Type::eAntiProton, Component::eHadronic);
  InsertComponent(Type::eKaon0S, Component::eHadronic);
  InsertComponent(Type::eEta, Component::eHadronic);
  InsertComponent(Type::eLambda, Component::eHadronic);
  InsertComponent(Type::eSigmaPlus, Component::eHadronic);
  InsertComponent(Type::eSigmaZero, Component::eHadronic);
  InsertComponent(Type::eSigmaMinus, Component::eHadronic);
  InsertComponent(Type::eXiZero, Component::eHadronic);
  InsertComponent(Type::eXiMinus, Component::eHadronic);
  InsertComponent(Type::eOmegaMinus, Component::eHadronic);
  InsertComponent(Type::eAntiNeutron, Component::eHadronic);
  InsertComponent(Type::eAntiLambda, Component::eHadronic);
  InsertComponent(Type::eAntiSigmaPlus, Component::eHadronic);
  InsertComponent(Type::eAntiSigmaZero, Component::eHadronic);
  InsertComponent(Type::eAntiSigmaMinus, Component::eHadronic);
  InsertComponent(Type::eAntiXiZero, Component::eHadronic);
  InsertComponent(Type::eAntiXiMinus, Component::eHadronic);
  InsertComponent(Type::eAntiOmegaMinus, Component::eHadronic);

  InsertComponent(Type::ePhi, Component::eHadronic);
  InsertComponent(Type::eOmegaMeson, Component::eHadronic);
  InsertComponent(Type::eRhoZero, Component::eHadronic);
  InsertComponent(Type::eRhoPlus, Component::eHadronic);
  InsertComponent(Type::eRhoMinus, Component::eHadronic);
  InsertComponent(Type::eDeltaPlusPlus, Component::eHadronic);
  InsertComponent(Type::eDeltaPlus, Component::eHadronic);
  InsertComponent(Type::eDeltaZero, Component::eHadronic);
  InsertComponent(Type::eDeltaMinus, Component::eHadronic);
  InsertComponent(Type::eAntiDeltaMinusMinus, Component::eHadronic);
  InsertComponent(Type::eAntiDeltaMinus, Component::eHadronic);
  InsertComponent(Type::eAntiDeltaZero, Component::eHadronic);
  InsertComponent(Type::eAntiDeltaPlus, Component::eHadronic);
  InsertComponent(Type::eKaonStarZero, Component::eHadronic);
  InsertComponent(Type::eKaonStarPlus, Component::eHadronic);
  InsertComponent(Type::eAntiKaonStarMinus, Component::eHadronic);
  InsertComponent(Type::eAntiKaonStarZero, Component::eHadronic);
  // hadronic? muonic? em?... would be nice to detect Nu though... ;)
  InsertComponent(Type::eNuElectron, Component::eHadronic);
  InsertComponent(Type::eAntiNuElectron, Component::eHadronic);
  InsertComponent(Type::eNuMuon, Component::eHadronic);
  InsertComponent(Type::eAntiNuMuon, Component::eHadronic);
  
}

Particle::Component
Particle::GetComponent(const Particle::Type type)
{
  if (gComponentMap.empty())
    InitComponentMap();
  const auto index = gComponentMap.find(type);
  return (index != gComponentMap.end()) ? static_cast<Particle::Component>(index->second) : Particle::Component::eUnknown;
}