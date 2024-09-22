// implementation of class Geometry
#include "Geometry.h"

Geometry::Geometry()
{

}

G4ThreeVector
Geometry::ToG4Vector(const std::vector<double>* v, const double unitConversion)
{
 	return G4ThreeVector(
		unitConversion * v->at(0),
		unitConversion * v->at(1),
		unitConversion * v->at(2)
	);
}

G4ThreeVector
Geometry::ToG4Vector(const std::vector<double>& v, const double unitConversion)
{
 	return G4ThreeVector(
		unitConversion * v.at(0),
		unitConversion * v.at(1),
		unitConversion * v.at(2)
	);
}

std::vector<double>
Geometry::ToStdVector(const G4ThreeVector &v) {
	
	std::vector<double> res(3);
	res[0] = v.x();
	res[1] = v.y();
	res[2] = v.z();
	
	return res;
}