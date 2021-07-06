#include "Particle.h"
#include "PhysicsConstants.h"
#include <sstream>
#include <cmath>

Particle::Particle(const int id, const double px,
                   const double py, const double pz,
                   const double x, const double y, const double z) : 
                  fId(id),
                  fPx(px),
                  fPy(py),
                  fPz(pz),
                  fX(x),
                  fY(y),
                  fZ(z)
                   { }

Particle::Particle(const int id, const std::vector<double>& position, const std::vector<double>& momentum) :
  fPosition(position),
  fDirection(momentum),
  fId(id)
  {
    SetMomentum(momentum);
    SetMass(id);
  }


void
Particle::SetMass(const int id)
{
  if (gParticleMassMap.empty())
    InitParticleMassMap();

  //const auto index = gParticleMassMap.find(id);
  //fMass = (index != gParticleMassMap.end()) ? index->second : 0. ;
  //return (index != gParticleMassMap.end()) ? index->second : 0.;
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
  std::cout << "[DEBUG] FRAMEWORK " << mass << std::endl; 
  fMomentum = sqrt(px*px + py*py + pz*pz);
  fKineticEnergy = sqrt(fMomentum*fMomentum + mass*mass) - mass;
}

int
Particle::NucleusCode(const unsigned int charge,
                      const unsigned int atomicNumber)
  {
    return
        kNucleusBase
      + kChargeFactor       * charge
      + kAtomicNumberFactor * atomicNumber;
  }
