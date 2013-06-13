#include "Circuit.h"
#include "TrueGate.h"
#include "FalseGate.h"
#include "NotGate.h"
#include "AndGate.h"
#include "OrGate.h"
#include "Link.h"
#include "MultiGate.h"

#include <set>

using namespace std;

Circuit::Circuit(unsigned int numOfInputs):
	inputs(numOfInputs)
{
	for(unsigned int i=0; i<numOfInputs; i++)
	{
		inputs[i] = make_shared<Input>(i);
	}
}

Circuit::Circuit(unsigned int numOfInputs, unsigned int numOfGates, bool includeConstants):
	inputs(numOfInputs)
{
	vector<Gate::GatePtr> elements;
	elements.reserve(2 + numOfInputs + numOfGates);
	set<Gate::GatePtr> deadEnds;
	
	if(includeConstants)
	{
		elements.push_back(make_shared<TrueGate>());
		elements.push_back(make_shared<FalseGate>());
	}

	for(unsigned int i=0; i<numOfInputs; i++)
	{
		Input::InputPtr newInput = make_shared<Input>(i);
		inputs[i] = newInput;
		elements.push_back(newInput);
		deadEnds.insert(newInput);
	}

	//generalunk veletlenszeru kapukat, amik inputkent a korabbi elemeket kapjak
	//elso szakaszban szabadon valaszthatunk:
	for(unsigned int i=0; numOfGates >= i+deadEnds.size(); i++)
	{
		Gate::GatePtr newGate, inp1, inp2;
		int gateType = rand()%3;

		switch(gateType)
		{
		case 0:
			inp1 = elements[rand()%elements.size()];
			newGate = make_shared<NotGate>(inp1);
			deadEnds.erase(inp1);
			deadEnds.insert(newGate);
			elements.push_back(newGate);
			break;
		case 1:
			inp1 = elements[rand()%elements.size()];
			inp2 = elements[rand()%elements.size()];
			newGate = make_shared<AndGate>(inp1, inp2);
			deadEnds.erase(inp1);
			deadEnds.erase(inp2);
			deadEnds.insert(newGate);
			elements.push_back(newGate);
			break;
		case 2:
			inp1 = elements[rand()%elements.size()];
			inp2 = elements[rand()%elements.size()];
			newGate = make_shared<OrGate>(inp1, inp2);
			deadEnds.erase(inp1);
			deadEnds.erase(inp2);
			deadEnds.insert(newGate);
			elements.push_back(newGate);
			break;
		}
	}
	//masodik szakaszban ugy valasztunk, hogy fogyjon az output nelkuli kapuk szama:
	set< pair<int, Gate::GatePtr> > randomOrdered;
	//mindhez Gate-hez rendelunk egy veletlen int-et, ez lesz a pair elso tagja
	//a set-en beluli sorrendet a pair elso tagja hatarozza meg (egyezes eseten a masodik)
	for(set<Gate::GatePtr>::iterator iter=deadEnds.begin(); iter!=deadEnds.end(); iter++)
	{
		randomOrdered.insert(pair<int, Gate::GatePtr>(rand(), *iter));
	}
	while(randomOrdered.size()>1)
	{
		Gate::GatePtr newGate, inp1, inp2;

		set< pair<int, Gate::GatePtr> >::iterator iter(randomOrdered.begin());
		inp1 = (iter->second);
		randomOrdered.erase(iter);
		iter = randomOrdered.begin();
		inp2 = (iter->second);
		randomOrdered.erase(iter);

		if(rand()%2)
		{
			newGate = make_shared<AndGate>(inp1, inp2);
		}
		else
		{
			newGate = make_shared<OrGate>(inp1, inp2);
		}

		randomOrdered.insert(pair<int, Gate::GatePtr>(rand(), newGate));
	}
	output = randomOrdered.begin()->second;
}

Circuit::Circuit(Circuit& other):
	inputs(other.inputs.size())
{
	for(unsigned int i=0; i<other.inputs.size(); i++)
	{
		inputs[i] = static_pointer_cast<Input>(other.inputs[i]->getCopyTwin());
	}
	this->output = other.output->getCopyTwin();
	other.output->resetFlagRecursive(Gate::TWIN);
}

Circuit::~Circuit()
{
	/*output->deleteRecursively();
	for each(Input::InputPtr input in inputs)
	{
		delete input;
	}*/
}

