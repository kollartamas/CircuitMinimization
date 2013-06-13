#include "OrGate.h"
#include "BivariateGate.h"
#include "Gate.h"
#include "AndGate.h"
#include "Link.h"
#include "MultiOr.h"

#include <algorithm>
#include <iterator>

#include <iostream>	//ideiglenes

using namespace std;

bool OrGate::getValue()
{
	if(!isFlagged(MARKED))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;
		value = ((*iter1)->getInput()->getValue()) || ((*iter2)->getInput()->getValue());
		setFlag(MARKED);
	}
	return value;
}

OrGate::OrGate(GatePtr in1, GatePtr in2):BivariateGate(in1, in2)
{
	type = OR;
}

void OrGate::addToStringInfix(std::string& dest)
{
	list<Link*>::iterator iter(inputs.begin());
	dest += '(';
	(*iter)->getInput()->addToStringInfix(dest);
	dest += '+';
	(*++iter)->getInput()->addToStringInfix(dest);
	dest += ')';
}

void OrGate::addToStringPrefix(std::string& dest)
{
	list<Link*>::iterator iter(inputs.begin());
	dest += '+';
	dest += ' ';
	(*iter)->getInput()->addToStringPrefix(dest);
	(*++iter)->getInput()->addToStringPrefix(dest);
}

void OrGate::addToStringPostfix(std::string& dest)
{
	list<Link*>::iterator iter(inputs.begin());
	(*iter)->getInput()->addToStringPostfix(dest);
	(*++iter)->getInput()->addToStringPostfix(dest);
	dest += '+';
	dest += ' ';
}

Gate::GatePtr OrGate::getNegatedTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter(inputs.begin());
		GatePtr newInp1 = (*iter)->getInput()->getNegatedTwin();
		GatePtr newInp2 = (*++iter)->getInput()->getNegatedTwin();
		GatePtr twinPtr(make_shared<AndGate>(newInp1, newInp2));
		setTwinStrong(twinPtr);
		twinPtr->setTwinWeak(shared_from_this());
	}

	return getTwinSafe();
}

Gate::GatePtr OrGate::getCopyTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;
		setTwinStrong(make_shared<OrGate>((*iter1)->getInput()->getCopyTwin(), (*iter2)->getInput()->getCopyTwin()));
	}
	return getTwinStrong();
}

Gate::GatePtr OrGate::getMultiTwin()
{
	//használja TWIN és MULTI_INPUTS flageket
	if(!isFlagged(TWIN))
	{
		MultiGate::MultiGatePtr newMulti(make_shared<MultiOr>());
		for each(Gate* oldInput in this->getMultiInputs() )
		{
			newMulti->addInput(oldInput->getMultiTwin());
		}
		setTwinStrong(newMulti);
	}
	return getTwinStrong();
}

Gate::GatePtr OrGate::getSingleTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter(inputs.begin());
		GatePtr inp1((*iter)->getInput()->getSingleTwin());
		GatePtr inp2((*++iter)->getInput()->getSingleTwin());

		GatePtr newGate(make_shared<OrGate>(inp1, inp2));
		setTwinStrong(newGate);
		newGate->setTwinWeak(newGate);	//TODO: megnézni erre szükség van-e (getTwinSafe helyett getTwinStrong-gal)
		
	}
	return getTwinSafe();
}

Gate::GatePtr OrGate::getConstantFreeTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter(inputs.begin());
		GatePtr inp1((*iter)->getInput()->getConstantFreeTwin());
		GatePtr inp2((*++iter)->getInput()->getConstantFreeTwin());

		if(inp1->type == TRUE || inp2->type == FALSE)
		{
			setTwinStrong(inp1);
		}
		else if(inp1->type == FALSE || inp2->type == TRUE)
		{
			setTwinStrong(inp2);
		}
		else if(inp1 == inputs.front()->getInput() && inp2 == (*iter)->getInput())	//ha az eredeti inputjai konstans-mentesek, önmagával tér vissza
		{
			setTwinWeak(shared_from_this());
		}
		else
		{
			GatePtr newGate(make_shared<OrGate>(inp1, inp2));
			setTwinStrong(newGate);
			newGate->setTwinWeak(newGate);	//beállítjuk saját twin-jének, mert tudjuk hogy az inputjai és õ is konstans-mentes
		}
	}
	return getTwinSafe();
}

/*list<list<Gate*>>& OrGate::getDnf()
{
	if(!marked)
	{
		normalForm = list<list<Gate*>>();//(*iter)->getInput().getDnf().size()+(*++iter)->getInput().getDnf().size());
		list<list<Gate*>>::iterator begin1((*iter)->getInput().getDnf().begin());
		list<list<Gate*>>::iterator end1((*iter)->getInput().getDnf().end());
		list<list<Gate*>>::iterator begin2((*++iter)->getInput().getDnf().begin());
		list<list<Gate*>>::iterator end2((*++iter)->getInput().getDnf().end());
		back_insert_iterator<list<list<Gate*>>> result(normalForm);	//back_insert_iterator értékadás helyett push_back()-et hív
		set_union(begin1, end1, begin2, end2, result);	//tartalom összehasonlítása alapértelmezetten lexicographical_compare() alapján

		/*while (true)
		{
			if (first1==last1)						//
			{										//
				while (first2!=last2)				//
				{									//	std::copy azzal a változtatással, hogy nem kell elõre beállítani a container méretét
					normalForm.push_back(*first2);	//
					++first2;						//
				}									//
			}										//
			if (first2==last2)
			{
				while (first1!=last1)
				{
					normalForm.push_back(*first1);
					++first1;
				}
			}
			//lexikografikus sorrendbe összefésüljük:
			if (*first1<*first2) { normalForm.push_back(*first1); ++first1; }
			else if (*first2<*first1) { normalForm.push_back(*first2); ++first2; }
			else { normalForm.push_back(*first1); ++first1; ++first2; }
		}

		marked = true;
	}
	return normalForm;
}*/

Gate::GatePtr OrGate::getDnf()
{
	if(!isFlagged(TWIN))
	{
		
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;

		GatePtr dnf1((*iter1)->getInput()->getDnf());
		GatePtr dnf2((*iter2)->getInput()->getDnf());
		if(dnf1 == (*iter1)->getInput() && dnf2 == (*iter2)->getInput())
		{
			setTwinWeak(shared_from_this());
		}
		else
		{
			setTwinStrong(make_shared<OrGate>(dnf1, dnf2));
		}
	}
	return getTwinSafe();
}