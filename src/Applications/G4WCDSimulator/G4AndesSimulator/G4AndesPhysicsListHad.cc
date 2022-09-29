// $Id$
#include "G4AndesPhysicsListHad.h"

#include <G4ParticleTypes.hh>
#include <G4Decay.hh>
#include <G4LeptonConstructor.hh>
#include <G4MesonConstructor.hh>
#include <G4BaryonConstructor.hh>
#include <G4ProcessManager.hh>
#include <G4PhotoElectricEffect.hh>
#include <G4ComptonScattering.hh>
#include <G4GammaConversion.hh>
#include <G4eMultipleScattering.hh>
#include <G4eIonisation.hh>
#include <G4eBremsstrahlung.hh>
#include <G4eplusAnnihilation.hh>
#include <G4MuMultipleScattering.hh>
#include <G4MuIonisation.hh>
#include <G4MuBremsstrahlung.hh>
#include <G4MuPairProduction.hh>
#include <G4MuonMinusCapture.hh>
#include <G4hMultipleScattering.hh>
#include <G4hIonisation.hh>
#include <G4OpAbsorption.hh>
#include <G4OpRayleigh.hh>
#include <G4OpBoundaryProcess.hh>
#include <G4OpWLS.hh>
#include <G4Scintillation.hh>
#include <G4Cerenkov.hh>

#include <iostream>

using namespace std;


G4AndesPhysicsListHad::G4AndesPhysicsListHad() : G4VUserPhysicsList()
{ 
	cout << "...G4AndesPhysicsListHad..." << endl;
}


void
G4AndesPhysicsListHad::ConstructParticle()
{
	// pseudo-particle: useful for testing
	G4Geantino::GeantinoDefinition();
	G4ChargedGeantino::ChargedGeantinoDefinition();

	G4Gamma::GammaDefinition();

	G4OpticalPhoton::OpticalPhotonDefinition();

	G4LeptonConstructor::ConstructParticle();

	// hadrons
	G4MesonConstructor::ConstructParticle();
	G4BaryonConstructor::ConstructParticle();
}


void
G4AndesPhysicsListHad::ConstructProcess()
{
	G4VUserPhysicsList::AddTransportation();

	// muon decay
	{
		const auto decayProcess = new G4Decay;

		auto& mumManager = *G4MuonMinus::MuonMinusDefinition()->GetProcessManager();
		mumManager.AddDiscreteProcess(decayProcess);
		mumManager.SetProcessOrdering(decayProcess, idxPostStep);
		mumManager.SetProcessOrdering(decayProcess, idxAtRest);

		auto& mupManager = *G4MuonPlus::MuonPlusDefinition()->GetProcessManager();
		mupManager.AddDiscreteProcess(decayProcess);
		mupManager.SetProcessOrdering(decayProcess, idxPostStep);
		mupManager.SetProcessOrdering(decayProcess, idxAtRest);
	}

	// EM processes

	auto& it = *G4ParticleTable::GetParticleTable()->GetIterator();
	for (it.reset(); it(); ) {
		const G4ParticleDefinition& particle = *it.value();
		auto& pManager = *particle.GetProcessManager();

		if (particle == *G4Gamma::Gamma()) {

			pManager.AddDiscreteProcess(new G4PhotoElectricEffect);
			pManager.AddDiscreteProcess(new G4ComptonScattering);
			pManager.AddDiscreteProcess(new G4GammaConversion);

		} else if (particle == *G4Electron::Electron()) {

			pManager.AddProcess(new G4eMultipleScattering, -1, 1, 1);
			pManager.AddProcess(new G4eIonisation,         -1, 2, 2);
			pManager.AddProcess(new G4eBremsstrahlung,     -1, 3, 3);

		} else if (particle == *G4Positron::Positron()) {

			pManager.AddProcess(new G4eMultipleScattering, -1,  1, 1);
			pManager.AddProcess(new G4eIonisation,         -1,  2, 2);
			pManager.AddProcess(new G4eBremsstrahlung,     -1,  3, 3);
			pManager.AddProcess(new G4eplusAnnihilation,    0, -1, 4);

		} else if (particle == *G4MuonPlus::MuonPlus() ||
							 particle == *G4MuonMinus::MuonMinus()) {

			pManager.AddProcess(new G4MuMultipleScattering, -1, 1, 1);
			pManager.AddProcess(new G4MuIonisation,         -1, 2, 2);
			pManager.AddProcess(new G4MuBremsstrahlung,     -1, 3, 3);
			pManager.AddProcess(new G4MuPairProduction,     -1, 4, 4);
			if (particle == *G4MuonMinus::MuonMinus())
				pManager.AddProcess(new G4MuonMinusCapture);

		} else if (particle.GetPDGCharge() &&
							 particle != *G4ChargedGeantino::ChargedGeantino()) {

			// all others charged particles except geantino
			pManager.AddProcess(new G4hMultipleScattering, -1, 1, 1);
			pManager.AddProcess(new G4hIonisation,         -1, 2, 2);

		}
	}

	// optical processes
	{
		const G4int maxNumPhotons = 3;

		const auto absorption = new G4OpAbsorption;

		const auto rayleighScattering = new G4OpRayleigh;

		const auto boundary = new G4OpBoundaryProcess;
		boundary->SetVerboseLevel(0);

		const auto wlsProcess = new G4OpWLS;
		wlsProcess->UseTimeProfile("exponential");

		// Scintillation process
		const auto scintProcess = new G4Scintillation("Scintillation");
		scintProcess->SetScintillationYieldFactor(1.);
		scintProcess->SetTrackSecondariesFirst(true);
		scintProcess->SetVerboseLevel(0);

		// Use Birks Correction in the Scintillation process
		//const auto emSaturation = G4LossTableManager::Instance()->EmSaturation();
		//scintProcess->AddSaturation(emSaturation);

		auto& it = *G4ParticleTable::GetParticleTable()->GetIterator();

		// Add Cerenkov and Scintillator processes

	}
}


void
G4AndesPhysicsListHad::SetCuts()
{
	
	const double defaultCutValue = 1.*CLHEP::mm;
	const double cutForGamma = defaultCutValue;
	const double cutForElectron = defaultCutValue;
	const double cutForPositron = defaultCutValue;
	const double cutForMuonMinus = defaultCutValue;
	const double cutForMuonPlus = defaultCutValue;
	const double cutForOpticalPhotons = defaultCutValue;

	// set cut values for gamma at first and for e- second and next for e+,
	// because some processes for e+/e- need cut values for gamma

	this->SetParticleCuts(cutForGamma, G4Gamma::Gamma());

	this->SetParticleCuts(cutForElectron, G4Electron::Electron());

	
	this->SetParticleCuts(cutForPositron, G4Positron::Positron());

	this->SetParticleCuts(cutForMuonPlus, G4MuonPlus::MuonPlus());

	this->SetParticleCuts(cutForMuonMinus, G4MuonMinus::MuonMinus());

	this->SetParticleCuts(cutForOpticalPhotons, G4OpticalPhoton::OpticalPhoton());

	this->SetVerboseLevel(0);
	this->DumpCutValuesTable(0);
}


