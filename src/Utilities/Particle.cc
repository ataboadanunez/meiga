#include "Particle.h"
#include "PhysicsConstants.h"

#include <sstream>
#include <cmath>
#include <boost/format.hpp>

using boost::format;
using boost::io::str;


// particle mass defined as static
std::map<int, double> Particle::gParticleMassMap;
//std::map<Particle::Type, Particle::Component> Particle::gComponentMap;
std::map<int, Particle::Component> Particle::gComponentMap;
std::map<int, std::string> Particle::gParticleNameMap;

const char* gNuclearNames[] = {
    "neutron",
    "H",
    "He",
    "Li",
    "Be",
    "B",
    "C",
    "N",
    "O",
    "F",
    "Ne",
    "Na",
    "Mg",
    "Al",
    "Si",
    "P",
    "S",
    "Cl",
    "Ar",
    "K",
    "Ca",
    "Sc",
    "Ti",
    "V",
    "Cr",
    "Mn",
    "Fe"
  };

//unsigned int kMaxNuclearNameIndex = utl::Length(gNuclearNames) - 1;
unsigned int kMaxNuclearNameIndex = 26;

Particle::Particle(int id, std::vector<double>& position, std::vector<double>& momentum) :
  fId(id),
  fPosition(position)
  
  {
    SetMass();
    SetMomentumDirection(momentum);
    SetName();
  }


void
Particle::SetMass()
{
  
  if (gParticleMassMap.empty())
    InitParticleMassMap();

  // Set mass value 
  fMass = IsNucleus() ? (GetMassNumber() * kAtomicMassUnit) : gParticleMassMap[fId];
  
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
  // Particle ID is the PDG Encoding

  gParticleMassMap.emplace(Particle::ePhoton, 0);
  gParticleMassMap.emplace(Particle::ePositron,  kElectronMass);
  gParticleMassMap.emplace(Particle::eElectron,  kElectronMass);
  gParticleMassMap.emplace(Particle::eAntiMuon, kMuonMass);
  gParticleMassMap.emplace(Particle::eMuon, kMuonMass);
  gParticleMassMap.emplace(Particle::ePiZero, kPiZeroMass);
  gParticleMassMap.emplace(Particle::ePiPlus, kPiChargedMass);
  gParticleMassMap.emplace(Particle::ePiMinus, kPiChargedMass);
  gParticleMassMap.emplace(Particle::eKaon0L, kKaonZeroMass);
  gParticleMassMap.emplace(Particle::eKaonPlus, kKaonChargedMass);
  gParticleMassMap.emplace(Particle::eKaonMinus, kKaonChargedMass);
  gParticleMassMap.emplace(Particle::eNeutron, kNeutronMass);
  gParticleMassMap.emplace(Particle::eProton, kProtonMass);
  gParticleMassMap.emplace(Particle::eAntiProton, kProtonMass);
  gParticleMassMap.emplace(Particle::eKaon0S, kKaonZeroMass);
  gParticleMassMap.emplace(Particle::eEta, kEtaMass);
  gParticleMassMap.emplace(Particle::eLambda, kLambdaMass);
  gParticleMassMap.emplace(Particle::eSigmaPlus, kSigmaPlusMass);
  gParticleMassMap.emplace(Particle::eSigmaZero, kSigmaZeroMass);
  gParticleMassMap.emplace(Particle::eSigmaMinus, kSigmaMinusMass);
  gParticleMassMap.emplace(Particle::eXiZero, kXiZeroMass);
  gParticleMassMap.emplace(Particle::eXiMinus, kXiMinusMass);
  gParticleMassMap.emplace(Particle::eOmegaMinus, kOmegaMinusMass);
  gParticleMassMap.emplace(Particle::eAntiNeutron, kNeutronMass);
  gParticleMassMap.emplace(Particle::eAntiLambda, kLambdaMass);
  gParticleMassMap.emplace(Particle::eAntiSigmaPlus, kSigmaPlusMass);
  gParticleMassMap.emplace(Particle::eAntiSigmaZero, kSigmaZeroMass);
  gParticleMassMap.emplace(Particle::eAntiSigmaMinus, kSigmaMinusMass);
  gParticleMassMap.emplace(Particle::eAntiXiZero, kXiZeroMass);
  gParticleMassMap.emplace(Particle::eAntiXiMinus, kXiMinusMass);
  gParticleMassMap.emplace(Particle::eAntiOmegaMinus, kOmegaMinusMass);

  gParticleMassMap.emplace(Particle::ePhi, kPhiMass);

  gParticleMassMap.emplace(Particle::eOmegaMeson, kOmegaMesonMass);
  gParticleMassMap.emplace(Particle::eRhoZero, kRhoZeroMass);
  gParticleMassMap.emplace(Particle::eRhoPlus, kRhoChargedMass);
  gParticleMassMap.emplace(Particle::eRhoMinus, kRhoChargedMass);
  gParticleMassMap.emplace(Particle::eDeltaPlusPlus, kDeltaMass);
  gParticleMassMap.emplace(Particle::eDeltaPlus, kDeltaMass);
  gParticleMassMap.emplace(Particle::eDeltaZero, kDeltaMass);
  gParticleMassMap.emplace(Particle::eDeltaMinus, kDeltaMass);
  gParticleMassMap.emplace(Particle::eAntiDeltaMinusMinus, kDeltaMass);
  gParticleMassMap.emplace(Particle::eAntiDeltaMinus, kDeltaMass);
  gParticleMassMap.emplace(Particle::eAntiDeltaZero, kDeltaMass);
  gParticleMassMap.emplace(Particle::eAntiDeltaPlus, kDeltaMass);
  gParticleMassMap.emplace(Particle::eKaonStarZero, kKaonStarZeroMass);
  gParticleMassMap.emplace(Particle::eKaonStarPlus, kKaonStarChargedMass);
  gParticleMassMap.emplace(Particle::eAntiKaonStarMinus, kKaonStarChargedMass);
  gParticleMassMap.emplace(Particle::eAntiKaonStarZero, kKaonStarZeroMass);

  gParticleMassMap.emplace(Particle::eNuElectron, 0.);
  gParticleMassMap.emplace(Particle::eAntiNuElectron, 0.);
  gParticleMassMap.emplace(Particle::eNuMuon, 0.);
  gParticleMassMap.emplace(Particle::eAntiNuMuon, 0.);

  gParticleMassMap.emplace(Particle::eEta, kEtaMass);
  
  gParticleMassMap.emplace(Particle::eDecayedAntiMuon, kMuonMass);
  gParticleMassMap.emplace(Particle::eDecayedMuon, kMuonMass);
  
}

