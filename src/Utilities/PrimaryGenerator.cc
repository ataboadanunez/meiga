#include "PrimaryGenerator.h"
#include "CorsikaUtilities.h"
#include "Geometry.h"
#include "Particle.h"

#include "Randomize.hh"
#include <CLHEP/Random/Randomize.h>

using CLHEP::RandFlat;
using namespace std;


PrimaryGenerator::PrimaryGenerator(Event& theEvent) :
	
	fEvent(theEvent)
{
	
	SimData &simData = fEvent.GetSimData();
	injMode = simData.GetInjectionMode();
	fUseEcoMug = (simData.GetInputMode() == simData.InputMode::eUseEcoMug);
	
	// initialize EcoMug generator in case is needed: only muons or HalfSphere generation
	if (fUseEcoMug || (injMode == simData.InjectionMode::eHalfSphere)) {
		// print info
		if (fUseEcoMug )
			cout << "[INFO] PrimaryGenerator::PrimaryGenerator: Primary generation using EcoMug. Only muons will be generated!" << endl;
		if ((injMode == simData.InjectionMode::eHalfSphere))
			cout << "[INFO] PrimaryGenerator::PrimaryGenerator: Primary generation in HalfSphere mode. Using EcoMug for position generation." << endl;

		fMuonGen.SetUseHSphere();
		double injectionRadius = simData.GetInjectionRadius();
		vector<double> injectionOrigin = simData.GetInjectionOrigin();
		double oX = injectionOrigin[0];
		double oY = injectionOrigin[1];
		double oZ = injectionOrigin[2];

		fMuonGen.SetHSphereRadius(injectionRadius);
		fMuonGen.SetHSphereCenterPosition({{oX, oY, oZ}});
		
		// limits for momentum and position
		fMuonGen.SetMinimumMomentum(1e-1);
    fMuonGen.SetMaximumMomentum(1e4);
    double minPhi = simData.GetInjectionMinPhi();
    double maxPhi = simData.GetInjectionMaxPhi();
    double minTheta = simData.GetInjectionMinTheta();
    double maxTheta = simData.GetInjectionMaxTheta();

    fMuonGen.SetHSphereMinPositionTheta(minTheta * (M_PI / 180));
    fMuonGen.SetHSphereMaxPositionTheta(maxTheta * (M_PI / 180));
    fMuonGen.SetHSphereMinPositionPhi(minPhi * (M_PI / 180));
    fMuonGen.SetHSphereMaxPositionPhi(maxPhi * (M_PI / 180));


	}


}


PrimaryGenerator::~PrimaryGenerator()
{

}



