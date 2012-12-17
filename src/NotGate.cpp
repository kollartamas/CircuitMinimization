#include "NotGate.h"
#include "Kapu.h"

void NotGate::erteketFrissit()
{
	value = !(inputs[0]->getValue()) ;
}

NotGate::NotGate(Kapu& in)
{
	tipus = NOT;

	inputs.push_back(&in);
	in.addOutput(this);
}

void NotGate::addToStringInfix(std::string& dest)
{
	dest += '-';
	inputs[0]->addToStringInfix(dest);
}
	
void NotGate::addToStringPrefix(std::string& dest)
{
	dest += '-';
	dest += ' ';
	inputs[0]->addToStringPrefix(dest);
}
	
void NotGate::addToStringPostfix(std::string& dest)
{
	inputs[0]->addToStringPostfix(dest);
	dest += '-';
	dest += ' ';
}

void NotGate::calculateInfixLength()
{
	stringLength = 2 + inputs[0]->getStringLength();
}

void NotGate::calculatePrefixLength()
{
	stringLength = 1 + inputs[0]->getStringLength();
}