bool Circuit::operator == (Circuit& other)
{
	if(this->inputs.size() != other.inputs.size())	return false;

	for(unsigned int i=0; i<inputs.size(); i++)
	{
		(*this)[i].setValue(false);
		other[i].setValue(false);
	}
	
	if(this->getOutputValue() != other.getOutputValue()) {return false;}
	
	unsigned int currentIndex = 0;
	while(currentIndex<inputs.size())
	{
		if((*this)[currentIndex].getValue())
		{
			(*this)[currentIndex].setValue(false);
			other[currentIndex++].setValue(false);
			//átvitel van, változtatjuk a következõ inputot is
		}
		else
		{
			(*this)[currentIndex].setValue(true);
			other[currentIndex].setValue(true);
			currentIndex = 0;
			//nincs több átvitel, tesztelhetõ az output értéke

			if(this->getOutputValue() != other.getOutputValue())
			{
				return false;
			}
		}
	}
	return true;
}

Input& Circuit::operator[](unsigned int id)
{
	return *inputs[id];
}

void Circuit::setOutput(Gate::GatePtr out)
{
	this->output = out;
}

bool Circuit::getOutputValue()
{
	bool r = output->getValue();
	output->resetFlagRecursive(Gate::MARKED);
	return r;
}

unsigned int Circuit::getSize()
{
	unsigned int size = output->getSize();
	output->resetFlagRecursive(Gate::MARKED);
	return size;
}

std::string Circuit::toStringInfix()
{
	string formula;
	formula.reserve(output->getInfixLength());
	output->resetFlagRecursive(Gate::MARKED);
	output->addToStringInfix(formula);
	return formula;
}

std::string Circuit::toStringPrefix()
{
	string formula;
	formula.reserve(output->getPrefixLength());
	output->resetFlagRecursive(Gate::MARKED);
	output->addToStringPrefix(formula);
	return formula;
}

std::string Circuit::toStringPostfix()
{
	string formula;
	formula.reserve(output->getPrefixLength());	//pre- és postfix hossz megegyezik
	output->resetFlagRecursive(Gate::MARKED);
	output->addToStringPostfix(formula);
	return formula;
}

void Circuit::removeNotGates()
{
	Gate::GatePtr oldOutput(output);	//az eredetit megtartjuk, és a másolatán hajtjuk végre az algoritmust, ezért akkor is marad minden kapura strong reference, ha a másolatból törlünk
	output = output->getCopyTwin();		//TODO: normális megoldást keresni
	for(unsigned int i=0; i<inputs.size(); i++){inputs[i]=static_pointer_cast<Input>(inputs[i]->getCopyTwin());}	//az inputokat is átállítjuk a másolatra
	while(output->type == Gate::NOT)
	{
		if(output->inputs.front()->getInput()->type == Gate::INPUT)	//ha a NOT közvetlenül egy INPUT-hoz kapcsolódik 
		{
			break;
		}
		else
		{
			output = output->getNegatedTwin()->getNegatedTwin();
		}
	}
	output->removeNotGatesRecursively();
	output->resetFlagRecursive(Gate::MARKED | Gate::TWIN);
}

void Circuit::removeMultiGates()
{
	while((output->type == Gate::MULTI_AND || output->type == Gate::MULTI_OR) && output->inputs.size()==1)
	{
		output = output->inputs.front()->getInput();
	}
	output->removeMultiGatesRecursively();

	output->resetFlagRecursive(Gate::MARKED | Gate::TWIN);
}

void Circuit::toDnf()
{
	//list<Gate::GatePtr> garbage;	//az itt tárolt shared_ptr-ek biztosítják, hogy az algoritmus alatt érvényes maradjon twin-ben a weak_ptr
	Gate::GatePtr oldOutput(output);
	output = output->getDnf();

	output->setMarkingRecursively();
	output->resetFlagRecursive(Gate::TWIN | Gate::MARKED );
}

void Circuit::reorderGates()
{
	removeNotGates();
	Gate::GatePtr newOutput = output->getMultiTwin();
	if(output==newOutput){return;}
	//output->deleteRecursively();
	output = newOutput;
	convertToSingleOptimized();
}

