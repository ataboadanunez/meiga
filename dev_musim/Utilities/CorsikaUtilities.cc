#include "CorsikaUtilities.h"
#include "Particle.h"

#include <map>
#include <sstream>
using std::ostringstream;
using std::map;

namespace {

  map<int, int> gCorsikaToPDGMap;


  void
  InsertCorsikaToPDG(const int corsikaCode, const int pdgCode)
  {
    const auto ite = gCorsikaToPDGMap.insert(std::make_pair(corsikaCode, pdgCode));
    if (!ite.second) {
      ostringstream err;
      err << "Particle with Corsika code " << corsikaCode << " already inserted.";
      throw err;
    }
  }


  void
  InitCorsikaToPDGMap()
  {
    InsertCorsikaToPDG(1, Particle::ePhoton);
    InsertCorsikaToPDG(2, Particle::ePositron);
    InsertCorsikaToPDG(3, Particle::eElectron);
    //                 4, none
    InsertCorsikaToPDG(5, Particle::eAntiMuon);
    InsertCorsikaToPDG(6, Particle::eMuon);
    InsertCorsikaToPDG(7, Particle::ePiZero);
    InsertCorsikaToPDG(8, Particle::ePiPlus);
    InsertCorsikaToPDG(9, Particle::ePiMinus);

    InsertCorsikaToPDG(10, Particle::eKaon0L);
    InsertCorsikaToPDG(11, Particle::eKaonPlus);
    InsertCorsikaToPDG(12, Particle::eKaonMinus);
    InsertCorsikaToPDG(13, Particle::eNeutron);
    InsertCorsikaToPDG(14, Particle::eProton);
    InsertCorsikaToPDG(15, Particle::eAntiProton);
    InsertCorsikaToPDG(16, Particle::eKaon0S);
    InsertCorsikaToPDG(17, Particle::eEta);
    InsertCorsikaToPDG(18, Particle::eLambda);
    InsertCorsikaToPDG(19, Particle::eSigmaPlus);
    InsertCorsikaToPDG(20, Particle::eSigmaZero);
    InsertCorsikaToPDG(21, Particle::eSigmaMinus);
    InsertCorsikaToPDG(22, Particle::eXiZero);
    InsertCorsikaToPDG(23, Particle::eXiMinus);
    InsertCorsikaToPDG(24, Particle::eOmegaMinus);
    InsertCorsikaToPDG(25, Particle::eAntiNeutron);
    InsertCorsikaToPDG(26, Particle::eAntiLambda);
    InsertCorsikaToPDG(27, Particle::eAntiSigmaPlus);
    InsertCorsikaToPDG(28, Particle::eAntiSigmaZero);
    InsertCorsikaToPDG(29, Particle::eAntiSigmaMinus);
    InsertCorsikaToPDG(30, Particle::eAntiXiZero);
    InsertCorsikaToPDG(31, Particle::eAntiXiMinus);
    InsertCorsikaToPDG(32, Particle::eAntiOmegaMinus);

    InsertCorsikaToPDG(49, Particle::ePhi);

    InsertCorsikaToPDG(50, Particle::eOmegaMeson);
    InsertCorsikaToPDG(51, Particle::eRhoZero);
    InsertCorsikaToPDG(52, Particle::eRhoPlus);
    InsertCorsikaToPDG(53, Particle::eRhoMinus);
    InsertCorsikaToPDG(54, Particle::eDeltaPlusPlus);
    InsertCorsikaToPDG(55, Particle::eDeltaPlus);
    InsertCorsikaToPDG(56, Particle::eDeltaZero);
    InsertCorsikaToPDG(57, Particle::eDeltaMinus);
    InsertCorsikaToPDG(58, Particle::eAntiDeltaMinusMinus);
    InsertCorsikaToPDG(59, Particle::eAntiDeltaMinus);
    InsertCorsikaToPDG(60, Particle::eAntiDeltaZero);
    InsertCorsikaToPDG(61, Particle::eAntiDeltaPlus);
    InsertCorsikaToPDG(62, Particle::eKaonStarZero);
    InsertCorsikaToPDG(63, Particle::eKaonStarPlus);
    InsertCorsikaToPDG(64, Particle::eAntiKaonStarMinus);
    InsertCorsikaToPDG(65, Particle::eAntiKaonStarZero);
    InsertCorsikaToPDG(66, Particle::eNuElectron);
    InsertCorsikaToPDG(67, Particle::eAntiNuElectron);
    InsertCorsikaToPDG(68, Particle::eNuMuon);
    InsertCorsikaToPDG(69, Particle::eAntiNuMuon);
    //                 70, none
    InsertCorsikaToPDG(71, Particle::eEta);
    InsertCorsikaToPDG(72, Particle::eEta);
    InsertCorsikaToPDG(73, Particle::eEta);
    InsertCorsikaToPDG(74, Particle::eEta);

    InsertCorsikaToPDG(95, Particle::eDecayedAntiMuon);
    InsertCorsikaToPDG(96, Particle::eDecayedMuon);
  }

}


int
Corsika::CorsikaToPDG(const int corsikaCode)
{
  if (corsikaCode < 100) {

    if (gCorsikaToPDGMap.empty())
      InitCorsikaToPDGMap();

    const auto index = gCorsikaToPDGMap.find(corsikaCode);

    return (index != gCorsikaToPDGMap.end()) ? index->second : Particle::eUndefined;

  } else if (corsikaCode < 9900) {                   // nucleus

    const unsigned int z = corsikaCode % 100;
    const unsigned int a = corsikaCode / 100;
    return Particle::NucleusCode(z, a);

  } else                                             // Cherenkov
    return Particle::eUndefined;
}
