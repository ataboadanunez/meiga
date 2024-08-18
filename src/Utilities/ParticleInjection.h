#ifndef ParticleInjection_h
#define ParticleInjection_h
#include <vector>
#include <string>

class ParticleInjection 
{
    
    public:

        ParticleInjection() {;}
        ParticleInjection(const std::string aTypeName);
        ~ParticleInjection() {;}

        enum InjectionType {
			eUnknown = 0,
			eCircle = 1,
			eHalfSphere = 2,
			eVertical = 3,
			eFromFile
		};


        void Init(const std::string aTypeName);
        bool IsValid();
        void InjectionConversion(std::string aName);
        void SetDefaultParameters();

        // settings for particle injection
		void SetInjectionType(const InjectionType aType) { fType = aType; }
		InjectionType GetInjectionType() const { return fType; }
		std::string GetInjectionTypeName() const { return fTypeName; }

		const std::vector<double>& GetInjectionOrigin() const { return fInjectionOrigin; }
		void SetInjectionOrigin(const std::vector<double> &aOrigin) { fInjectionOrigin = aOrigin; }

		// for circle & halfsphere injection 
		void SetInjectionRadius(const double aR) { fInjectionRadius = aR; }
		double GetInjectionRadius() { return fInjectionRadius; }

		void SetInjectionMinTheta(const double aMinTheta) { fInjectionMinTheta = aMinTheta; }
		double GetInjectionMinTheta() { return fInjectionMinTheta; }

		void SetInjectionMaxTheta(const double aMaxTheta) { fInjectionMaxTheta = aMaxTheta; }
		double GetInjectionMaxTheta() { return fInjectionMaxTheta; }

		void SetInjectionMinPhi(const double aMinPhi) { fInjectionMinPhi = aMinPhi; }
		double GetInjectionMinPhi() { return fInjectionMinPhi; }

		void SetInjectionMaxPhi(const double aMaxPhi) { fInjectionMaxPhi = aMaxPhi; }
		double GetInjectionMaxPhi() { return fInjectionMaxPhi; }

    private:
        InjectionType fType;
        std::string fTypeName;

        double fInjectionRadius;
		double fInjectionMinTheta;
		double fInjectionMaxTheta;
		double fInjectionMinPhi;
		double fInjectionMaxPhi;

		std::vector<double> fInjectionOrigin;

};

#endif