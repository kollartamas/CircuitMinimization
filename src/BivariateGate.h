#ifndef BIVARIATE_GATE_H
#define BIVARIATE_GATE_H

#include "Gate.h"
#include <set>

class BivariateGate : public Gate
{
public:
	//typedef std::shared_ptr<BivariateGate> BivariateGatePtr;
	BivariateGate(GatePtr in1, GatePtr in2);
	~BivariateGate();
	/*void deleteRecursively();*/
	
	unsigned int getInfixLength();
	unsigned int getPrefixLength();
	const std::set<Gate*>& getMultiInputs(); 

	void addMultiGateInputs(std::set<GatePtr>& inputSet);
};

#endif