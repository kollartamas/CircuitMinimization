#include "NotGate.h"
#include "Gate.h"
#include "Link.h"
#include "TrueGate.h"
#include "FalseGate.h"
#include "Input.h"

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

unsigned int NotGate::getSizeAbcStyle()
{
	setFlag(MARKED);
	return inputs.front()->getInput()->getSizeAbcStyle();
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

const set<Implicant>& NotGate::getDnf(unsigned int numOfVariables)
	//csak a közvetlenül inputhoz kapcsolódó NotGate esetén használható
{
	if(!isFlagged(NORMAL_FORM))
	{
		setFlag(NORMAL_FORM);
		vector<int> values(numOfVariables, Implicant::DONT_CARE);
		values[static_pointer_cast<Input>(inputs.front()->getInput())->id] = Implicant::FALSE;
		normalForm.clear();
		normalForm.insert(Implicant(values));
	}
	return normalForm;
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

		if(newInput->type == FALSE_GATE)
		{
			setTwinStrong(make_shared<TrueGate>());
		}
		else if(newInput->type == TRUE_GATE)
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

Gate::GatePtr NotGate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)
{
	//TODO: simplifyRecursively2()-ben rekurziót elõrébb hozni és ezt törölni
	if(outputs.size()==1)	//enélkül törlõdnének olyan elemek, amik szerepelnek multiInputsWithLink-ben
	{
		setTwinWeak(shared_from_this());
		return Gate::replaceInputRecursively2(original, newGate, protectedLink, sourceInputs, separatedGates);
	}

	if(!isFlagged(TWIN))
	{
		setTwinWeak(shared_from_this());
		if(inputs.front()->getInput()==original->shared_from_this() && inputs.front()!=protectedLink)
		{
			if(newGate->type==TRUE_GATE)
			{
				separatedGates.push_back(shared_from_this());
				setTwinStrong(make_shared<FalseGate>());
			}
			else if(newGate->type == FALSE_GATE)
			{
				separatedGates.push_back(shared_from_this());
				setTwinStrong(make_shared<TrueGate>());
			}
		}
	}

	return getTwinSafe()==shared_from_this() ? Gate::replaceInputRecursively2(original, newGate, protectedLink, sourceInputs, separatedGates) : getTwinSafe();
}

Gate::GatePtr NotGate::replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)
{
	//TODO: simplifyRecursively2()-ben rekurziót elõrébb hozni és ezt törölni
	if(outputs.size()==1)	//enélkül törlõdnének olyan elemek, amik szerepelnek multiInputsWithLink-ben
	{
		setTwinWeak(shared_from_this());
		return Gate::replaceInputRecursively3(original, newGate, protectedLink1, protectedLink2, sourceInputs, separatedGates);
	}

	if(!isFlagged(TWIN))
	{
resetFlag(MULTI_INPUTS_LINK);
		setTwinWeak(shared_from_this());
		if(inputs.front()->getInput()==original->shared_from_this() && inputs.front()!=protectedLink1 && inputs.front()!=protectedLink2)
		{
			if(newGate->type==TRUE_GATE)
			{
				separatedGates.push_back(shared_from_this());
				setTwinStrong(make_shared<FalseGate>());
			}
			else if(newGate->type == FALSE_GATE)
			{
				separatedGates.push_back(shared_from_this());
				setTwinStrong(make_shared<TrueGate>());
			}
		}
	}

	return getTwinSafe()==shared_from_this() ? Gate::replaceInputRecursively3(original, newGate, protectedLink1, protectedLink2, sourceInputs, separatedGates) : getTwinSafe();
}

abc::Abc_Obj_t* NotGate::getAbcNode(abc::Abc_Ntk_t* pNetwork)
{
	if(!isFlagged(ABC_NODE))
	{
		setFlag(ABC_NODE);
		pAbcNode = abc::Abc_ObjNot( inputs.front()->getInput()->getAbcNode(pNetwork) );
	}
	return pAbcNode;
}

Gate::gateTypes NotGate::getIndirectType()
{
	gateTypes inpType = inputs.front()->getInput()->type;
	if(inpType==TRUE_GATE)
	{
		return FALSE_GATE;
	}
	else if(inpType==FALSE_GATE)
	{
		return TRUE_GATE;
	}
	return this->type;
}

bool NotGate::factorizeRecursively()
{
	bool changed(false);
	if( !isFlagged(FACTORIZED) )
	{
		changed = inputs.front()->getInput()->factorizeRecursively() || changed;
		setFlag(FACTORIZED);
	}
	return changed;
}