#include "PrimaryGenerator.h"
#include "ParticleInjection.h"
#include "CorsikaUtilities.h"
#include "Geometry.h"
#include "Particle.h"
#include "SimData.h"
#include "Logger.h"

#include "Randomize.hh"
#include <CLHEP/Random/Randomize.h>

using CLHEP::RandFlat;
using namespace std;


PrimaryGenerator::PrimaryGenerator(Event& theEvent) :
    
    fEvent(theEvent)
{
    
    SimData &simData = fEvent.GetSimData();
    fUseEcoMug = (simData.GetInputMode() == SimData::InputMode::eUseEcoMug);
    Event::Config &cfg = fEvent.GetConfig();
    fSaveInput = cfg.fSaveInput;
    ostringstream msg;
    msg << "SaveInput = " << (fSaveInput ? "yes" : "no");
    Logger::Print(msg, INFO, "PrimaryGenerator");
    // initialize EcoMug generator in case is needed: only muons or HalfSphere generation
    if (fUseEcoMug) {
        // print info
        ostringstream msg;
        msg << "Primary generation using EcoMug. Only muons will be generated!";
        Logger::Print(msg, WARNING, "PrimaryGenerator");
        
        fMuonGen.SetUseHSphere();
        ParticleInjection &injection = simData.GetParticleInjection();
        if (!injection.IsValid()) {
            Logger::Print("Invalid particle injection.", ERROR, "PrimaryGenerator");
            throw runtime_error("Invalid particle injection.");
        }
        double injectionRadius = injection.GetInjectionRadius();
        vector<double> injectionOrigin = injection.GetInjectionOrigin();
        double oX = injectionOrigin[0];
        double oY = injectionOrigin[1];
        double oZ = injectionOrigin[2];

        fMuonGen.SetHSphereRadius(injectionRadius);
        fMuonGen.SetHSphereCenterPosition({{oX, oY, oZ}});
        
        // limits for momentum and position
        fMuonGen.SetMinimumMomentum(1e-1);
        fMuonGen.SetMaximumMomentum(1e4);
        double minPhi = injection.GetInjectionMinPhi();
        double maxPhi = injection.GetInjectionMaxPhi();
        double minTheta = injection.GetInjectionMinTheta();
        double maxTheta = injection.GetInjectionMaxTheta();

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
    ParticleInjection &injection = simData.GetParticleInjection();
    if (!injection.IsValid()) {
        Logger::Print("Invalid particle injection.", ERROR, "GeneratePrimaryParticle");
            throw runtime_error("Invalid particle injection.");
    }
    auto injectionMode = injection.GetInjectionType();
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
        if (injectionMode != ParticleInjection::eVertical) {
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
        if (injectionMode != ParticleInjection::eVertical) {
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
    Particle &currentParticle = simData.GetCurrentParticle();
    ParticleInjection &injection = simData.GetParticleInjection();
    if (!injection.IsValid()) {
        Logger::Print("Invalid particle injection.", ERROR, "ComputeInjectionPosition");
            throw runtime_error("Invalid particle injection.");
    }
    auto injectionType = injection.GetInjectionType();
    vector<double> injectionOrigin = injection.GetInjectionOrigin();
    bool isDefaultOrigin = true;
    double x0 = injectionOrigin[0];
    double y0 = injectionOrigin[1];
    double z0 = injectionOrigin[2];
    if (x0 != 0.0 || y0 != 0.0 || z0 != 0.0)
        isDefaultOrigin = false;

    double xInj;
    double yInj;
    double zInj;

    switch(injectionType) 
    {
        case ParticleInjection::eCircle:
        {

            double injRadius = injection.GetInjectionRadius();

            double rand = RandFlat::shoot(0., 1.);
            double r = injRadius * sqrt(rand);
            double minPhi = injection.GetInjectionMinPhi() * (CLHEP::pi / 180);
            double maxPhi = injection.GetInjectionMaxPhi() * (CLHEP::pi / 180);
            double phi = RandFlat::shoot(minPhi, maxPhi);

            xInj = r * cos(phi) + x0;
            yInj = r * sin(phi) + y0;
            zInj = z0;
            break;
        }
        case ParticleInjection::eHalfSphere:
        {
            double injRadius = injection.GetInjectionRadius();
            
            double minTheta = injection.GetInjectionMinTheta() * (CLHEP::pi / 180);
            double maxTheta = injection.GetInjectionMaxTheta() * (CLHEP::pi / 180);
            double maxCosTheta = cos(maxTheta);
            double minCosTheta = cos(minTheta);

            double minPhi = injection.GetInjectionMinPhi() * (CLHEP::pi / 180);
            double maxPhi = injection.GetInjectionMaxPhi() * (CLHEP::pi / 180);

            double randTheta = acos(RandFlat::shoot(maxCosTheta, minCosTheta));
            double randPhi = RandFlat::shoot(minPhi, maxPhi);

            xInj = injRadius * sin(randTheta) * cos(randPhi) + x0;
            yInj = injRadius * sin(randTheta) * sin(randPhi) + y0;
            zInj = injRadius * cos(randTheta) + z0;
            break;
        }
        case ParticleInjection::eVertical:
        {   
            double injRadius = injection.GetInjectionRadius();
            zInj = z0;
            if (injRadius) {
                ostringstream infomsg;
                infomsg << "Injecting particle vertically over a circle of radius: " << injRadius / CLHEP::m << " m at height: " << zInj / CLHEP::m;
                Logger::Print(infomsg, INFO, "ComputeInjectionPosition");

                double rand = RandFlat::shoot(0., 1.);
                double r = injRadius * sqrt(rand);
                double minPhi = injection.GetInjectionMinPhi() * (CLHEP::pi / 180);
                double maxPhi = injection.GetInjectionMaxPhi() * (CLHEP::pi / 180);
                double phi = RandFlat::shoot(minPhi, maxPhi);
                xInj = r * cos(phi) + x0;
                yInj = r * sin(phi) + y0;

            } else {
                xInj = x0;
                yInj = y0;
            }
            break;
        }
        case ParticleInjection::eFromFile:
            if (isDefaultOrigin) {
                ostringstream infomsg;
                infomsg << "[WARNING] PrimaryGenerator::ComputeInjectionPosition: in injection mode 'eFromFile', found coordinates for the origin position: (" 
                        << x0 / CLHEP::m << ", " << y0 / CLHEP::m << ", " << z0 / CLHEP::m 
                        << ") m. These will be ignored." << endl;
                Logger::Print(infomsg, WARNING, "ComputeInjectionPosition");
            }

            if (fUseEcoMug) {
                Logger::Print("using injection mode 'eFromFile' with EcoMug particle generator might not produce the desired results.", WARNING, "ComputeInjectionPosition");
            }
            xInj = currentParticle.GetPosition().at(0);
            yInj = currentParticle.GetPosition().at(1);
            zInj = currentParticle.GetPosition().at(2);
            break;
        case ParticleInjection::eUnknown:
        default:
            xInj = currentParticle.GetPosition().at(0);
            yInj = currentParticle.GetPosition().at(1);
            zInj = currentParticle.GetPosition().at(2);
            break;
    }

    particlePosition = {xInj, yInj, zInj};
    Logger::PrintVector(particlePosition, "Injecting particle at position: ", INFO, "ComputeInjectionPosition");
}