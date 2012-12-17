#ifndef KETVALTOZOSKAPU_H
#define KETVALTOZOSKAPU_H

#include "Kapu.h"

class KetvaltozosKapu : public Kapu
{
public:
	KetvaltozosKapu(Kapu& in1, Kapu& in2);

	//virtual void erteketFrissit() =0;
	
	/*virtual void addToStringInfix(std::string& dest) =0;
	virtual void addToStringPrefix(std::string& dest) =0;*/
	
	void calculateInfixLength();
	void calculatePrefixLength();
};

#endif