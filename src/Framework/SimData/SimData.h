#ifndef SimData_h
#define SimData_h

#include "Particle.h"
#include "DetectorSimData.h"
#include <string>
#include <vector>
#include <map>


class SimData
{
	public:
		SimData();
		~SimData();

		enum class InputMode {
			eUnknown = 0,
			eUseARTI = 1,
			eUseEcoMug = 2,

		};

		enum class SimulationMode {
			eFull = 0,
			eFast = 1
		};

		enum class InjectionMode {
			eUnknown = 0,
			eCircle = 1,
			eHalfSphere = 2,
			eVertical = 3,
			// specific case when particles are injected inside the detector (e.g. muon decay inside WCD)
			eInsideDetector
		};


		typedef std::vector<Particle> ParticleVector;
		const ParticleVector& GetParticleVector() const { return fParticles; }
		ParticleVector& GetParticleVector()  { return fParticles; }
		void InsertParticle(Particle& particle) { fParticles.push_back(particle); }

		unsigned int GetTotalNumberOfParticles() const { return fTotalNumberOfParticles; }
		void SetTotalNumberOfParticles(const unsigned int n) { fTotalNumberOfParticles = n; }

		Particle& GetCurrentParticle() { return fCurrentParticle; }
		void SetCurrentParticle(Particle particle) { fCurrentParticle = particle; }

		typedef std::vector<Particle> InjectedParticleVector;
		ParticleVector& GetInjectedParticleVector() { return fInjectedParticles; }
		void InsertInjectedParticle(Particle& particle) { fInjectedParticles.push_back(particle); }

		// Input mode
		InputMode InputModeConversion(std::string name);
		void SetInputMode(const InputMode inpMode) { fInputMode = inpMode; }
		InputMode GetInputMode() const { return fInputMode; }
		std::string GetInputModeName() const { return fInputModeName; }

		// Simulation mode
		SimulationMode SimulationModeConversion(std::string name);
		void SetSimulationMode(const SimulationMode simMode) { fSimulationMode = simMode; }
		SimulationMode GetSimulationMode() const { return fSimulationMode; }
		std::string GetSimulationModeName() const { return fSimulationModeName; }

		// Visualization settings
		void SetVisualizationGeometry(const bool geovis) { fGeoVis = geovis; }
		bool VisualizeGeometry() const { return fGeoVis; }
		
		void SetVisualizationTrajectory(const bool trajvis) { fTrajVis = trajvis; }
		bool VisualizeTrajectory() const { return fTrajVis; }

		// Other simulation settings
		void SetInputFileName(const std::string name) { fInputFileName = name; }
		std::string GetInputFileName() const { return fInputFileName; }

		void SetOutputFileName(const std::string name) { fOutputFileName = name; }
		std::string GetOutputFileName() const { return fOutputFileName; }

		void SetDetectorListFile(const std::string name) { fDetectorList = name; }
		std::string GetDetectorListFile() const { return fDetectorList; }

		void SetRenderName(const std::string name) { fRenderName = name; }
		std::string GetRenderName() const { return fRenderName; }

		void SetPhysicsListName(const std::string name) { fPhysicsListName = name; }
		std::string GetPhysicsListName() const { return fPhysicsListName; }

		void SetVerbosity(const int verb) { fVerbosity = verb; }
		int GetVerbosity() const { return fVerbosity; }

		void SetDetectorPropertiesFile(const std::string file) { fDetectorProperties = file; }
		std::string GetDetectorPropertiesFile() const { return fDetectorProperties; }

		// getters and setters for ground dimensions
		/*
			in Geant4 coordinate system
			Lenght    along x-axis
			Width     " " 	y-axis
			Thickness " " 	z-axis
		*/
		void SetGroundLength(const double l) { fGroundLength = l; }
		double GetGroundLength() { return fGroundLength; }

		void SetGroundWidth(const double w) { fGroundWidth = w; }
		double GetGroundWidth() { return fGroundWidth; }

		void SetGroundThickness(const double t) { fGroundThickness = t; }
		double GetGroundThickness() { return fGroundThickness; }

		// settings for particle injection
		InjectionMode InjectionConversion(std::string name);
		void SetInjectionMode(const InjectionMode injMode) { fInjectionMode = injMode; }
		InjectionMode GetInjectionMode() const { return fInjectionMode; }
		std::string GetInjectionModeName() const { return fInjectionModeName; }

		const std::vector<double>& GetInjectionOrigin() const { return fInjectionOrigin; }
		void SetInjectionOrigin(const std::vector<double> &pos) { fInjectionOrigin = pos; }

		// for circle & halfsphere injection 
		void SetInjectionRadius(const double r) { fInjectionRadius = r; }
		double GetInjectionRadius() { return fInjectionRadius; }

		void SetInjectionHeight(const double h) { fInjectionHeight = h; }
		double GetInjectionHeight() { return fInjectionHeight; }

		void SetInjectionMinTheta(const double minTh) { fInjectionMinTheta = minTh; }
		double GetInjectionMinTheta() { return fInjectionMinTheta; }

		void SetInjectionMaxTheta(const double maxTh) { fInjectionMaxTheta = maxTh; }
		double GetInjectionMaxTheta() { return fInjectionMaxTheta; }

		void SetInjectionMinPhi(const double minPh) { fInjectionMinPhi = minPh; }
		double GetInjectionMinPhi() { return fInjectionMinPhi; }

		void SetInjectionMaxPhi(const double maxPh) { fInjectionMaxPhi = maxPh; }
		double GetInjectionMaxPhi() { return fInjectionMaxPhi; }


		// DetectorSimData getters
		void MakeDetectorSimData(unsigned int id);
		DetectorSimData& GetDetectorSimData() { return fDetectorSimData; }
		const DetectorSimData& GetDetectorSimData() const { return fDetectorSimData; }
		DetectorSimData& GetDetectorSimData(unsigned int id) { return fDetectorSimDataMap[id]; }
		const DetectorSimData& GetDetectorSimData(unsigned int id) const { return fDetectorSimDataMap.at(id); }
		bool HasDetectorSimData(unsigned int id) const { return fDetectorSimDataMap.count(id) != 0; }

	private:

		ParticleVector fParticles;
		ParticleVector fInjectedParticles;
		Particle fCurrentParticle;
		unsigned int fTotalNumberOfParticles = 0;

		DetectorSimData fDetectorSimData;
		std::map<int, DetectorSimData> fDetectorSimDataMap;
		
		InputMode fInputMode;
		std::string fInputModeName;
		SimulationMode fSimulationMode;
		std::string fSimulationModeName;
		InjectionMode  fInjectionMode;
		std::string fInjectionModeName;

		bool fGeoVis = false;
		bool fTrajVis = false;
		std::string fRenderName;
		std::string fPhysicsListName;
		std::string fInputFileName;
		std::string fOutputFileName;
		std::string fDetectorList; 
		std::string fDetectorProperties;
		int fVerbosity = 0;

		double fGroundLength;
		double fGroundWidth;
		double fGroundThickness;

		double fInjectionRadius = 0;
		double fInjectionHeight = 0;

		double fInjectionMinTheta = 0;
		double fInjectionMaxTheta = 90;

		double fInjectionMinPhi = 0;
		double fInjectionMaxPhi = 360;

		std::vector<double> fInjectionOrigin;
};

#endif
