#include "SiPM.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGauss.h"

SiPM::SiPM(unsigned int id) : fSiPMId(id) { }

void
SiPM::SPEPulse(std::vector<double> &amplitude, double fBinTime, size_t fStartPulse)
{

#warning	move to header. generate random values from mean SPE pulse parameters instead

	const double A1 = 0.3, A2 = 23.8, A3 = 1.59;
	const double tr = 3.9, tf1 = 1.13, tf2 = 22.2, tf3 = 2.51e-1;
	
	const size_t bins = (fPulseDuration/fBinTime);
		
		for (size_t i = 0; i < bins && (fStartPulse + i) < amplitude.size(); ++i) {
			const double t = i*fBinTime;
			amplitude[fStartPulse + i] += A1 * (1.-exp(-t / tr)) * (A2*exp(-t/tf1) + A3*exp(-t/tf2) + exp(-t/tf3));
		}

}

bool 
SiPM::IsPhotonDetected(double energy)
{

	/*
	this function returns if "photon" was detected by the SiPM
	based on its photo-detection efficiency
	*/
	
	double waveLength = 1240. / energy;
	double rand = CLHEP::RandFlat::shoot();
	bool isdetected = false;

	if (waveLength >= 300. && waveLength < 350.)
		rand < 0.07 ? isdetected = true : isdetected = false;

	else if (waveLength >= 350. && waveLength < 400.)
		rand < 0.25 ? isdetected = true : isdetected = false;

	else if (waveLength >= 400. && waveLength < 450.)
		rand < 0.37 ? isdetected = true : isdetected = false;

	else if (waveLength >= 450. && waveLength < 500.)
		rand < 0.40 ? isdetected = true : isdetected = false;

	else if (waveLength >= 500. && waveLength < 550.)
		rand < 0.35 ? isdetected = true : isdetected = false;

	else if (waveLength >= 550. && waveLength < 600.)
		rand < 0.29 ? isdetected = true : isdetected = false;

	else if (waveLength >= 600. && waveLength < 650.)
		rand < 0.23 ? isdetected = true : isdetected = false;

	else if (waveLength >= 650. && waveLength < 700.)
		rand < 0.18 ? isdetected = true : isdetected = false;

	else if (waveLength >= 700. && waveLength < 750.)
		rand < 0.13 ? isdetected = true : isdetected = false;

	else if (waveLength >= 750. && waveLength < 800.)
		rand < 0.10 ? isdetected = true : isdetected = false;

	else if (waveLength >= 800. && waveLength < 850.)
		rand < 0.07 ? isdetected = true : isdetected = false;

	else if (waveLength >= 850. && waveLength < 900.)
		rand < 0.04 ? isdetected = true : isdetected = false;

	else
		isdetected = false;

	return isdetected;
}
