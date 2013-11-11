#include "MultiGate.h"
#include "Link.h"


using namespace std;

MultiGate::MultiGate(set<GatePtr>& inputSet)
{
#ifdef VS2010
	for each(GatePtr gate in inputSet)
#else
	for(GatePtr gate : inputSet)
#endif
	{
		new Link(gate, this);
		inputIndex[gate] = --inputs.end();		//TODO: átírni hogy iterátort használjon (konstans komplexitás)
	}
}

MultiGate::~MultiGate()
{
	while(!inputs.empty())
	{
		delete inputs.front();	//ez egyúttal törli a list-bõl is
	}
}

unsigned int MultiGate::getSize()
{
	if(isFlagged(MARKED))
	{
		return 0;
	}
	else
	{
		unsigned int size = -1;
#ifdef VS2010
		for each(Link* input in inputs)
#else
		for(Link* input : inputs)
#endif
		{
			(++size) += input->getInput()->getSize();
		}
		setFlag(MARKED);
		return size;
	}
}

void MultiGate::addOccurrencesRecursive(occurrenceList& occurrences, int type)
{
	if(!isFlagged(MARKED))
	{
		for(list<Link*>::iterator i(inputs.begin()); i!=inputs.end(); ++i)
		{
			list<Link*>::iterator j(i);
			++j;
			for(; j!=inputs.end(); ++j)
			{
				if(this->type==type)
				{
				pair<GatePtr,GatePtr> gatePair((*i)->getInput(), (*j)->getInput());	//csak addig hívjuk, amíg inputs rendezett, ezért *i<*j (vagyis a fordítottjuk nem fordul elõ kulcsként)	//TODO:ez nem igaz, javítani
				occurrences[gatePair].push_back(static_pointer_cast<MultiGate>(shared_from_this()));	//feljegyezzük, hogy az adott pár ebben a Multigate-ben elõfordul
															//ha még nem szerepelt a map-ben az adott pár, az []operator automatikusan inicializál egy üres list-et
				}
			}
			((*i)->getInput())->addOccurrencesRecursive(occurrences, type);
		}
		setFlag(MARKED);
	}
}

bool MultiGate::hasInput(GatePtr input)
{
	return inputIndex.find(input) != inputIndex.end();
}


void MultiGate::replaceInputs(GatePtr input1, GatePtr input2, GatePtr newInput, occurrenceList& occurrences, list<MultiGate::occurrenceIterator>& newElements)
{
	map<GatePtr, list<Link*>::iterator>::iterator oldPosition(inputIndex.find(input1));
	delete *oldPosition->second;
	inputIndex.erase(oldPosition);
	oldPosition = inputIndex.find(input2);
	delete *oldPosition->second;
	inputIndex.erase(oldPosition);

	//occurrences-be feljegyezzük az új kapu elõfordulásait
	for(map<GatePtr, list<Link*>::iterator>::iterator i=inputIndex.begin(); i!=inputIndex.end(); ++i)
	{
		GatePtr gate1, gate2;
		if(newInput<i->first)
		{
			gate1 = newInput;
			gate2 = i->first;
		}
		else
		{
			gate1 = i->first;
			gate2 = newInput;
		}
		pair<GatePtr,GatePtr> key(gate1, gate2);
		occurrenceIterator iter(occurrences.find(key));
		if(iter==occurrences.end())
		{
			iter = occurrences.insert(pair<pair<GatePtr,GatePtr>,list<MultiGatePtr>>(key, list<MultiGatePtr>())).first;
			newElements.push_back(iter);
		}
		//occurrences[pair<Gate*,Gate*>(gate1, gate2)].push_back(this);
		iter->second.push_back(static_pointer_cast<MultiGate>(shared_from_this()));
	}
	
	new Link(newInput, this);
	inputIndex.insert(pair<GatePtr,list<Link*>::iterator>(newInput, --inputs.end()));
}

/*void MultiGate::swapInput(GatePtr original, GatePtr newInput)	//occurrenceList frissítése nem kell, mert olyan fázisban hívjuk csak, amikor az üres
{
	map<GatePtr, list<Link*>::iterator>::iterator iterOriginal(inputIndex.find(original));
	map<GatePtr, list<Link*>::iterator>::iterator iterNew(inputIndex.find(newInput));

	if(iterNew==inputIndex.end())	//ha newInput még nem szerepel, hozzáadjuk original helyére
	{
		(*iterOriginal->second)->setInput(newInput);
		inputIndex.insert(pair<GatePtr,list<Link*>::iterator>(newInput, iterOriginal->second));
	}
	else	//különben csak töröljük originalt
	{
		delete *iterOriginal->second;
	}
	inputIndex.erase(iterOriginal);

	/*if(original->outputs.empty())
	{
		original->deleteRecursively();
	}
}*/


void MultiGate::addInput(GatePtr newInput)
{
	map<GatePtr, list<Link*>::iterator>::iterator iter(inputIndex.find(newInput));
	if(iter==inputIndex.end())	//ha newInput még nem szerepel, hozzáadjuk
	{
		new Link (newInput, this);
		inputIndex.insert(pair<GatePtr,list<Link*>::iterator>(newInput, --inputs.end()));
	}
}

