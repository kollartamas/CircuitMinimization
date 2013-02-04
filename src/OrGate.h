#ifndef ORGATE_H
#define ORGATE_H

#include "Kapu.h"
#include "KetvaltozosKapu.h"

class OrGate : public KetvaltozosKapu
{
public:
	OrGate(Kapu& in1, Kapu& in2);
	
	void erteketFrissit();
	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	void createNegatedTwin();
};

#endif