void
PrimaryGenerator::GeneratePrimaryParticle()
{

	// cout << "[INFO] PrimaryGenerator::PrimaryGenerator: Primary generated using EcoMug" << endl;
	// set particle Id = current particle in the event / muon
	// set particle position 
	// set particle direction as muon momentum

	SimData &simData = fEvent.GetSimData();
	Particle &currentParticle = simData.GetCurrentParticle();
	cout << "[DEBUG] PrimaryGenerator::GeneratePrimaryParticle: Particle ID before generation = " << currentParticle.GetParticleId() << endl;
	cout << "[DEBUG] PrimaryGenerator::GeneratePrimaryParticle: Particle position (X0) before generation = " << currentParticle.GetPosition().at(0) / CLHEP::cm << endl; 
	cout << "[DEBUG] PrimaryGenerator::GeneratePrimaryParticle: Particle momentum (pX) before generation = " << currentParticle.GetMomentumDirection().at(0) / CLHEP::GeV << endl; 
	// coordinates of particle position
	double x0;
	double y0;
	double z0;
	
	// components of particle momentum
	double px;
	double py;
	double pz;
	double pTot;
	
	// particle zenith and azimuth angle
	double pTheta;
	double pPhi;

	// generate only muons using EcoMug library
	if (fUseEcoMug) {
		
		/*
			In case EcoMug is selected as input mode, only muons
			will be generated using the EcoMug library: 
			https://doi.org/10.1016/j.nima.2021.165732

			This library generates position and momentum of
			muons according to parameterized functions.

			Muon momentum is drawn from the distribution of the measured
			muon spectrum and position is generated in a HalfSphere
			with radius and origin of coordinates given by the user 
			in the DetectorList.xml file. 

			Particle id (muon charge), position and momentum are
			set to the "currentParticle" in corresponding units.

		*/

		fMuonGen.Generate();
		
		array<double, 3> muonPosition = fMuonGen.GetGenerationPosition();
		x0 = muonPosition[0] * CLHEP::mm;
		y0 = muonPosition[1] * CLHEP::mm;
		z0 = muonPosition[2] * CLHEP::mm;

		fParticlePosition = {x0, y0, z0};
		pTot = fMuonGen.GetGenerationMomentum() * CLHEP::GeV;
		pTheta = fMuonGen.GetGenerationTheta();
		pPhi = fMuonGen.GetGenerationPhi();

		px = pTot * sin(pTheta) * cos(pPhi);
		py = pTot * sin(pTheta) * sin(pPhi);
		pz = pTot * cos(pTheta);

		fParticleDirection = {px, py, pz};

		if (fMuonGen.GetCharge() < 0)
			fParticleId = 6; // muon (mu-)
		else if (fMuonGen.GetCharge() > 0)
			fParticleId = 5; // anti-muon (mu+)
		
		currentParticle.SetZenith(pTheta);
		currentParticle.SetAzimuth(pPhi);
		currentParticle.SetPosition(fParticlePosition);
		currentParticle.SetMomentumDirection(fParticleDirection);
		currentParticle.SetParticleId(fParticleId);
		currentParticle.SetMomentum(pTot);

		cout << "[DEBUG] PrimaryGenerator::GeneratePrimaryParticle: Particle ID AFTER generation = " << currentParticle.GetParticleId() << endl;
		cout << "[DEBUG] PrimaryGenerator::GeneratePrimaryParticle: Particle position (X0) AFTER generation = " << currentParticle.GetPosition().at(0) / CLHEP::cm << endl; 
		cout << "[DEBUG] PrimaryGenerator::GeneratePrimaryParticle: Particle momentum (pX) AFTER generation = " << currentParticle.GetMomentumDirection().at(0) / CLHEP::GeV << endl;
		return;
	}

	// if not EcoMug, use ARTI (ASCII) file as input
	else {
		/*
			In this case, particle ID and momentum are read from 
			the input (ASCII) file. The input file has the
			ARTI shower files format (for more info read 
			https://doi.org/10.1140/epjc/s10052-022-10883-z
			), and the injection position is generated by this function.

			In this sense, "currentParticle" ID and momentum were already 
			set in the IO/ParticleFiller code, and the injection position
			will be set here according to the selected injection mode.

		*/

		// switch case (injectionMode)

		px = currentParticle.GetMomentumDirection().at(0);
		py = currentParticle.GetMomentumDirection().at(1);
		pz = currentParticle.GetMomentumDirection().at(2);
		pTot = currentParticle.GetMomentum();
		
		pTheta = acos(pz / pTot);
		pPhi = atan2(-py, -px);

		currentParticle.SetZenith(pTheta);
		currentParticle.SetAzimuth(pPhi);

		switch(injMode) {
			
			case SimData::InjectionMode::eCircle:
			{

				double injRadius = simData.GetInjectionRadius();
				double injHeight = simData.GetInjectionHeight();

				double rand = RandFlat::shoot(0., 1.);
				double r = injRadius * sqrt(rand);
				double minPhi = simData.GetInjectionMinPhi() * (CLHEP::pi / 180);
				double maxPhi = simData.GetInjectionMaxPhi() * (CLHEP::pi / 180);
				double phi = RandFlat::shoot(minPhi, maxPhi);

				x0 = r * cos(phi);
				y0 = r * sin(phi);
				z0 = injHeight;
				
			}	
			break;

			case SimData::InjectionMode::eHalfSphere:
			{

				// use EcoMug generator method for HalfSphere generation
				fMuonGen.Generate();
				array<double, 3> particlePos = fMuonGen.GetGenerationPosition();
				x0 = particlePos[0] * CLHEP::mm;
				y0 = particlePos[1] * CLHEP::mm;
				z0 = particlePos[2] * CLHEP::mm;

			}
			break;

			case SimData::InjectionMode::eUnknown:
			{

				x0 = 0;
				y0 = 0;
				z0 = 0;

			}
			break;

		}

		fParticlePosition = {x0, y0, z0};
		currentParticle.SetPosition(fParticlePosition);

		return;
	}

}