void 
Particle::InitParticleNameMap()
{

  gParticleNameMap.emplace(Particle::eUndefined, "Undefined");
  gParticleNameMap.emplace(Particle::eElectron, "e-");
  gParticleNameMap.emplace(Particle::ePositron, "e+");
  gParticleNameMap.emplace(Particle::eNuElectron, "nu_e");
  gParticleNameMap.emplace(Particle::eAntiNuElectron, "anti_nu_e");
  gParticleNameMap.emplace(Particle::eMuon, "mu-");
  gParticleNameMap.emplace(Particle::eAntiMuon, "mu+");
  gParticleNameMap.emplace(Particle::eNuMuon, "nu_mu");
  gParticleNameMap.emplace(Particle::eAntiNuMuon, "anti_nu_mu");
  gParticleNameMap.emplace(Particle::eTau, "tau-");
  gParticleNameMap.emplace(Particle::eAntiTau, "tau+");
  gParticleNameMap.emplace(Particle::eNuTau, "nu_tau");
  gParticleNameMap.emplace(Particle::eAntiNuTau, "anti_nu_tau");
  gParticleNameMap.emplace(Particle::ePhoton, "gamma");
  gParticleNameMap.emplace(Particle::eDecayedMuon, "~mu-");
  gParticleNameMap.emplace(Particle::eDecayedAntiMuon, "~mu+");
  gParticleNameMap.emplace(Particle::ePiZero, "pi0");
  gParticleNameMap.emplace(Particle::eRhoZero, "rho0");
  gParticleNameMap.emplace(Particle::ePiPlus, "pi+");
  gParticleNameMap.emplace(Particle::ePiMinus, "pi-");
  gParticleNameMap.emplace(Particle::eRhoPlus, "rho+");
  gParticleNameMap.emplace(Particle::eRhoMinus, "rho+");
  gParticleNameMap.emplace(Particle::eEta, "eta");
  gParticleNameMap.emplace(Particle::eOmegaMeson, "omega_meson");
  gParticleNameMap.emplace(Particle::eKaon0L, "kaon0L");
  gParticleNameMap.emplace(Particle::eKaon0S, "kaon0S");
  gParticleNameMap.emplace(Particle::eKaonStarZero, "kaon*0");
  gParticleNameMap.emplace(Particle::eAntiKaonStarZero, "anti_kaon*0");
  gParticleNameMap.emplace(Particle::eKaonPlus, "kaon+");
  gParticleNameMap.emplace(Particle::eKaonMinus, "kaon-");
  gParticleNameMap.emplace(Particle::eKaonStarPlus, "kaon*+");
  gParticleNameMap.emplace(Particle::eAntiKaonStarMinus, "anti_kaon*-");
  gParticleNameMap.emplace(Particle::ePhi, "phi");
  gParticleNameMap.emplace(Particle::eDeltaMinus, "delta-");
  gParticleNameMap.emplace(Particle::eAntiDeltaPlus, "anti_delta+");
  gParticleNameMap.emplace(Particle::eNeutron, "neutron");
  gParticleNameMap.emplace(Particle::eAntiNeutron, "anti_neutron");
  gParticleNameMap.emplace(Particle::eDeltaZero, "delta0");
  gParticleNameMap.emplace(Particle::eAntiDeltaZero, "anti_delta0");
  gParticleNameMap.emplace(Particle::eProton, "proton");
  gParticleNameMap.emplace(Particle::eAntiProton, "anti_proton");
  gParticleNameMap.emplace(Particle::eDeltaPlus, "delta+");
  gParticleNameMap.emplace(Particle::eAntiDeltaMinus, "anti_delta-");
  gParticleNameMap.emplace(Particle::eDeltaPlusPlus, "delta++");
  gParticleNameMap.emplace(Particle::eAntiDeltaMinusMinus, "anti_delta--");
  gParticleNameMap.emplace(Particle::eLambda, "lambda");
  gParticleNameMap.emplace(Particle::eAntiLambda, "anti_lambda");
  gParticleNameMap.emplace(Particle::eSigmaPlus, "sigma+");
  gParticleNameMap.emplace(Particle::eSigmaZero, "sigma0");
  gParticleNameMap.emplace(Particle::eSigmaMinus, "sigma-");
  gParticleNameMap.emplace(Particle::eAntiSigmaPlus, "anti_sigma+");
  gParticleNameMap.emplace(Particle::eAntiSigmaZero, "anti_sigma0");
  gParticleNameMap.emplace(Particle::eAntiSigmaMinus, "anti_sigma-");
  gParticleNameMap.emplace(Particle::eXiZero, "xi0");
  gParticleNameMap.emplace(Particle::eXiMinus, "xi-");
  gParticleNameMap.emplace(Particle::eAntiXiZero, "anti_xi0");
  gParticleNameMap.emplace(Particle::eAntiXiMinus, "anti_xi-");
  gParticleNameMap.emplace(Particle::eOmegaMinus, "omega-");
  gParticleNameMap.emplace(Particle::eAntiOmegaMinus, "anti_omega-");
  gParticleNameMap.emplace(Particle::eLambdac, "lambda_c+");

}

