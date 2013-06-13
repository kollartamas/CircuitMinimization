#ifndef ORGATE_H
#define ORGATE_H

#include "Gate.h"
#include "BivariateGate.h"

#include <list>

class OrGate : public BivariateGate
{
public:
	OrGate(GatePtr in1, GatePtr in2);
	~OrGate(){}
	
	bool getValue();
	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
	GatePtr getMultiTwin();
	GatePtr getSingleTwin();
	GatePtr getConstantFreeTwin();

	//std::list<std::list<Gate*>>& getDnf();
	GatePtr getDnf();
};

#endif