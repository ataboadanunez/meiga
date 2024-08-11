#ifndef Detector_h
#define Detector_h
#include "OptDevice.h"

#include "G4SystemOfUnits.hh"
#include "G4LogicalVolume.hh"

#include <vector>
#include <map>
#include <boost/property_tree/ptree.hpp>

class Event;

/*
	declaration of Detector class

	contains methods for the definition of the detector components
	this class is intended to set all these parameters which then
	will be taken by the DetectorConstructor class in Geant4.

	a detector can be composed of several modules, and each module
	is composed by its enclosure, scintillator bars, WLS fibers, SiPMs, etc.

	all methods and functions related to the detector geometry and components
	should be written here and not in the Geant4 code. 

	author: alvaro taboada

*/

class Detector
{
	friend class ConfigManager;
	// friend class DataWriter;

	public:

		enum DetectorType {
			eUnknown = 0,
			eMusaic = 1,
			eMudulus = 2,
			eWCD = 3,
			eScintillator = 4,
			eHodoscope = 5,
			eSaltyWCD = 6,
			eDummy
		};


		Detector(){ ; }
		Detector(const int id, const DetectorType type);
		virtual ~Detector() = default;

public:
	void SetId(int id) { fDetectorId = id; }
	virtual unsigned int GetId() const { return fDetectorId; }
	
	void SetType(Detector::DetectorType type) { fType = type; }
	DetectorType GetType() const { return fType; }
	bool isValid() { return fType != DetectorType::eUnknown; }
	
	static DetectorType StringToType(const std::string name);
	static std::string TypeToString(const DetectorType aType);

	void SetName(const std::string name) { fName = name; }
	std::string GetName() const { return fName; }

	const std::vector<double>& GetDetectorPosition() const { return fDetectorPosition; }
	void SetDetectorPosition(const std::vector<double> &pos) { fDetectorPosition = pos; }

	// Optical device
	void MakeOptDevice(int id, OptDevice::DeviceType type);
	OptDevice& GetOptDevice(int id) { return fOptDeviceMap[id]; }
	OptDevice GetOptDevice(OptDevice::DeviceType type) { return OptDevice(type); }
	bool HasOptDevice(int id);
	
	std::map<int, OptDevice>& OptDeviceRange() { return fOptDeviceMap; }
	const std::map<int, OptDevice>& OptDeviceRange() const { return fOptDeviceMap; }
	int GetNOptDevice() const { return fNOptDevices; }

	virtual void BuildDetector(G4LogicalVolume *logMother, Event &aEvent, G4bool overlaps = false) = 0;
	virtual void SetDetectorProperties(const boost::property_tree::ptree &aTree);
	virtual void SetDefaultProperties();

	void SetLogicalVolume(std::string volName, G4LogicalVolume* log);
	G4LogicalVolume* GetLogicalVolume(std::string volName) { return fLogicalVolumeMap[volName]; }
	bool HasLogicalVolume(std::string volName);

	protected:
		unsigned int fDetectorId = 0;
		std::string fName;
		DetectorType fType;
		int fNOptDevices = 0;
		std::string fDefaultPropertiesFile;

		std::vector<double> fDetectorPosition;
		OptDevice fOptDevice;
		std::map<int, OptDevice> fOptDeviceMap;
		std::map<OptDevice::DeviceType, OptDevice> fOptDeviceMapT;
		std::map<std::string, G4LogicalVolume*> fLogicalVolumeMap;
		std::vector<std::string> fOverwrittenPropertiesVector;
};

#endif
