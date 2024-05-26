#include "PrimaryGenerator.h"
#include "CorsikaUtilities.h"
#include "Geometry.h"
#include "Particle.h"
#include "SimData.h"

#include "Randomize.hh"
#include <CLHEP/Random/Randomize.h>

using CLHEP::RandFlat;
using namespace std;


PrimaryGenerator::PrimaryGenerator(Event& theEvent) :
    
    fEvent(theEvent)
{
    
    SimData &simData = fEvent.GetSimData();
    injMode = simData.GetInjectionMode();
    fUseEcoMug = (simData.GetInputMode() == SimData::InputMode::eUseEcoMug);
    Event::Config &cfg = fEvent.GetConfig();
    fSaveInput = cfg.fSaveInput;
    cout << "[INFO] PrimaryGenerator::PrimaryGenerator: SaveInput = " << (fSaveInput ? "yes" : "no") << endl;
    // initialize EcoMug generator in case is needed: only muons or HalfSphere generation
    if (fUseEcoMug) {
        // print info
        cout << "[INFO] PrimaryGenerator::PrimaryGenerator: Primary generation using EcoMug. Only muons will be generated!" << endl;
        
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
    SimData &simData = fEvent.GetSimData();
    Particle &currentParticle = simData.GetCurrentParticle();
    auto injectionMode = simData.GetInjectionMode();
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

        long int seed = simData.GetSeed();
        fMuonGen.SetSeed(seed);
        fMuonGen.Generate();
        ComputeInjectionPosition(simData, fParticlePosition);
        pTot = fMuonGen.GetGenerationMomentum() * CLHEP::GeV;
        pTheta = fMuonGen.GetGenerationTheta();
        pPhi = fMuonGen.GetGenerationPhi();
        if (injectionMode != SimData::InjectionMode::eVertical) {
            px = pTot * sin(pTheta) * cos(pPhi);
            py = pTot * sin(pTheta) * sin(pPhi);
            pz = pTot * cos(pTheta);
        } else {
            px = 0;
            py = 0;
            pz = -pTot;
        }
        fParticleDirection = {px, py, pz};
        if (fMuonGen.GetCharge() < 0) {
            fParticleId = Particle::eMuon; // muon (mu-)
            fParticleName = "mu-";
        }
        else if (fMuonGen.GetCharge() > 0) {
            fParticleId = Particle::eAntiMuon; // anti-muon (mu+)
            fParticleName = "mu+";
        }
        currentParticle.SetZenith(pTheta);
        currentParticle.SetAzimuth(pPhi);
        currentParticle.SetInjectionPosition(fParticlePosition);
        currentParticle.SetMomentumDirection(fParticleDirection);
        currentParticle.SetParticleId(fParticleId);
        currentParticle.SetName(fParticleName);
        currentParticle.SetMomentum(pTot);

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

        pTot = currentParticle.GetMomentum();
        if (injectionMode != SimData::InjectionMode::eVertical) {
            px = currentParticle.GetMomentumDirection().at(0);
            py = currentParticle.GetMomentumDirection().at(1);
            pz = currentParticle.GetMomentumDirection().at(2);
        } else {
            px = 0;
            py = 0;
            pz = -pTot;
        }
        fParticleDirection = {px, py, pz};
        pTheta = acos(pz / pTot);
        pPhi = atan2(-py, -px);

        currentParticle.SetZenith(pTheta);
        currentParticle.SetAzimuth(pPhi);

        ComputeInjectionPosition(simData, fParticlePosition);
        currentParticle.SetInjectionPosition(fParticlePosition);
        currentParticle.SetMomentumDirection(fParticleDirection);
    }

    if (fSaveInput) {
        // save particle information
        simData.InsertInjectedParticle(currentParticle);
    }


    return;
}

