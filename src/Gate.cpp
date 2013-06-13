#include "Gate.h"
#include "Link.h"
#include "MultiGate.h"

#include <list>

using namespace std;

unsigned int Gate::constrCall=0;
unsigned int Gate::destrCall=0;

Gate::Gate():
	inputs(0),
	outputs(0),
	value(false),
	type(NONE),
	level(0),
	stringLength(0),
	twinStrong(),
	twinWeak(),
	flags(0)
	//marked(false),
	//marked2(false)
{constrCall++;}

unsigned int Gate::getSize()
{
	if(isFlagged(MARKED))
	{
		return 0;
	}
	else
	{
		unsigned int size = 1;
		for each(Link* input in inputs)
		{
			size += input->getInput()->getSize();
		}
		setFlag(MARKED);
		return size;
	}
}

bool Gate::isFlagged(unsigned int flag)
{
	return (this->flags&flag) != 0;
}

void Gate::setFlag(unsigned int flag)
{
	this->flags |= flag;
}

void Gate::resetFlag(unsigned int flag)
{
	if(flag & TWIN)
	{
		twinStrong.reset();	//TODO: megnézni hogy külön függvény hatékonyabb-e
		twinWeak.reset();
	}
	if(flag & NORMAL_FORM)
	{
		normalForm.clear();	//TODO: megnézni hogy külön függvény hatékonyabb-e
	}
	this->flags &= (~flag);
}

void Gate::resetFlagRecursive(unsigned int flag)
{
	resetFlag(flag);
	for each(Link* input in inputs)
	{
		if(input->getInput()->isFlagged(flag))
		{
			input->getInput()->resetFlagRecursive(flag);
		}
	}
}

const set<Gate*>& Gate::getMultiInputs()
{
	return multiInputs;
}

Gate::GatePtr Gate::getTwinStrong()
{
	return twinStrong;
}

void Gate::setTwinStrong(GatePtr twin)
{
	twinStrong = twin;
	setFlag(TWIN);
}

void Gate::setTwinWeak(GatePtr twin)
{
	twinStrong.reset();
	twinWeak = twin;
	setFlag(TWIN);
}

Gate::GatePtr Gate::getTwinSafe()
{
	return twinStrong ? twinStrong : twinWeak.lock();
}

void Gate::setTwinSafe(GatePtr twin)
{
	shared_ptr<Gate> thisPtr = shared_from_this();
	if(twin == thisPtr || twin->getTwinStrong() == thisPtr)	//ha körbehivatkozás alakulna ki
	{
		twinStrong.reset();
		twinWeak = twin;
	}
	else
	{
		twinStrong = twin;
	}
	setFlag(TWIN);
}
	
	
/*void Gate::resetFlagRecursive(Gate::MARKED)
{
	this->marked = false;
	for each(Link* input in inputs)
	{
		if(input->getInput().marked)
		{
			input->getInput().resetFlagRecursive(Gate::MARKED);
		}
	}
}*/

/*void Gate::resetMarking2Recursively()
{
	this->marked2 = false;
	for each(Link* input in inputs)
	{
		if(input->getInput().marked2)
		{
			input->getInput().resetMarking2Recursively();
		}
	}
}*/

void Gate::setMarkingRecursively()
{
	this->setFlag(MARKED);
	for each(Link* input in inputs)
	{
		if(!input->getInput()->isFlagged(MARKED))
		{
			input->getInput()->setMarkingRecursively();
		}
	}
}

/*void Gate::resetMarkingAndTwinRecursively()
{
	this->marked = false;
	this->twin = NULL;
	for each(Link* input in inputs)
	{
		if(input->getInput().marked)
		{
			input->getInput().resetMarkingAndTwinRecursively();
		}
	}
}*/

Gate::GatePtr Gate::getMultiTwin()
{
	return shared_from_this();
}


Gate::GatePtr Gate::getSingleTwin()
{
	return shared_from_this();
}

Gate::GatePtr Gate::getConstantFreeTwin()
{
	return shared_from_this();
}

