#include "NotGate.h"
#include "Gate.h"
#include "Link.h"
#include "TrueGate.h"
#include "FalseGate.h"

using namespace std;

bool NotGate::getValue()
{
	if(!isFlagged(MARKED))
	{
		value = !(inputs.front()->getInput()->getValue());
		setFlag(MARKED);
	}
	return value;
}

NotGate::NotGate(GatePtr in)
{
	type = NOT;
	new Link(in, this);
}

NotGate::~NotGate()
{
	delete inputs.front();
}

/*void NotGate::deleteRecursively()
{
	GatePtr inp = inputs.front()->getInput();
	delete this;
	if(inp->outputs.size()==0)
	{
		inp->deleteRecursively();
	}
}*/

void NotGate::addToStringInfix(std::string& dest)
{
	dest += '-';
	inputs.front()->getInput()->addToStringInfix(dest);
}
	
void NotGate::addToStringPrefix(std::string& dest)
{
	dest += '-';
	dest += ' ';
	inputs.front()->getInput()->addToStringPrefix(dest);
}
	
void NotGate::addToStringPostfix(std::string& dest)
{
	inputs.front()->getInput()->addToStringPostfix(dest);
	dest += '-';
	dest += ' ';
}

unsigned int NotGate::getInfixLength()
{
	if(!isFlagged(MARKED))
	{
		stringLength = 2 + inputs.front()->getInput()->getInfixLength();
		setFlag(MARKED);
	}
	return stringLength;
}

unsigned int NotGate::getPrefixLength()
{
	if(!isFlagged(MARKED))
	{
		stringLength = 1 + inputs.front()->getInput()->getPrefixLength();
		setFlag(MARKED);
	}
	return stringLength;
}

Gate::GatePtr NotGate::getNegatedTwin()
{
	if(!isFlagged(TWIN))
	{
		setTwinStrong(inputs.front()->getInput());
	}

	return getTwinSafe();
}

Gate::GatePtr NotGate::getCopyTwin()
{
	if(!isFlagged(TWIN))
	{
		setTwinStrong(make_shared<NotGate>(inputs.front()->getInput()->getCopyTwin()));
	}
	return getTwinStrong();
}

Gate::GatePtr NotGate::getConstantFreeTwin()
{
	if(!isFlagged(TWIN))
	{
		GatePtr newInput(inputs.front()->getInput()->getConstantFreeTwin());

		if(newInput->type == FALSE)
		{
			setTwinStrong(make_shared<TrueGate>());
		}
		else if(newInput->type == TRUE)
		{
			setTwinStrong(make_shared<FalseGate>());
		}
		else if(newInput == inputs.front()->getInput())	//ha az eredeti input konstans-mentes, önmagával tér vissza
		{
			setTwinWeak(shared_from_this());
		}
		else
		{
			GatePtr newGate(make_shared<NotGate>(newInput));
			setTwinStrong(newGate);
			newGate->setTwinWeak(newGate);	//beállítjuk saját twin-jének, mert tudjuk hogy az inputjai és õ is konstans-mentes
		}
	}
	return getTwinSafe();
}

/*void NotGate::removeNotGate()
{
	if(inputs.front()->getTipus() == INPUT)
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
}*/