#ifndef NOTGATE_H
#define NOTGATE_H

#include "Gate.h"

class NotGate : public Gate
{
public:
	NotGate(GatePtr in);
	~NotGate();
	/*void deleteRecursively();*/

	bool getValue();

	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	unsigned int getInfixLength();
	unsigned int getPrefixLength();

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
	GatePtr getConstantFreeTwin();

	void removeNotGate();
};

#endif