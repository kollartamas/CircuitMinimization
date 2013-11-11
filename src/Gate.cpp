#include "Gate.h"
#include "Link.h"
#include "MultiGate.h"
#include "TrueGate.h"
#include "FalseGate.h"
#include "Logger.h"

#include <list>
#include <iostream>
#include <string>

using namespace std;

#ifdef LOG_MAX_GATENUM
unsigned int Gate::constrCall=0;
unsigned int Gate::destrCall=0;
#endif

Gate::Gate():
	inputs(0),
	outputs(0),
	value(false),
	type(NONE),
	level(0),
	stringLength(0),
	normalForm(),
	twinStrong(),
	twinWeak(),
	flags(0)
	//marked(false),
	//marked2(false)
{
#ifdef LOG_MAX_GATENUM
	Logger::getLogger().setMaxGates(++constrCall-destrCall);
#endif
}

Gate::~Gate()
{
#ifdef LOG_MAX_GATENUM
	++destrCall;
#endif
}

unsigned int Gate::getSize()
{
	if(isFlagged(MARKED))
	{
		return 0;
	}
	else
	{
		unsigned int size = 1;
#ifdef VS2010
		for each(Link* input in inputs)
#else
		for(Link* input : inputs)
#endif
		{
			size += input->getInput()->getSize();
		}
		setFlag(MARKED);
		return size;
	}
}

unsigned int Gate::getSizeAbcStyle()	//NOT és INPUT kapuk nélkül
{
	if(isFlagged(MARKED))
	{
		return 0;
	}
	else
	{
		unsigned int size = 1;
#ifdef VS2010
		for each(Link* input in inputs)
#else
		for(Link* input : inputs)
#endif
		{
			size += input->getInput()->getSizeAbcStyle();
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
#ifdef VS2010
	for each(Link* input in inputs)
#else
	for(Link* input : inputs)
#endif
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

const map<Gate::GatePtr, Link*>& Gate::getMultiInputsWithLink()
{
	return multiInputsWithLink;
}

const set<Gate*>& Gate::getMultiInputsWithSingleOutput()
{
	return multiInputs;
}

const set<Gate*>& Gate::getMultiInputsWithDoubleOutput()
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
#ifdef VS2010
	for each(Link* input in inputs)
#else
	for(Link* input : inputs)
#endif
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

void Gate::removeKeyAndValueFromMap(GatePtr key, Link* value, std::map<GatePtr,Link*>& source)
{
	std::map<GatePtr,Link*>::iterator iter(source.find(key));
	if(iter!=source.end() && iter->second==value)
	{
		source.erase(iter);
	}
}

void Gate::removeNotGatesRecursively()
{
	if(isFlagged(MARKED)) return;

	setFlag(MARKED);
#ifdef VS2010
	for each(Link* link in inputs)
#else
	for(Link* link : inputs)
#endif
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
#ifdef VS2010
	for each(Link* link in inputs)
#else
	for(Link* link : inputs)
#endif
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


/*Gate::GatePtr Gate::getDnf()
{
	return shared_from_this();
}*/

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
#ifdef VS2010
		for each(Link* inp in inputs)
#else
		for(Link* inp : inputs)
#endif
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

void Gate::simplifyRecursively2()
{
}

void Gate::simplifyRecursively3()
{
}

void Gate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
	{
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			if(inputLink->getInput()->getLevel() < original->getLevel()) {continue;}	//TODO: getLevel() <= original->getLevel() a fgv elejére

			if(inputLink->getInput() == original)
			{
				inputLink->setInput(newGate);
			}
			inputLink->getInput()->replaceInputRecursively(original, newGate);
		}
	}
}

Gate::GatePtr Gate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)
{
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
	{
		multiInputsWithLink.erase(original->shared_from_this());	//ha protectedLink az inputok között van, a törlés indokolatlan; így néhány törölhetõ kaput csak következõ iterációban detektálunk TODO kihagyni?
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			if(inputLink->getInput()->getLevel() < original->getLevel()) {continue;}	//TODO: megnézni <= -vel

			if(inputLink->getInput() == original->shared_from_this())
			{
				if(inputLink != protectedLink)
				{
					inputLink->setInput(newGate);
				}
			}
			else
			{
				inputLink->setInput( inputLink->getInput()->replaceInputRecursively2(original, newGate, protectedLink, sourceInputs, separatedGates) );
			}
		}
	}
	return shared_from_this();
}

