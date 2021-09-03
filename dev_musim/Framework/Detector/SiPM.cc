#include "SiPM.h"
#include "CLHEP/Random/RandFlat.h"

SiPM::SiPM(unsigned int id) : fSiPMId(id) { }


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