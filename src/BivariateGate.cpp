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

const std::set<Gate*>& BivariateGate::getMultiInputs()
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