void Gate::removeNotGatesRecursively()
{
	if(isFlagged(MARKED)) return;

	setFlag(MARKED);
	for each(Link* link in inputs)
	{
		while(link->getInput()->type == Gate::NOT)
		{
			GatePtr notGate = link->getInput();
			if(notGate->inputs.front()->getInput()->getNegatedTwin() == notGate/*->type == Gate::INPUT*/)	//ha ez az INPUT twin-je  
			{
				break;//continue;
			}

			link->setInput(notGate->getNegatedTwin()->getNegatedTwin());
			/*if(notGate->outputs.empty())
			{
				orphanedGates.push_back(notGate);	//elõfordulhat hogy késõbb újra kap outputot, de az már végleg meg is marad
													//így minden kapu legfeljebb egyszer szerepel a listában (de ellenõrizni kell hogy tényleg törlendõ-e)
			}*/
		}
		link->getInput()->removeNotGatesRecursively(/*orphanedGates*/);
	}
}

void Gate::removeMultiGatesRecursively()
{
	if(isFlagged(MARKED)) return;

	setFlag(MARKED);
	for each(Link* link in inputs)
	{
		if((link->getInput()->type == Gate::MULTI_AND || link->getInput()->type == Gate::MULTI_OR) && link->getInput()->inputs.size()==1)
		{
			GatePtr multiGate = link->getInput();
			link->setInput(multiGate->inputs.front()->getInput());
		}
		link->getInput()->removeMultiGatesRecursively();
	}
}

void Gate::addOccurrencesRecursive(MultiGate::occurrenceList& occurrences, int type)
{
	if(!isFlagged(MARKED))
	{
		for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); ++iter)
		{
			(*iter)->getInput()->addOccurrencesRecursive(occurrences, type);
		}
		setFlag(MARKED);
	}
}

/*Gate::GatePtr Gate::replaceWithSingleRecursively()
{
	if(!isFlagged(MARKED))
	{
		for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); ++iter)
		{
			(*iter)->getInput()->replaceWithSingleRecursively();
		}
		setFlag(MARKED);
	}
	return shared_from_this();
}*/


Gate::GatePtr Gate::getDnf()
{
	return shared_from_this();
}

/*Gate::ClauseSet& Gate::getDnf2()
{
	if(!isFlagged(NORMAL_FORM))
	{
		/*struct clauseComp {
			bool operator() (const GatePtr left, const GatePtr right) const
			{return *left<right;}
		};*//*
		Clause newClause;
		newClause.insert(shared_from_this());
		normalForm.insert(newClause);
		setFlag(NORMAL_FORM);
	}
	return normalForm;
}*/

unsigned int Gate::getLevel()
{
	if(!isFlagged(LEVEL))
	{
		level = 0;
		for each(Link* inp in inputs)
		{
			unsigned int current = inp->getInput()->getLevel();
			if(current >= level)
			{
				level = current + 1;
			}
		}
		setFlag(LEVEL);
	}
	return level;
}

unsigned int Gate::getMarkedParents()
{
	if(!isFlagged(MARKED_PARENTS))	//ennek az az értelme, hogy resetflags nullázhassa markedParents-t
	{
		markedParents = 0;
		setFlag(MARKED_PARENTS);
	}
	return markedParents;
}

unsigned int Gate::incrementMarkedParents()
{
	if(!isFlagged(MARKED_PARENTS))	//ennek az az értelme, hogy resetflags nullázhassa markedParents-t
	{
		markedParents = 0;
		setFlag(MARKED_PARENTS);
	}
	return ++markedParents;
}

void Gate::simplifyRecursively()
{
}

void Gate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
	{
		for each(Link* inputLink in inputs)
		{
			if(inputLink->getInput()->getLevel() < original->getLevel()) {continue;}

			if(inputLink->getInput() == original)
			{
				inputLink->setInput(newGate);
			}
			inputLink->getInput()->replaceInputRecursively(original, newGate);
		}
	}
}

/*void Gate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{
	if(!isFlagged(MARKED2))
	{
		for each(Link* inputLink in inputs)
		{
			//TODO: level compare
			if(incrementMarkedParents() < inputLink->getInput()->outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
			{
				continue;
			}

			if(inputLink->getInput() == original)
			{
				inputLink->setInput(newGate);
				/*if(original->outputs.empty())
				{
					original->deleteRecursively();
				}
			}
			inputLink->getInput()->replaceInputRecursively(original, newGate);
		}
		setFlag(MARKED2);
	}
}*/