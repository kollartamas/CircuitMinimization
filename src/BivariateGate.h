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
	const std::map<GatePtr,Link*>& getMultiInputsWithLink();

	void addMultiGateInputs(std::set<GatePtr>& inputSet);
	const std::set<Gate*>& getMultiInputsWithSingleOutput();
	const std::set<Gate*>& getMultiInputsWithDoubleOutput();
	
	GatePtr removeFactor(Gate* factor);

protected:
	std::set<Gate*> multiInputsWithSingleOutput;	//TODO: kipróbálni list-ként
	std::set<Gate*> multiInputsWithDoubleOutput;	//TODO: kipróbálni list-ként
};

#endif