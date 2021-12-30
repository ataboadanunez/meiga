#include "Detector.h"

Detector
Detector::MakeOptDevice(unsigned int id, OptDevice::DeviceType type)
{
	Detector res;
	res.fOptDeviceMap.emplace(id, OptDevice(id, type));
	res.fNOptDevices = res.fOptDeviceMap.size();
  return res;
}