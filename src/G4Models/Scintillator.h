#ifndef Scintillator_h
#define Scintillator_h 1

// #include "G4SDManager.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"

#include "Detector.h"
#include "Materials.h"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4OpticalSkinSurface;
class Event;

class Scintillator : public Detector
{
	friend class Musaic;
	friend class Mudulus;
	friend class Hodoscope;

	// model class for a basic scintillator detector

	public:
		Scintillator(const int id, const Detector::DetectorType type);
		virtual void BuildDetector(G4LogicalVolume *logMother, Event& aEvent, G4bool overlaps = false) override;
	
		virtual void SetDefaultProperties() override;
		virtual void SetDetectorProperties(const boost::property_tree::ptree &aTree) override;
		
		int GetNPanels() const { return fNumberOfPanels; }
		void SetNPanels(int nPanels) { fNumberOfPanels = nPanels; }

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

		// mechanical properties
		double GetCasingThickness() const { return fCasingThickness; }
		void SetCasingThickness(double casingThickness) { fCasingThickness = casingThickness; }
		
		double GetRotationAngle() const { return fRotationAngle; }
		void SetRotationAngle(double angle);

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
		int    fNumberOfPanels;
		double fDistancePanels;
};

#endif
