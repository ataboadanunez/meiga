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

		enum SimulationMode {
			eFull = 0,
			eFast = 1
		};

		enum InjectionMode {
			eUnknown = 0,
			eRandom = 1,
			ePositionFromFile = 2,
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

		// Simulation mode
		SimulationMode ModeConversion(const std::string name);
		void SetSimulationMode(const SimulationMode simMode) { fSimulationMode = simMode; }
		SimulationMode GetSimulationMode() const { return fSimulationMode; }
		std::string GetSimulationModeName() const { return fSimulationModeName; }

		// Particle injection mode
		InjectionMode InjectionConversion(const std::string name);
		void SetInjectionMode(const InjectionMode injMode) { fInjectionMode = injMode; }
		InjectionMode GetInjectionMode() const { return fInjectionMode; }
		std::string GetInjectionModeName() const { return fInjectionModeName; }
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

		void SetDetectorList(const std::string name) { fDetectorList = name; }
		std::string GetDetectorList() const { return fDetectorList; }

		void SetRenderName(const std::string name) { fRenderName = name; }
		std::string GetRenderName() const { return fRenderName; }

		void SetPhysicsListName(const std::string name) { fPhysicsListName = name; }
		std::string GetPhysicsListName() const { return fPhysicsListName; }

		void SetVerbosity(const int verb) { fVerbosity = verb; }
		int GetVerbosity() const { return fVerbosity; }

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

		// DetectorSimData getters
		void MakeDetectorSimData(unsigned int id);
		DetectorSimData& GetDetectorSimData() { return fDetectorSimData; }
		DetectorSimData& GetDetectorSimData(unsigned int id) { return fDetectorSimDataMap[id]; }
		

	private:

		ParticleVector fParticles;
		Particle fCurrentParticle;
		unsigned int fTotalNumberOfParticles = 0;

		DetectorSimData fDetectorSimData;
		std::map<int, DetectorSimData> fDetectorSimDataMap;
		
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
		int fVerbosity = 0;

		double fGroundLength;
		double fGroundWidth;
		double fGroundThickness;
};

#endif
