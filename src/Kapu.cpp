#include "Kapu.h"

using namespace std;

Kapu::Kapu():
	elozoekCount(0)
{}

void Kapu::addOutput(Kapu* out)
{
	outputs.push_back(out);
}

bool Kapu::getValue()
{
	return value;
}

int Kapu::getTipus()
{
	return tipus;
}

unsigned int Kapu::getStringLength()
{
	return stringLength;
}

const list<Kapu*> Kapu::getOutputs()
{
	return outputs;
}

bool Kapu::incrementElozoekCount(bool iranyElore)
{
	unsigned int target;
	if(iranyElore)
	{
		target = inputs.size();
	}
	else
	{
		target = outputs.size();
	}

	if( ++elozoekCount >= target )
	{
		elozoekCount = 0;
		return true;
	}
	else
	{
		return false;
	}
}