void 
PrimaryGenerator::ComputeInjectionPosition(SimData &simData, std::vector<double> &particlePosition)
{

    auto mode = simData.GetInjectionMode();
    Particle &currentParticle = simData.GetCurrentParticle();
    vector<double> injectionOrigin = simData.GetInjectionOrigin();
    double x0 = injectionOrigin[0];
    double y0 = injectionOrigin[1];
    double z0 = injectionOrigin[2];

    double xInj;
    double yInj;
    double zInj;

    switch(mode) {

    case SimData::InjectionMode::eCircle:
    {

        double injRadius = simData.GetInjectionRadius();
        double injHeight = simData.GetInjectionHeight();
        if (injHeight != z0){
            cout << "[WARNING] PrimaryGenerator::ComputeInjectionPosition: in injectionMode = eCircle, `injHeight` does not match with `z-coordinate` of the circle. Using `injHeight` as default." << endl;
        }


        double rand = RandFlat::shoot(0., 1.);
        double r = injRadius * sqrt(rand);
        double minPhi = simData.GetInjectionMinPhi() * (CLHEP::pi / 180);
        double maxPhi = simData.GetInjectionMaxPhi() * (CLHEP::pi / 180);
        double phi = RandFlat::shoot(minPhi, maxPhi);

        xInj = r * cos(phi) + x0;
        yInj = r * sin(phi) + y0;
        zInj = injHeight;

        break;
    }

    case SimData::InjectionMode::eHalfSphere:
    {
        double injRadius = simData.GetInjectionRadius();
        
        double minTheta = simData.GetInjectionMinTheta() * (CLHEP::pi / 180);
        double maxTheta = simData.GetInjectionMaxTheta() * (CLHEP::pi / 180);
        double maxCosTheta = cos(maxTheta);
        double minCosTheta = cos(minTheta);

        double minPhi = simData.GetInjectionMinPhi() * (CLHEP::pi / 180);
        double maxPhi = simData.GetInjectionMaxPhi() * (CLHEP::pi / 180);

        double randTheta = acos(RandFlat::shoot(maxCosTheta, minCosTheta));
        double randPhi = RandFlat::shoot(minPhi, maxPhi);

        xInj = injRadius * sin(randTheta) * cos(randPhi) + x0;
        yInj = injRadius * sin(randTheta) * sin(randPhi) + y0;
        zInj = injRadius * cos(randTheta) + z0;
        break;
    }
    case SimData::InjectionMode::eVertical:
    {   
        double injRadius = simData.GetInjectionRadius();
        double injHeight = simData.GetInjectionHeight();
        if (injHeight != z0){
            cout << "[WARNING] PrimaryGenerator::ComputeInjectionPosition: in injectionMode = eVertical, `injHeight` does not match with `z-coordinate`. Using `std::max(injHeight, z0)`." << endl;
            zInj = std::max(injHeight, z0);
        } else {
            zInj = z0;
        }
        if (injRadius) {
            cout << "[INFO] PrimaryGenerator::ComputeInjectionPosition: Injecting particle vertically over a circle of radius: " << injRadius / CLHEP::m << " m at height: " << zInj / CLHEP::m << endl;
            double rand = RandFlat::shoot(0., 1.);
            double r = injRadius * sqrt(rand);
            double minPhi = simData.GetInjectionMinPhi() * (CLHEP::pi / 180);
            double maxPhi = simData.GetInjectionMaxPhi() * (CLHEP::pi / 180);
            double phi = RandFlat::shoot(minPhi, maxPhi);
            xInj = r * cos(phi) + x0;
            yInj = r * sin(phi) + y0;

        } else {
            xInj = x0;
            yInj = y0;
        }
        break;
    }
    case SimData::InjectionMode::eFromFile:
        xInj = currentParticle.GetPosition().at(0);
        yInj = currentParticle.GetPosition().at(1);
        zInj = currentParticle.GetPosition().at(2);
        break;
    case SimData::InjectionMode::eUnknown:
    default:
        xInj = currentParticle.GetPosition().at(0);
        yInj = currentParticle.GetPosition().at(1);
        zInj = currentParticle.GetPosition().at(2);
        break;
    

        }

    particlePosition = {xInj, yInj, zInj};
    cout << "[INFO] PrimaryGenerator::ComputeInjectionPosition: Injecting particle at position: x = " << xInj / CLHEP::m << ", y = " << yInj / CLHEP::m << " z = " << zInj / CLHEP::m << " m " << endl;

}