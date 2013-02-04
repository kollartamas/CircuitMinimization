#ifndef NOTGATE_H
#define NOTGATE_H

#include "Kapu.h"

class NotGate : public Kapu
{
public:
	NotGate(Kapu& in);

	void erteketFrissit();

	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	void calculateInfixLength();
	void calculatePrefixLength();

	void createNegatedTwin();
	void removeNotGate();
};

#endif