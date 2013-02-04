#include "OrGate.h"
#include "KetvaltozosKapu.h"
#include "Kapu.h"
#include "AndGate.h"

void OrGate::erteketFrissit()
{
	value = (inputs[0]->getValue()) || (inputs[1]->getValue());
}

OrGate::OrGate(Kapu& in1, Kapu& in2):KetvaltozosKapu(in1, in2)
{
	tipus = OR;
}

void OrGate::addToStringInfix(std::string& dest)
{
	dest += '(';
	inputs[0]->addToStringInfix(dest);
	dest += 'v';
	inputs[1]->addToStringInfix(dest);
	dest += ')';
}

void OrGate::addToStringPrefix(std::string& dest)
{
	dest += 'v';
	dest += ' ';
	inputs[0]->addToStringPrefix(dest);
	inputs[1]->addToStringPrefix(dest);
}

void OrGate::addToStringPostfix(std::string& dest)
{
	inputs[0]->addToStringPostfix(dest);
	inputs[1]->addToStringPostfix(dest);
	dest += 'v';
	dest += ' ';
}

void OrGate::createNegatedTwin()
{
	twin = new AndGate(*inputs[0]->getTwin(), *inputs[1]->getTwin());
	inputs[0]->getTwin()->addOutput(this);
	inputs[1]->getTwin()->addOutput(this);
	twin->twin = this;
}