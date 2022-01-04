#include "Detector.h"

Detector
Detector::MakeOptDevice(int id, OptDevice::DeviceType type)
{
	Detector res;
	res.fOptDeviceMap.emplace(id, OptDevice(id, type));
	res.fNOptDevices = res.fOptDeviceMap.size();
	return res;
}

Detector
Detector::MakeOptDevice(OptDevice::DeviceType type)
{
	Detector res;
	res.fOptDeviceMapT.emplace(type, OptDevice(type));
	res.fNOptDevices = res.fOptDeviceMap.size();
  return res;
}