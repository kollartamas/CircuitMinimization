#include "AndGate.h"
#include "KetvaltozosKapu.h"
#include "Kapu.h"

void AndGate::erteketFrissit()
{
	value = (inputs[0]->getValue()) && (inputs[1]->getValue());
}

AndGate::AndGate(Kapu& in1, Kapu& in2):KetvaltozosKapu(in1, in2)
{
	tipus = AND;
}

void AndGate::addToStringInfix(std::string& dest)
{
	dest += '(';
	inputs[0]->addToStringInfix(dest);
	dest += '*';
	inputs[1]->addToStringInfix(dest);
	dest += ')';
}

void AndGate::addToStringPrefix(std::string& dest)
{
	dest += '*';
	dest += ' ';
	inputs[0]->addToStringPrefix(dest);
	inputs[1]->addToStringPrefix(dest);
}

void AndGate::addToStringPostfix(std::string& dest)
{
	inputs[0]->addToStringPostfix(dest);
	inputs[1]->addToStringPostfix(dest);
	dest += '*';
	dest += ' ';
}