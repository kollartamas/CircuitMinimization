#ifndef INPUT_H
#define INPUT_H

#include "Kapu.h"
#include <string>

class Input: public Kapu
{
private:
	unsigned int id;
public:
	Input(unsigned int id=0);
	void setValue(bool val);
	void erteketFrissit();

	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);
	void calculateInfixLength();
	void calculatePrefixLength();
};

#endif