void
Particle::SetMomentumDirection(const std::vector<double>& momentum)
{
  const double px = momentum[0];
  const double py = momentum[1];
  const double pz = momentum[2];
  const double mass = GetMass();
  
  fMomentumDirection = momentum;
  fMomentum = sqrt(px*px + py*py + pz*pz);
  fKineticEnergy = sqrt(fMomentum*fMomentum + mass*mass) - mass;  
}


int
Particle::NucleusCode(const unsigned int charge,
                      const unsigned int atomicNumber)
{
  return kNucleusBase + kChargeFactor * charge + kAtomicNumberFactor * atomicNumber;
}

bool
Particle::IsNucleus()
{
  return kNucleusBase < fId && fId < kNucleusMax;
}

void
Particle::SetName()
{

  if (IsNucleus()) { 
    
    const unsigned int z = (fId / kChargeFactor) % kChargeMask;
    const unsigned int a = (fId / kAtomicNumberFactor) % kAtomicNumberMask; 

    fName =  (z <= kMaxNuclearNameIndex) ?
      str(format("%1% %2%") % gNuclearNames[z] % a) :
      str(format("N(%1%) %2%") % z % a);
  }

  else {
    
    if (gParticleNameMap.empty())
      InitParticleNameMap();
    
    fName = gParticleNameMap[fId];
  }
  
}


double
Particle::GetMassNumber()
{

  double A = IsNucleus() ? (fId / kAtomicNumberFactor) % kAtomicNumberMask : 0;
  
  return A;
}

double
Particle::GetAtomicNumber()
{

  double Z = IsNucleus() ? (fId / kChargeFactor) % kChargeMask : 0;

  return Z;

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
Particle::GetComponent(/*const Particle::Type type*/)
{
  if (gComponentMap.empty())
    InitComponentMap();
  
  if (IsNucleus()) {
    return Particle::Component::eHadronic;
  }

  else {
    
    const auto index = gComponentMap.find(fId);
    
    return (index != gComponentMap.end()) ? static_cast<Particle::Component>(index->second) : Particle::Component::eUnknown;
    
  }


  
}

std::string
Particle::GetComponentName(Particle::Component type)
{
  
  if (type == Particle::eElectromagnetic)
    return "eElectromagnetic";
  else if (type == Particle::eMuonic)
    return "eMuonic";
  else if (type == Particle::eHadronic)
    return "eHadronic";
  else if (type == Particle::eMuonDecay)
    return "eMuonDecay";
  else 
    return "eUnknown";
  
}