void Circuit::convertToSingleOptimized()
{
	MultiGate::occurrenceList occurrences;
	output->addOccurrencesRecursive(occurrences, Gate::MULTI_AND);
	output->resetFlagRecursive(Gate::MARKED);

	struct occurrenceComparator
	{
		bool operator()(const MultiGate::occurrenceIterator& left, const MultiGate::occurrenceIterator& right)
		{
			if(left->second.size()>right->second.size()) {return true;}
			if(left->second.size()<right->second.size()) {return false;}
			return (left->first < right->first);
		}
	};
	set<MultiGate::occurrenceIterator, occurrenceComparator> occurrencesOrdered;	//egy set, ami elsõsorban az iterator által mutatott list hossza alapján rendez, fordított sorrendben
	for(MultiGate::occurrenceIterator i=occurrences.begin(); i!=occurrences.end(); i++)
	{
		occurrencesOrdered.insert(i);
	}

	while(!occurrencesOrdered.empty())
	{
		MultiGate::occurrenceIterator highest(*occurrencesOrdered.begin());
		occurrencesOrdered.erase(occurrencesOrdered.begin());

		updateOccurrences(highest);
		if(occurrencesOrdered.empty() || highest->second.size()>=(*occurrencesOrdered.begin())->second.size())
		{
			if(highest->second.empty())
			{
				occurrencesOrdered.clear();	//ha üres a lista, az összes további is üres lenne, nem kell tovább menni
				break;
			}
			
			Gate::GatePtr newGate = make_shared<AndGate>(highest->first.first, highest->first.second);
			list<MultiGate::occurrenceIterator> newElements;	//az új Gate-hez tartozó occurrenceIterator-ok
			for(list<MultiGate::MultiGatePtr>::iterator i=highest->second.begin(); i!=highest->second.end(); i++)
			{
				(*i)->replaceInputs(highest->first.first, highest->first.second, newGate, occurrences, newElements);
			}
			for(list<MultiGate::occurrenceIterator>::iterator i = newElements.begin(); i!=newElements.end(); i++)
			{
				occurrencesOrdered.insert(*i);
			}
		}
		else
		{
			occurrencesOrdered.insert(highest);
		}
	}
	
	//removeMultiGates();
	output = output->getSingleTwin();
	output->resetFlagRecursive(Gate::TWIN);
	occurrences.clear();
	output->addOccurrencesRecursive(occurrences, Gate::MULTI_OR);
	output->resetFlagRecursive(Gate::MARKED);

	for(MultiGate::occurrenceIterator i=occurrences.begin(); i!=occurrences.end(); i++)
	{
		occurrencesOrdered.insert(i);
	}

	while(!occurrencesOrdered.empty())
	{
		MultiGate::occurrenceIterator highest(*occurrencesOrdered.begin());
		occurrencesOrdered.erase(occurrencesOrdered.begin());

		updateOccurrences(highest);
		if(occurrencesOrdered.empty() || highest->second.size()>=(*occurrencesOrdered.begin())->second.size())
		{
			if(highest->second.empty())
			{
				occurrencesOrdered.clear();	//ha üres a lista, az összes további is üres lenne, nem kell tovább menni
				break;
			}
			
			Gate::GatePtr newGate = make_shared<OrGate>(highest->first.first, highest->first.second);
			list<MultiGate::occurrenceIterator> newElements;	//az új Gate-hez tartozó occurrenceIterator-ok
			for(list<MultiGate::MultiGatePtr>::iterator i=highest->second.begin(); i!=highest->second.end(); i++)
			{
				(*i)->replaceInputs(highest->first.first, highest->first.second, newGate, occurrences, newElements);
			}
			for(list<MultiGate::occurrenceIterator>::iterator i = newElements.begin(); i!=newElements.end(); i++)
			{
				occurrencesOrdered.insert(*i);
			}
		}
		else
		{
			occurrencesOrdered.insert(highest);
		}
	}
	
	//removeMultiGates();
	output = output->getSingleTwin();
	output->resetFlagRecursive(Gate::TWIN);
}

