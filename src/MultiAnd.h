#ifndef MULTIAND_H
#define MULTIAND_H

#include "MultiGate.h"
#include "Gate.h"
#include <set>

class MultiAnd : public MultiGate
{
public:
	MultiAnd();
	MultiAnd(std::set<GatePtr>& inputSet);
	~MultiAnd(){}

	bool getValue();
	
	GatePtr getSingleTwin();
	GatePtr getConstantFreeTwin();

	void simplifyRecursively();
};

#endif