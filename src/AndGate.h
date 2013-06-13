#ifndef ANDGATE_H
#define ANDGATE_H

#include "Gate.h"
#include "BivariateGate.h"

#include <list>
#include <map>

class AndGate : public BivariateGate
{
public:
	AndGate(GatePtr in1, GatePtr in2);
	~AndGate(){}

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
	static GatePtr conjunctionOfDnfs(GatePtr dnf1, GatePtr dnf2);
	static GatePtr conjunctionOfClauseAndDnf(GatePtr dnf1, GatePtr dnf2);

};

#endif