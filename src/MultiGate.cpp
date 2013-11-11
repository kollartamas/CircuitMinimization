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
		inputIndex[gate] = --inputs.end();		//TODO: �t�rni hogy iter�tort haszn�ljon (konstans komplexit�s)
	}
}

MultiGate::~MultiGate()
{
	while(!inputs.empty())
	{
		delete inputs.front();	//ez egy�ttal t�rli a list-b�l is
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
				pair<GatePtr,GatePtr> gatePair((*i)->getInput(), (*j)->getInput());	//csak addig h�vjuk, am�g inputs rendezett, ez�rt *i<*j (vagyis a ford�tottjuk nem fordul el� kulcsk�nt)	//TODO:ez nem igaz, jav�tani
				occurrences[gatePair].push_back(static_pointer_cast<MultiGate>(shared_from_this()));	//feljegyezz�k, hogy az adott p�r ebben a Multigate-ben el�fordul
															//ha m�g nem szerepelt a map-ben az adott p�r, az []operator automatikusan inicializ�l egy �res list-et
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

	//occurrences-be feljegyezz�k az �j kapu el�fordul�sait
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

/*void MultiGate::swapInput(GatePtr original, GatePtr newInput)	//occurrenceList friss�t�se nem kell, mert olyan f�zisban h�vjuk csak, amikor az �res
{
	map<GatePtr, list<Link*>::iterator>::iterator iterOriginal(inputIndex.find(original));
	map<GatePtr, list<Link*>::iterator>::iterator iterNew(inputIndex.find(newInput));

	if(iterNew==inputIndex.end())	//ha newInput m�g nem szerepel, hozz�adjuk original hely�re
	{
		(*iterOriginal->second)->setInput(newInput);
		inputIndex.insert(pair<GatePtr,list<Link*>::iterator>(newInput, iterOriginal->second));
	}
	else	//k�l�nben csak t�r�lj�k originalt
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
	if(iter==inputIndex.end())	//ha newInput m�g nem szerepel, hozz�adjuk
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
			//this->deleteRecursively();	//TODO: rekurzi� felesleges, mert a lesz�rmazott mindig megmarad
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
				if(result == original)	// ha !=, akkor az aktu�lis �rt�k t�rl�d�tt a list�b�l
				{
					++iter;		//csak akkor kell l�ptetni, ha nem t�rt�nt t�rl�s
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
	if(incrementMarkedParents() == outputs.size())		//hogy csak olyan kapun v�ltoztassunk, ahonnan minden �t az 'original' szomsz�dj�hoz vezet
	{
		map<GatePtr, list<Link*>::iterator>::iterator iterOriginal(inputIndex.find(original));
		if(iterOriginal != inputIndex.end())
		{
			removeInput(original);	//TODO: egy hat�konyabb overloaddal felhaszn�lni az iteratort
			addInput(newGate);	
		}
#ifdef VS2010
		for each(Link* inputLink in inputs)
#else
		for(Link* inputLink : inputs)
#endif
		{
			if(inputLink->getInput()->getLevel() > original->getLevel())	//csak azok a kapuk tartalmazhatj�k original-t, amelyek szintje nagyobb
			{	//b�r a t�rl�sek miatt a level-ben t�rol �rt�k k�s�bb elt�rhet a val�st�l, a < �s > rel�ci�k nem v�ltoznak
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
			if(incrementMarkedParents() < (*iter)->getInput()->outputs.size())		//hogy csak olyan kapun v�ltoztassunk, ahonnan minden �t az 'original' szomsz�dj�hoz vezet
			{
				++iter;
				continue;
			}

			if((*iter)->getInput() == original)
			{
				this->swapInput(original, newGate);	//TODO: nem j�, az iter�tor invalidd� v�lik ha newGate m�r szerepel
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
