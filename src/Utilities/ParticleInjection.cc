#include "ParticleInjection.h"
#include "Logger.h"

ParticleInjection::ParticleInjection(const std::string aTypeName) :
	fType(eUnknown), fTypeName("Unknown")
{
	Init(aTypeName);
}

void ParticleInjection::Init(const std::string aTypeName)
{   

	InjectionConversion(aTypeName);
	SetDefaultParameters();
}

bool ParticleInjection::IsValid()
{
	if(fType == eHalfSphere || fType == eCircle) {
		if(fInjectionRadius <= 0) {
			Logger::Print("Invalid Radius value for injection type " + fTypeName, ERROR, "ParticleInjection");
		}
		return (fInjectionRadius > 0);
	}
	return (fType != eUnknown);
}

void ParticleInjection::InjectionConversion(std::string aName)
{
	if (aName == "eCircle") {
		fTypeName = "Circle";
		fType = eCircle;
	} else if (aName == "eHalfSphere") {
		fTypeName = "HalfSphere";
		fType = eHalfSphere;
	} else if (aName == "eVertical") {
		fTypeName = "Vertical";
		fType = eVertical;
	} else if (aName == "eFromFile") {
		fTypeName = "FromFile";
		fType = eFromFile;
	} else {
		throw std::invalid_argument("Invalid injection type string. Valid types are: eCircle, eVertical, eHalfSphere and eFromFile. ");
	}

}

void ParticleInjection::SetDefaultParameters()
{
	fInjectionOrigin = {0, 0, 0};
	fInjectionRadius = 0;
	fInjectionMinTheta = 0;
	fInjectionMaxTheta = 90;
	fInjectionMinPhi = 0;
	fInjectionMaxPhi = 360;	
}
