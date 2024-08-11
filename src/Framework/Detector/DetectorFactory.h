// DetectorFactory.h
#ifndef DETECTOR_FACTORY_H
#define DETECTOR_FACTORY_H

#include <memory>
#include "Logger.h"
#include "Detector.h"

class DetectorFactory {
public:

    static std::unique_ptr<Detector> CreateDetector(const int aId, const Detector::DetectorType aType);
};

#endif // DETECTOR_FACTORY_H