Gate::GatePtr Gate::replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)
{
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
	{
		//getMultiInputsWithLink();
resetFlag(MULTI_INPUTS_LINK);
	//multiInputsWithLink.erase(original->shared_from_this());	//ha protectedLink az inputok között van, a törlés indokolatlan; így néhány törölhetõ kaput csak következõ iterációban detektálunk TODO kihagyni?
		//sourceInputs.erase(original->shared_from_this());
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			if(inputLink->getInput()->getLevel() < original->getLevel()) {continue;}	//TODO: megnézni <= -vel

			if(inputLink->getInput() == original->shared_from_this())
			{
				if(inputLink != protectedLink1 && inputLink != protectedLink2)
				{
					inputLink->setInput(newGate);
				}
			}
			else
			{
				inputLink->setInput( inputLink->getInput()->replaceInputRecursively3(original, newGate, protectedLink1, protectedLink2, sourceInputs, separatedGates) );
			}
		}
	}
	return shared_from_this();
}
/*Link* Gate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink)
{
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
	{
		multiInputs.erase(original);
		for each(Link* inputLink in inputs)
		{
			if(inputLink->getInput()->getLevel() < original->getLevel()) {continue;}	//TODO: megnézni <= -vel

			if(protectedLink)
			{
				if(inputLink->getInput() == original->shared_from_this())
				{
					if(inputLink != protectedLink)
					{
						inputLink->setInput(newGate);
					}
				}
				else if(inputLink->getInput()->type == this->type)
				{
					inputLink->getInput()->replaceInputRecursively2(original, newGate, protectedLink);
				}
			}
			else
			{
				if(inputLink->getInput() == original->shared_from_this())
				{
					protectedLink = inputLink;
				}
				else if(inputLink->getInput()->type == this->type)
				{
					protectedLink = inputLink->getInput()->replaceInputRecursively2(original, newGate, protectedLink);
				}
				else
				{
					inputLink->getInput()->replaceInputRecursively2(original, newGate, inputLink);
				}
			}		
		}
	}
	return protectedLink;
}*/

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

bool Gate::factorizeRecursively()
{
	return false;
}

unsigned int Gate::printInputsRecursively(unsigned int& nextId)
{
	if(!isFlagged(MARKED))
	{
		setFlag(MARKED);
		string inputsString("");
		for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); ++iter)
		{
			//TODO: stringként küldjük valahova, iostream használata nélkül
			inputsString += " " + to_string(static_cast<long long>((*iter)->getInput()->printInputsRecursively(nextId))); //VC 2010 compatibility
		}
		id = nextId++;
		cout << id <<": " << typeToString(type) << inputsString << endl;
	}
	return id;
}

Gate::gateTypes Gate::getIndirectType()
{
	return this->type;
}

void Gate::removeConstGatesRecursively()	//TODO átnézni helyes-e?
{
	if(!isFlagged(MARKED2))
	{
		setFlag(MARKED2);
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			inputLink->getInput()->removeConstGatesRecursively();
			gateTypes inpType = inputLink->getInput()->getIndirectType();
			if(inpType==TRUE_GATE)
			{
				inputLink->setInput(make_shared<TrueGate>());
			}
			else if(inpType==FALSE_GATE)
			{
				inputLink->setInput(make_shared<FalseGate>());
			}
		}
	}
}

void Gate::removeConstGatesRecursivelyIfFlagged(int flag)
{
	if(isFlagged(flag))
	{
		resetFlag(flag);
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			inputLink->getInput()->removeConstGatesRecursivelyIfFlagged(flag);
			gateTypes inpType = inputLink->getInput()->getIndirectType();
			if(inpType==TRUE_GATE)
			{
				inputLink->setInput(make_shared<TrueGate>());
			}
			else if(inpType==FALSE_GATE)
			{
				inputLink->setInput(make_shared<FalseGate>());
			}
		}
	}
}

std::string Gate::typeToString(int type)
{
	switch(type){
	case INPUT:
		return "input";
	case TRUE_GATE:
		return "true";
	case FALSE_GATE:
		return "false";
	case NOT:
		return "not";
	case AND:
		return "and";
	case OR:
		return "or";
	case MULTI_AND:
		return "multi-and";
	case MULTI_OR:
		return "multi-or";
	default: return "none";
	}
}