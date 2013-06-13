#ifndef FALSEGATE_H
#define FALSEGATE_H

#include "Gate.h"

class FalseGate : public Gate
{
private:

public:
	FalseGate();
	~FalseGate(){}
	/*void deleteRecursively();*/

	bool getValue();

	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);
	unsigned int getInfixLength();
	unsigned int getPrefixLength();

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
};

#endif