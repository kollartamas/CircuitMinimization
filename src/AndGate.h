#ifndef ANDGATE_H
#define ANDGATE_H

#include "Kapu.h"
#include "KetvaltozosKapu.h"

class AndGate : public KetvaltozosKapu
{
public:
	AndGate(Kapu& in1, Kapu& in2);

	void erteketFrissit();
	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	void createNegatedTwin();
};

#endif