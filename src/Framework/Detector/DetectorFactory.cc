#include "DetectorFactory.h"
#include "WCD.h"
#include "Scintillator.h"
#include "Hodoscope.h"
#include "Mudulus.h"
#include "Musaic.h"
#include "SaltyWCD.h"
#include "Dummy.h"

std::unique_ptr<Detector> 
DetectorFactory::CreateDetector(const int aId, const Detector::DetectorType aType)
{
    switch(aType) {
        case Detector::eWCD:
            return std::make_unique<WCD>(aId, aType);
        case Detector::eSaltyWCD:
            return std::make_unique<SaltyWCD>(aId, aType);
        case Detector::eScintillator:
            return std::make_unique<Scintillator>(aId, aType);
        case Detector::eHodoscope:
            return std::make_unique<Hodoscope>(aId, aType);
        case Detector::eMudulus:
            return std::make_unique<Mudulus>(aId, aType);
        case Detector::eDummy:
            return std::make_unique<Dummy>(aId, aType);
    }
    throw std::invalid_argument("Invalid detector type.");
}