void MultiGate::removeInput(GatePtr oldInput)
{
	map<GatePtr, list<Link*>::iterator>::iterator iter(inputIndex.find(oldInput));
	if(iter!=inputIndex.end())
	{
		delete *iter->second;
		inputIndex.erase(iter);
	}
}

/*Gate::GatePtr MultiGate::replaceWithSingleRecursively()
{
	if(!isFlagged(MARKED))
	{
		if(inputIndex.size()==1)
		{
			GatePtr onlyInput = inputs.front()->getInput();
			while(!outputs.empty())
			{
				if(outputs.front()->getOutput()->type==MULTI_AND || outputs.front()->getOutput()->type==MULTI_OR)
				{
					map<GatePtr, list<Link*>::iterator>::iterator iter(static_pointer_cast<MultiGate>(outputs.front()->getOutput())->inputIndex.find(shared_from_this()));
					static_pointer_cast<MultiGate>(outputs.front()->getOutput())->swapInput(shared_from_this(), onlyInput);
				}
				else
				{
					outputs.front()->setInput(onlyInput);
				}
			}
			onlyInput->replaceWithSingleRecursively();
			//this->deleteRecursively();	//TODO: rekurzió felesleges, mert a leszármazott mindig megmarad
			return onlyInput;
		}
		else
		{
			for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end();)
			{
				list<Link*>::iterator nextIter(iter);
				++nextIter;
				(*iter)->getInput()->replaceWithSingleRecursively();
				iter = nextIter;
				/*Gate* original = &(*iter)->getInput();
				Gate* result = (*iter)->getInput().replaceWithSingleRecursively();
				if(result == original)	// ha !=, akkor az aktuális érték törlõdött a listából
				{
					++iter;		//csak akkor kell léptetni, ha nem történt törlés
				}
			}
			setFlag(MARKED);
		}
	}
	return shared_from_this();
}*/

void MultiGate::removeMultiGatesRecursively()
{
	if(isFlagged(MARKED)) return;

	setFlag(MARKED);
	for (list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); )
	{
		if(((*iter)->getInput()->type == Gate::MULTI_AND || (*iter)->getInput()->type == Gate::MULTI_OR) && (*iter)->getInput()->inputs.size()==1)
		{
			//GatePtr multiGate = (*iter)->getInput();
			//(*iter)->getInput()->inputs.front()->getInput()->removeMultiGatesRecursively();
			this->addInput((*iter)->getInput()->inputs.front()->getInput());
			this->removeInput((*iter++)->getInput());
		}
		else
		{
			(*iter++)->getInput()->removeMultiGatesRecursively();
		}
	}
}

/*void MultiGate::removeMultiGatesRecursively()
{
	if(isFlagged(MARKED)) return;

	setFlag(MARKED);
	for each(Link* link in inputs)
	{
		if((link->getInput()->type == Gate::MULTI_AND || link->getInput()->type == Gate::MULTI_OR) && link->getInput()->inputs.size()==1)
		{
			GatePtr multiGate = link->getInput();
			this->swapInput(multiGate, multiGate->inputs.front()->getInput());
		}
		link->getInput()->removeMultiGatesRecursively();
	}
}*/

void MultiGate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
	{
		map<GatePtr, list<Link*>::iterator>::iterator iterOriginal(inputIndex.find(original));
		if(iterOriginal != inputIndex.end())
		{
			removeInput(original);	//TODO: egy hatékonyabb overloaddal felhasználni az iteratort
			addInput(newGate);	
		}
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			if(inputLink->getInput()->getLevel() > original->getLevel())	//csak azok a kapuk tartalmazhatják original-t, amelyek szintje nagyobb
			{	//bár a törlések miatt a level-ben tárol érték késõbb eltérhet a valóstól, a < és > relációk nem változnak
				inputLink->getInput()->replaceInputRecursively(original, newGate);
			}
		}
	}
}

/*void MultiGate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{
	if(!isFlagged(MARKED2))
	{
		for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); ) //each(Link* inputLink in inputs)
		{
			//TODO: level compare
			if(incrementMarkedParents() < (*iter)->getInput()->outputs.size())		//hogy csak olyan kapun változtassunk, ahonnan minden út az 'original' szomszédjához vezet
			{
				++iter;
				continue;
			}

			if((*iter)->getInput() == original)
			{
				this->swapInput(original, newGate);	//TODO: nem jó, az iterátor invaliddá válik ha newGate már szerepel
			}
			inputLink->getInput()->replaceInputRecursively(original, newGate);
		}
		setFlag(MARKED2);
	}
}*/

//ideiglenes, hogy ne abstract class legyen
void MultiGate::addToStringInfix(std::string& dest){throw runtime_error("multi to string");}
void MultiGate::addToStringPrefix(std::string& dest){}
void MultiGate::addToStringPostfix(std::string& dest){}
unsigned int MultiGate::getInfixLength(){return 0;}
unsigned int MultiGate::getPrefixLength(){return 0;}
const std::set<Implicant>& MultiGate::getDnf(unsigned int numOfVariables){throw runtime_error("multi to dnf");}
Gate::GatePtr MultiGate::getNegatedTwin(){return NULL;}
Gate::GatePtr MultiGate::getCopyTwin(){return NULL;}
abc::Abc_Obj_t* MultiGate::getAbcNode(abc::Abc_Ntk_t* network){return NULL;}
//const std::set<Gate*>& MultiGate::getMultiInputs(){return multiInputs;} 
