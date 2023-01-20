#ifndef Detector_h
#define Detector_h

#include "OptDevice.h"
#include "DefaultProperties.h"

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
	public:

		enum DetectorType {
			eUnknown = 0,
			eMusaic = 1,
			eMudulus = 2,
			eWCD = 3,
			eScintillator = 4,
			eDummy
		};


		Detector(){ ; }
		Detector(const unsigned int id, const DetectorType type);
		virtual ~Detector() { }

		unsigned int GetId() const { return fDetectorId; }
		void SetId(int id) { fDetectorId = id; }

		DetectorType GetType() const { return fType; }
		void SetType(Detector::DetectorType type) { fType = type; }

		const std::vector<double>& GetDetectorPosition() const { return fDetectorPosition; }
		void SetDetectorPosition(const std::vector<double> &pos) { fDetectorPosition = pos; }

		// Mechanical properties
		double GetCasingThickness() const { return fCasingThickness; }
		void SetCasingThickness(double casingThickness) { fCasingThickness = casingThickness; }
		double GetRotationAngle() const { return fRotationAngle; }
		void SetRotationAngle(double angle);

		// scintillator-type detector properties
		int GetNBars() const { return fNumberOfBars; }
		void SetNBars(int nBars) { fNumberOfBars = nBars; }

		double GetBarLength() const { return fBarLength; }
		void SetBarLength(double barLength) { fBarLength = barLength; }
		
		double GetBarWidth() const { return fBarWidth; }
		void SetBarWidth(double barWidth) { fBarWidth = barWidth; }
		
		double GetBarThickness() const { return fBarThickness; }
		void SetBarThickness(double barThickness) { fBarThickness = barThickness; }
		
		double GetBarCoatingThickness() const { return fCoatingThickness; }
		void SetBarCoatingThickness(double barCoatThickness) { fCoatingThickness = barCoatThickness; }

		double GetDistanceBtwPanels() const { return fDistancePanels; }
		void SetDistanceBtwPanels( double dist) { fDistancePanels = dist; }

		// WLS fibers
		double GetFiberRadius() const { return fFiberRadius; }
		void SetFiberRadius(double fiberRad) { fFiberRadius = fiberRad; }

		double GetFiberLength() const { return fFiberLength; }
		void SetFiberLength(double fiberLength) { fFiberLength = fiberLength; }

		double GetCladdingThickness() const { return fCladdingThickness; }
		void SetCladdingThickness(double cladThickness) { fCladdingThickness = cladThickness; }

		// WCD 
		double GetTankHeight() const { return fTankHeight; }
		void SetTankHeight(double h) { fTankHeight = h; }
		
		double GetTankRadius() const { return fTankRadius; }
		void SetTankRadius(double r) { fTankRadius = r; }

		double GetTankThickness() const { return fTankThickness; }
		void SetTankThickness(double t) { fTankThickness = t; }

		// general detector size parameters
		double GetLength() const { return fLength; }
		void SetLength(double l) { fLength = l; }

		double GetWidth() const { return fWidth; }
		void SetWidth(double w) { fWidth = w; }

		double GetThickness() const { return fThickness; }
		void SetThickness(double t) { fThickness = t; }

		// ground size
		double GetGroundSizeX() const { return fGroundX; }
		void SetGroundSizeX(double gx) { fGroundX = gx; }

		double GetGroundSizeY() const { return fGroundY; }
		void SetGroundSizeY(double gy) { fGroundY = gy; }

		double GetGroundSizeZ() const { return fGroundZ; }
		void SetGroundSizeZ(double gz) { fGroundZ = gz; }

		// Optical device
		//OptDevice& GetOptDevice() { return fOptDevice; } // to access OptDevice class members
		// Make, Get and Has optical device by its id
		void MakeOptDevice(int id, OptDevice::DeviceType type);
		OptDevice& GetOptDevice(int id) { return fOptDeviceMap[id]; }
		bool HasOptDevice(int id);

		OptDevice GetOptDevice(OptDevice::DeviceType type) { return OptDevice(type); }
		
		std::map<int, OptDevice>& OptDeviceRange() { return fOptDeviceMap; }
		const std::map<int, OptDevice>& OptDeviceRange() const { return fOptDeviceMap; }
		int GetNOptDevice() const { return fNOptDevices; }
		
		// Setters & Getters for Geant4 logical volumes
		void SetLogicalVolume(std::string volName, G4LogicalVolume* log);
		G4LogicalVolume* GetLogicalVolume(std::string volName) { return fLogicalVolumeMap[volName]; }
		bool HasLogicalVolume(std::string volName);

		DetectorType StringToType(const std::string name);
		void SetDefaultProperties(const std::string file);
		void SetDetectorProperties(const boost::property_tree::ptree &det, DefaultProperties &defProp);
		//std::map<std::string, DetectorType> conversion;
	private:

		unsigned int fDetectorId = 0;
		DetectorType fType;
		int fNOptDevices = 0;

		// Scintillator bars

		double fBarWidth;
		double fBarLength;
		double fBarThickness;
		double fCasingThickness;
		double fRotationAngle;
		double fFiberLength;
		double fFiberRadius;
		double fCladdingThickness;
		double fCoatingThickness;
		int    fNumberOfBars;
		double fDistancePanels;

		// Water-Cerenkov Detector parameters
		double fTankHeight;
		double fTankRadius;
		double fTankThickness;

		// general detector size parameters
		double fLength;
		double fWidth;
		double fThickness;
		
		// ground size parameters
		double fGroundX;
		double fGroundY;
		double fGroundZ;

		std::vector<double> fDetectorPosition;
		OptDevice fOptDevice;
		std::map<int, OptDevice> fOptDeviceMap;
		std::map<OptDevice::DeviceType, OptDevice> fOptDeviceMapT;

		std::map<std::string, G4LogicalVolume*> fLogicalVolumeMap;
		// configuration file with detector properties
		std::string fDetectorProperties = "./DetectorProperties.xml";



};

void BuildDetector(G4LogicalVolume *logMother, Detector &det, Event &evt, G4bool overlaps = false);

#endif
