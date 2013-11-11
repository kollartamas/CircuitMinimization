#include "BivariateGate.h"
#include "Link.h"
#include "MultiGate.h"

#include <set>
#include <algorithm>
#include <iterator>

using namespace std;

BivariateGate::BivariateGate(GatePtr in1, GatePtr in2)
{
	new Link(in1, this);
	new Link(in2, this);
}


BivariateGate::~BivariateGate()
{
	delete inputs.front();	//ez egyúttal törli a list-bõl is
	delete inputs.front();
}

/*void BivariateGate::deleteRecursively()
{
	list<Link*>::iterator iter(inputs.begin());
	GatePtr inp1 = (*iter)->getInput();
	GatePtr inp2 = (*++iter)->getInput();

	delete this;

	if(inp1->outputs.empty())
	{
		inp1->deleteRecursively();
	}
	if(&inp1!=&inp2 && inp2->outputs.empty())
	{
		inp2->deleteRecursively();
	}
}*/

unsigned int BivariateGate::getInfixLength()
{
	if(!isFlagged(MARKED))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;
		stringLength = (*iter1)->getInput()->getInfixLength() + 3 + (*iter2)->getInput()->getInfixLength();
		setFlag(MARKED);
	}
	return stringLength;
}

unsigned int BivariateGate::getPrefixLength()
{
	if(!isFlagged(MARKED))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;
		stringLength = 2 + (*iter1)->getInput()->getPrefixLength() + (*iter2)->getInput()->getPrefixLength();
		setFlag(MARKED);
	}
	return stringLength;
}

void BivariateGate::addMultiGateInputs(set<GatePtr>& inputSet)
{
	if(!isFlagged(MARKED))
	{
		list<Link*>::iterator iter(inputs.begin());
		/*list<Link*>::iterator iter2(inputs.begin());
		++iter2;*/
		if((*iter)->getInput()->type == this->type)
		{
			static_pointer_cast<BivariateGate>((*iter)->getInput())->addMultiGateInputs(inputSet);
		}
		else
		{
			inputSet.insert((*iter)->getInput()->getMultiTwin());
		}

		if((*++iter)->getInput()->type == this->type)
		{
			static_pointer_cast<BivariateGate>((*iter)->getInput())->addMultiGateInputs(inputSet);
		}
		else
		{
			inputSet.insert((*iter)->getInput()->getMultiTwin());
		}
		setFlag(MARKED);
	}
}

const set<Gate*>& BivariateGate::getMultiInputs()
{
	if(!isFlagged(MULTI_INPUTS))
	{
		multiInputs.clear();
		list<Link*>::iterator iter(inputs.begin());

		set<Gate*> empty1, empty2;
		const set<Gate*> *inpSet1, *inpSet2;	//azért pointerek, hogy ne legyenek temporary copy-k (reference nem lehet, mert nem deklaráláskor inicializáljuk)

		if((*iter)->getInput()->type == this->type)
		{
			inpSet1 = &(*iter)->getInput()->getMultiInputs();
		}
		else
		{
			inpSet1 = &empty1;
			empty1.insert((*iter)->getInput().get());
		}

		if((*++iter)->getInput()->type == this->type)
		{
			inpSet2 = &(*iter)->getInput()->getMultiInputs();
		}
		else
		{
			inpSet2 = &empty2;
			empty2.insert((*iter)->getInput().get());
		}
		
		set_union(inpSet1->begin(), inpSet1->end(), inpSet2->begin(), inpSet2->end(), std::inserter(multiInputs,multiInputs.begin()));	//std::inserter azért kell, mert set.begin() const iteratort ad
		setFlag(MULTI_INPUTS);
	}
	return multiInputs;
}

const map<Gate::GatePtr, Link*>& BivariateGate::getMultiInputsWithLink()
{
	if(!isFlagged(MULTI_INPUTS_LINK))
	{
		multiInputsWithLink.clear();
		list<Link*>::iterator iter(inputs.begin());

		if((*iter)->getInput()->type == this->type)
		{
			multiInputsWithLink = (*iter)->getInput()->getMultiInputsWithLink();	//lineáris
		}
		else if((*iter)->getInput()->type!=TRUE_GATE && (*iter)->getInput()->type!=FALSE_GATE)
		{
			multiInputsWithLink.insert( pair<GatePtr, Link*>( (*iter)->getInput(), (*iter) ));	//konstans idõben, mert üres
		}

		if((*++iter)->getInput()->type == this->type)
		{
			const map<GatePtr, Link*>& additionalMap = (*iter)->getInput()->getMultiInputsWithLink();
			map<GatePtr, Link*>::iterator iterOriginal(multiInputsWithLink.begin());
			map<GatePtr, Link*>::const_iterator iterAdditional(additionalMap.begin());
			while(iterAdditional!=additionalMap.end())	//összességében lineáris
			{
				if(iterOriginal==multiInputsWithLink.end() || iterOriginal->first > iterAdditional->first)
				{
					multiInputsWithLink.insert(iterOriginal, *iterAdditional++);
				}
				else if( iterOriginal->first < iterAdditional->first )
				{
					++iterOriginal;
				}
				else
				{
					++iterOriginal;
					++iterAdditional;
				}
			}
		}
		else if((*iter)->getInput()->type!=TRUE_GATE && (*iter)->getInput()->type!=FALSE_GATE)
		{
			multiInputsWithLink.insert( pair<GatePtr, Link*>( (*iter)->getInput(), (*iter) ));	//logaritmikus
		}
		setFlag(MULTI_INPUTS_LINK);
	}
	return multiInputsWithLink;
}

