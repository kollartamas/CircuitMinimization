#ifndef TRUEGATE_H
#define TRUEGATE_H

#include "Gate.h"

class TrueGate : public Gate
{
private:

public:
	TrueGate();
	~TrueGate(){}
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