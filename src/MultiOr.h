#ifndef MULTIOR_H
#define MULTIOR_H

#include "MultiGate.h"
#include "Gate.h"
#include <set>

class MultiOr : public MultiGate
{
public:
	MultiOr();
	MultiOr(std::set<GatePtr>& inputSet);
	~MultiOr(){}

	bool getValue();
	
	GatePtr getSingleTwin();
	GatePtr getConstantFreeTwin();

	void simplifyRecursively();
};

#endif