const set<Gate*>& BivariateGate::getMultiInputsWithSingleOutput()
{
	if(!isFlagged(MULTI_INPUTS_SINGLE_OUTPUT))
	{
		setFlag(MULTI_INPUTS_SINGLE_OUTPUT);
		multiInputsWithSingleOutput.clear();
		if(outputs.size() > 1)
		{
			multiInputsWithSingleOutput.insert(this);
			return multiInputsWithSingleOutput;
		}

		list<Link*>::iterator iter(inputs.begin());

		set<Gate*> empty1, empty2;
		const set<Gate*> *inpSet1, *inpSet2;	//azért pointerek, hogy ne legyenek temporary copy-k (reference nem lehet, mert nem deklaráláskor inicializáljuk)

		if((*iter)->getInput()->type == this->type)
		{
			inpSet1 = &(*iter)->getInput()->getMultiInputsWithSingleOutput();
		}
		else
		{
			inpSet1 = &empty1;
			empty1.insert((*iter)->getInput().get());
		}

		if((*++iter)->getInput()->type == this->type)
		{
			inpSet2 = &(*iter)->getInput()->getMultiInputsWithSingleOutput();
		}
		else
		{
			inpSet2 = &empty2;
			empty2.insert((*iter)->getInput().get());
		}
		
		set_union(inpSet1->begin(), inpSet1->end(), inpSet2->begin(), inpSet2->end(), std::inserter(multiInputsWithSingleOutput,multiInputsWithSingleOutput.begin()));	//std::inserter azért kell, mert set.begin() const iteratort ad
	}
	return multiInputsWithSingleOutput;
}

const set<Gate*>& BivariateGate::getMultiInputsWithDoubleOutput()
{
	if(!isFlagged(MULTI_INPUTS_DOUBLE_OUTPUT))
	{
		setFlag(MULTI_INPUTS_DOUBLE_OUTPUT);
		multiInputsWithDoubleOutput.clear();

		list<Link*>::iterator iter(inputs.begin());

		set<Gate*> empty1, empty2;
		const set<Gate*> *inpSet1, *inpSet2;	//azért pointerek, hogy ne legyenek temporary copy-k (reference nem lehet, mert nem deklaráláskor inicializáljuk)

		if((*iter)->getInput()->type == this->type)
		{
			if(outputs.size()>1)
			{
				inpSet1 = &(*iter)->getInput()->getMultiInputsWithSingleOutput();
			}
			else
			{
				inpSet1 = &(*iter)->getInput()->getMultiInputsWithDoubleOutput();
			}
		}
		else
		{
			inpSet1 = &empty1;
			empty1.insert((*iter)->getInput().get());
		}

		if((*++iter)->getInput()->type == this->type)
		{
			
			if(outputs.size()>1)
			{
				inpSet2 = &(*iter)->getInput()->getMultiInputsWithSingleOutput();
			}
			else
			{
				inpSet2 = &(*iter)->getInput()->getMultiInputsWithDoubleOutput();
			}
		}
		else
		{
			inpSet2 = &empty2;
			empty2.insert((*iter)->getInput().get());
		}
		
		set_union(inpSet1->begin(), inpSet1->end(), inpSet2->begin(), inpSet2->end(), std::inserter(multiInputsWithDoubleOutput,multiInputsWithDoubleOutput.begin()));	//std::inserter azért kell, mert set.begin() const iteratort ad
	}
	return multiInputsWithDoubleOutput;
}

Gate::GatePtr BivariateGate::removeFactor(Gate* factor)
{
	set<Gate*>::iterator factorInSet = getMultiInputsWithSingleOutput().find(factor);
	if( factorInSet == getMultiInputsWithSingleOutput().end() )
	{
		return shared_from_this();
	}
	else
	{
		multiInputsWithSingleOutput.erase(factorInSet);
		multiInputsWithDoubleOutput.erase(factor);
	}

	GatePtr input1 (inputs.front()->getInput()->shared_from_this());
	GatePtr input2((*++inputs.begin())->getInput()->shared_from_this());
	if(input1 == factor->shared_from_this())
	{
		return input2->type == this->type ? static_pointer_cast<BivariateGate>(input2)->removeFactor(factor) : input2;	//TODO: átírni polimorfikusra
	}
	else if(input2 == factor->shared_from_this())
	{
		return input1->type == this->type ? static_pointer_cast<BivariateGate>(input1)->removeFactor(factor) : input1;	//TODO: átírni polimorfikusra
	}
	else
	{
		if(input1->type == this->type) //TODO: átírni polimorfikusra
		{
			GatePtr retVal( static_pointer_cast<BivariateGate>(input1)->removeFactor(factor) );
			if(retVal != input1)	//TODO: tesztelni nélküle
			{
				inputs.front()->setInput(retVal);
			}
		}
		if(input2->type == this->type) //TODO: átírni polimorfikusra
		{
			GatePtr retVal( static_pointer_cast<BivariateGate>(input2)->removeFactor(factor) );
			if(retVal != input2)	//TODO: tesztelni nélküle
			{
				(*++inputs.begin())->setInput(retVal);
			}
		}
	}
	return shared_from_this();
}