/*void Circuit::reorderGates()
{
	removeNotGates();
	Gate::GatePtr newOutput = output->getMultiTwin();
	if(output==newOutput){return;}
	//output->deleteRecursively();

	MultiGate::occurrenceList occurrences;
	newOutput->addOccurrencesRecursive(occurrences, Gate::MULTI_AND);
	newOutput->resetFlagRecursive(Gate::MARKED);

	struct occurrenceComparator
	{
		bool operator()(const MultiGate::occurrenceIterator& left, const MultiGate::occurrenceIterator& right)
		{
			if(left->second.size()>right->second.size()) {return true;}
			if(left->second.size()<right->second.size()) {return false;}
			return (left->first < right->first);
		}
	};
	set<MultiGate::occurrenceIterator, occurrenceComparator> occurrencesOrdered;	//egy set, ami elsõsorban az iterator által mutatott list hossza alapján rendez, fordított sorrendben
	for(MultiGate::occurrenceIterator i=occurrences.begin(); i!=occurrences.end(); i++)
	{
		occurrencesOrdered.insert(i);
	}

	while(!occurrencesOrdered.empty())
	{
		MultiGate::occurrenceIterator highest(*occurrencesOrdered.begin());
		occurrencesOrdered.erase(occurrencesOrdered.begin());

		updateOccurrences(highest);
		if(occurrencesOrdered.empty() || highest->second.size()>=(*occurrencesOrdered.begin())->second.size())
		{
			if(highest->second.empty())
			{
				occurrencesOrdered.clear();	//ha üres a lista, az összes további is üres lenne, nem kell tovább menni
				break;
			}
			
			Gate::GatePtr newGate = make_shared<AndGate>(highest->first.first, highest->first.second);
			list<MultiGate::occurrenceIterator> newElements;	//az új Gate-hez tartozó occurrenceIterator-ok
			for(list<MultiGate::MultiGatePtr>::iterator i=highest->second.begin(); i!=highest->second.end(); i++)
			{
				(*i)->replaceInputs(highest->first.first, highest->first.second, newGate, occurrences, newElements);
			}
			for(list<MultiGate::occurrenceIterator>::iterator i = newElements.begin(); i!=newElements.end(); i++)
			{
				occurrencesOrdered.insert(*i);
			}
		}
		else
		{
			occurrencesOrdered.insert(highest);
		}
	}
	
	removeMultiGates();
	newOutput->resetFlagRecursive(Gate::MARKED);
	occurrences.clear();
	newOutput->addOccurrencesRecursive(occurrences, Gate::MULTI_OR);
	newOutput->resetFlagRecursive(Gate::MARKED);

	for(MultiGate::occurrenceIterator i=occurrences.begin(); i!=occurrences.end(); i++)
	{
		occurrencesOrdered.insert(i);
	}

	while(!occurrencesOrdered.empty())
	{
		MultiGate::occurrenceIterator highest(*occurrencesOrdered.begin());
		occurrencesOrdered.erase(occurrencesOrdered.begin());

		updateOccurrences(highest);
		if(occurrencesOrdered.empty() || highest->second.size()>=(*occurrencesOrdered.begin())->second.size())
		{
			if(highest->second.empty())
			{
				occurrencesOrdered.clear();	//ha üres a lista, az összes további is üres lenne, nem kell tovább menni
				break;
			}
			
			Gate::GatePtr newGate = make_shared<OrGate>(highest->first.first, highest->first.second);
			list<MultiGate::occurrenceIterator> newElements;	//az új Gate-hez tartozó occurrenceIterator-ok
			for(list<MultiGate::MultiGatePtr>::iterator i=highest->second.begin(); i!=highest->second.end(); i++)
			{
				(*i)->replaceInputs(highest->first.first, highest->first.second, newGate, occurrences, newElements);
			}
			for(list<MultiGate::occurrenceIterator>::iterator i = newElements.begin(); i!=newElements.end(); i++)
			{
				occurrencesOrdered.insert(*i);
			}
		}
		else
		{
			occurrencesOrdered.insert(highest);
		}
	}
	
	removeMultiGates();
	newOutput->resetFlagRecursive(Gate::MARKED);

	output = newOutput;
}*/

void Circuit::updateOccurrences(MultiGate::occurrenceIterator iter)
{
	list<MultiGate::MultiGatePtr>& occList = iter->second;
	if(occList.empty()){return;}

	list<MultiGate::MultiGatePtr>::iterator listIter = occList.begin();
	while(listIter != occList.end())
	{
		if((*listIter)->hasInput(iter->first.first) && (*listIter)->hasInput(iter->first.second))
		{
			listIter++;
		}
		else
		{
			listIter = occList.erase(listIter);
		}
	}
}

void Circuit::simplify()
{
	removeNotGates();
	output = output->getMultiTwin();
	output->resetFlagRecursive(Gate::TWIN | Gate::MULTI_INPUTS);
	
	output->simplifyRecursively();
	output = output->getConstantFreeTwin();
	output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni

	convertToSingleOptimized();
}

void Circuit::simplifyIterative()
{
	removeNotGates();
	output = output->getMultiTwin();
	output->resetFlagRecursive(Gate::TWIN | Gate::MULTI_INPUTS);
	
	unsigned int oldSize = getSize();
	while(true)
	{
		output->simplifyRecursively();
		output = output->getConstantFreeTwin();
		output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
		unsigned int newSize = getSize();
		if(oldSize <= newSize){break;}
	}
	convertToSingleOptimized();
}