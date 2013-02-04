#include "Kapu.h"

using namespace std;

Kapu::Kapu():
	elozoekCount(0),
	twin(NULL),
	marked(false)
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

/*void Kapu::removeNotGates()
{
	/*if(output is notgate)
	{
	}
	else{
	for (unsigned int i=0; i<inputs.size(); i++)
	{
		Kapu* input = inputs[i];
		if (input->megjelolve)
		{
			continue;
		}

		if(input->tipus != NOT)
		{
			input->removeNotGates();
		}
		else
		{
			inputs[i] = input->inputs[0]->getNegatedTwin();
			inputs[i]->addOutput(this);
		}
	}
	megjelolve = true;
	}
}

Kapu* Kapu::getNegatedTwin()
{
	if(twin == NULL)
	{
		createNegatedTwin();
	}
	return twin;
}*/

Kapu* Kapu::getTwin()
{
	return twin;
}

void Kapu::replaceInput(Kapu* original, Kapu* other)
{
	bool changed = false;
	for(unsigned int i=0; i<inputs.size(); i++)
	{
		if(inputs[i]==original)
		{
			inputs[i] = other;
			other->addOutput(this);
			changed = true;
		}
	}
	if(!changed)
	{
		throw exception("Hiba az input kapu cseréjekor");
	}
}

void Kapu::removeIfNotMarked()
{
	if(!marked)
	{
		delete this;
	}
	else
	{
		for(list<Kapu*>::iterator iter=outputs.begin(); iter!=outputs.end();)
		{
			if(!((*iter)->marked)){iter=outputs.erase(iter);}
			else ++iter;
		}
	}
}