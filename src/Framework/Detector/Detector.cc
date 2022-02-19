#include "Detector.h"
#include "Mudulus.h"
#include "Musaic.h"
#include "WCD.h"

#include <map>
#include <iostream>
using std::map;
using namespace std;

// Detector
// Detector::MakeOptDevice(int id, OptDevice::DeviceType type)
// {
// 	Detector res;
// 	res.fOptDeviceMap.emplace(id, OptDevice(id, type));
// 	res.fNOptDevices = res.fOptDeviceMap.size();
// 	return res;
// }


//AS: Mapa que va de enum Detector::DetectorType a función BuildDetector(G4LogicalVolume* logMother, Detector& detector, Event& theEvent, G4bool fCheckOverlaps = false);
static map<enum Detector::DetectorType, void(*)(G4LogicalVolume*, Detector&, Event&, G4bool)> enumToBuild;

static void initEnumToBuild() {
	static bool isInitialized = false;
	if (isInitialized) return;
	isInitialized = true;
	enumToBuild[Detector::eMudulus] = Mudulus::BuildDetector;
	enumToBuild[Detector::eMusaic]  = Musaic::BuildDetector;
	enumToBuild[Detector::eWCD]			= WCD::BuildDetector;
}

void BuildDetector(G4LogicalVolume *logMother, Detector &det, Event &evt, G4bool overlaps) {
	initEnumToBuild();
	try {
		enumToBuild[det.GetType()](logMother, det, evt, overlaps);
	}
	catch (std::out_of_range &e) {
		std::cerr << "No BuildDetector for DetectorType " << det.GetType() << std::endl;
	}
}

void
Detector::MakeOptDevice(int id, OptDevice::DeviceType type)
{
	fOptDeviceMap.emplace(id, OptDevice(id, type));
	fNOptDevices = fOptDeviceMap.size();
}

bool
Detector::HasOptDevice(int id)
{
	return fOptDeviceMap.find(id) != fOptDeviceMap.end();
}

void
Detector::SetLogicalVolume(string volName, G4LogicalVolume* log)
{
	fLogicalVolumeMap.emplace(volName, log);
}

bool
Detector::HasLogicalVolume(string volName)
{	
	return fLogicalVolumeMap.find(volName) != fLogicalVolumeMap.end();
}