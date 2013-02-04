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

void NotGate::createNegatedTwin()
{	//de lehet hogy ez nem lesz meghívva az újonnan létrejövõ twinekre, akkor viszont ezeket törölni és az outputjait egyesíteni a twin twinjével
	twin = inputs[0];
}

void NotGate::removeNotGate()
{
	if(inputs[0]->getTipus() == INPUT)
	{
		return;
	}
	while(outputs.size()>0)
	{
		Kapu* out = outputs.front();
		outputs.pop_front();
		out->replaceInput(this, this->getTwin()->getTwin());
		//this->getTwin()->getTwin()->addOutput(out); nem kell, ezt a replaceInput elintézi
	}

	//le kell kezelni itt azt az esetet amikor a NotGate nem az output
	//amikor az output, azt majd máshol!!
}