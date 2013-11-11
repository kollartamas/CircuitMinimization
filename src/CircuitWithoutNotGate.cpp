#include "Circuit.h"
#include "CircuitWithoutNotGate.h"
#include "AndGate.h"
#include "OrGate.h"
#include "NotGate.h"
#include "Link.h"
#include "Logger.h"

#include <set>
#include <iostream>	//ideiglenes

using namespace std;

CircuitWithoutNotGate::CircuitWithoutNotGate(Circuit& other) : Circuit(other)
{
	this->removeNotGates();
}

CircuitWithoutNotGate::CircuitWithoutNotGate(CircuitWithoutNotGate& other) : Circuit(other)
{}

CircuitWithoutNotGate::CircuitWithoutNotGate(unsigned int inputokSzama) : Circuit(inputokSzama)
{}

void CircuitWithoutNotGate::toDnf()
{
	const set<Implicant>& implicantSet = output->getDnf(inputs.size());
	Gate::GatePtr oldOutput(output);
	output = buildFromDnf(implicantSet);
	oldOutput->resetFlagRecursive(Gate::NORMAL_FORM);
}

void CircuitWithoutNotGate::reorderGates()
{
	Gate::GatePtr newOutput = output->getMultiTwin();
	if(output==newOutput){return;}
	//output->deleteRecursively();
	output = newOutput;
	convertToSingleOptimized();
}

void CircuitWithoutNotGate::convertToSingleOptimized()
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

void CircuitWithoutNotGate::updateOccurrences(MultiGate::occurrenceIterator iter)
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

void CircuitWithoutNotGate::simplify()
{
	output = output->getMultiTwin();
	output->resetFlagRecursive(Gate::TWIN | Gate::MULTI_INPUTS);
	
	output->simplifyRecursively();
	output = output->getConstantFreeTwin();
	output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni

	convertToSingleOptimized();
}

void CircuitWithoutNotGate::simplifyIterative()
{
	output = output->getMultiTwin();
	output->resetFlagRecursive(Gate::TWIN | Gate::MULTI_INPUTS);
	unsigned int oldSize = getSize();
//cout << "toSingle "<< oldSize << endl;
	while(true)
	{
		output->simplifyRecursively();
		output = output->getConstantFreeTwin();
		output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
		unsigned int newSize = getSize();
//cout << "simplify "<< newSize << endl;
		if(oldSize <= newSize){break;}
		oldSize = newSize;
	}
	convertToSingleOptimized();	//TODO: összehasonlítani a cikluson belül és kívül
//cout << "toSingle "<< getSize() << endl;

	/*unsigned int oldSize = getSize();
	while(true)
	{
	output = output->getMultiTwin();
	output->resetFlagRecursive(Gate::TWIN | Gate::MULTI_INPUTS);
		output->simplifyRecursively();
		output = output->getConstantFreeTwin();
		output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
	convertToSingleOptimized();	//TODO: összehasonlítani a cikluson belül és kívül
		unsigned int newSize = getSize();
cout << "simplify "<< newSize << endl;
		if(oldSize <= newSize){break;}
		oldSize = newSize;
	}*/
}

void CircuitWithoutNotGate::simplify2()
{
	output->simplifyRecursively2();
	output = output->getConstantFreeTwin();
	output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN | Gate::MULTI_INPUTS_LINK);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
}

void CircuitWithoutNotGate::simplifyIterative2()
{
	unsigned int oldSize = getSize();
	while(true)
	{
//cout << "start simplify "<< endl;
		output->simplifyRecursively3();
/*output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN | Gate::MULTI_INPUTS_LINK);
printStructure();*/
		output = output->getConstantFreeTwin();
//printStructure();
		output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN | Gate::MULTI_INPUTS_LINK);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
		unsigned int newSize = getSize();
//cout << "end simplify "<< newSize << endl;
		if(oldSize <= newSize){break;}
		oldSize = newSize;
	}
}

