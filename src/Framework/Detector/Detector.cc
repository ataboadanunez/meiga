#include "Detector.h"

// Detector
// Detector::MakeOptDevice(int id, OptDevice::DeviceType type)
// {
// 	Detector res;
// 	res.fOptDeviceMap.emplace(id, OptDevice(id, type));
// 	res.fNOptDevices = res.fOptDeviceMap.size();
// 	return res;
// }

void
Detector::MakeOptDevice(int id, OptDevice::DeviceType type)
{
	fOptDeviceMap.emplace(id, OptDevice(id, type));
	fNOptDevices = fOptDeviceMap.size();
}