void CircuitWithoutNotGate::simplify3()
{
	output->simplifyRecursively3();
	output = output->getConstantFreeTwin();
	output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN | Gate::MULTI_INPUTS_LINK);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
}

void CircuitWithoutNotGate::simplifyIterative3()
{
	unsigned int oldSize = getSize();
	while(true)
	{
//cout << "start simplify "<< endl;
		output->simplifyRecursively3();
/*output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN | Gate::MULTI_INPUTS_LINK);
printStructure();*/
		output = output->getConstantFreeTwin();
//printStructure();
		output->resetFlagRecursive(Gate::MARKED | Gate::LEVEL | Gate::TWIN | Gate::MULTI_INPUTS_LINK);	//TODO: átgondolni hogy nem kell-e level-t hamarabb resetelni
		unsigned int newSize = getSize();
//cout << "end simplify "<< newSize << endl;
		if(oldSize <= newSize){break;}
		oldSize = newSize;
	}
}

void CircuitWithoutNotGate::factorize()
{
	Gate::GatePtr temp( make_shared<NotGate>(output) );	//TODO: temporary NOT kapu nélkül megoldani
	output->factorizeRecursively();
	output = temp->inputs.front()->getInput();
	output->setMarkingRecursively();
	output->resetFlagRecursive(Gate::MARKED | Gate::FACTORIZED | Gate::MULTI_INPUTS_SINGLE_OUTPUT | Gate::MULTI_INPUTS_DOUBLE_OUTPUT);
}

void CircuitWithoutNotGate::factorizeIteratively()
{
	int oldSize = getSize();
	int newSize = 0;
	bool cont(true);
	while(cont)
	{
//cout << "start factorize "<< getSize() << endl;
		Gate::GatePtr temp( make_shared<NotGate>(output) );	//TODO: temporary NOT kapu nélkül megoldani
		cont = output->factorizeRecursively();
		output = temp->inputs.front()->getInput();
		output->setMarkingRecursively();
		output->resetFlagRecursive(Gate::MARKED | Gate::FACTORIZED | Gate::MULTI_INPUTS_SINGLE_OUTPUT | Gate::MULTI_INPUTS_DOUBLE_OUTPUT);
//cout << "end factorize "<< getSize() << endl;
		
		newSize = getSize();
		if(oldSize < newSize){break;}
		oldSize = newSize;
	}
}

void CircuitWithoutNotGate::optimizeIteratively()
{
#ifdef LOG_MAX_GATENUM
	Logger::getLogger().setData(Logger::STARTING_GATENUM, Gate::constrCall-Gate::destrCall);
#endif
#ifdef LOG_SIZE
	Logger::getLogger().setData(Logger::STARTING_SIZE, getSize());
	Logger::getLogger().setData(Logger::DNF_SIZE, 0);
#endif
#ifdef LOG_LEVEL
	Logger::getLogger().setData(Logger::STARTING_LEVEL, getLevel());
#endif
#ifdef LOG_TIME
	Logger::getLogger().setData(Logger::DNF_TIME, 0);
	Logger::getLogger().updateTimer();
#endif

	int oldSize = getSize();
	int newSize = 0;
	while(true)
	{
		simplifyIterative3();
		factorizeIteratively();
//cout << "start multi" << endl;
		output = output->getMultiTwin();
		output->resetFlagRecursive(Gate::TWIN | Gate::MULTI_INPUTS);
//cout << "start single" << endl;
		convertToSingleOptimized();
//cout << "end single" << endl;
		
		newSize = getSize();
		if(oldSize <= newSize){break;}
		oldSize = newSize;
//cout << newSize << endl;
	}

#ifdef LOG_TIME
	Logger::getLogger().setTime(Logger::FINISH_TIME);
#endif
#ifdef LOG_SIZE
	Logger::getLogger().setData(Logger::FINAL_SIZE, getSize());
#endif
#ifdef LOG_LEVEL
	Logger::getLogger().setData(Logger::FINAL_LEVEL, getLevel());
#endif

}