#include "AndGate.h"
#include "BivariateGate.h"
#include "Gate.h"
#include "OrGate.h"
#include "FalseGate.h"
#include "Link.h"
#include "MultiAnd.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <list>
#include <set>

using namespace std;

AndGate::AndGate(GatePtr in1, GatePtr in2):BivariateGate(in1, in2)
{
	type = AND;
}

bool AndGate::getValue()
{
	if(!isFlagged(MARKED))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;
		value = ((*iter1)->getInput()->getValue()) && ((*iter2)->getInput()->getValue());
		setFlag(MARKED);
	}
	return value;
}

void AndGate::addToStringInfix(std::string& dest)
{
	list<Link*>::iterator iter(inputs.begin());
	dest += '(';
	(*iter)->getInput()->addToStringInfix(dest);
	dest += '*';
	(*++iter)->getInput()->addToStringInfix(dest);
	dest += ')';
}

void AndGate::addToStringPrefix(std::string& dest)
{
	list<Link*>::iterator iter(inputs.begin());
	dest += '*';
	dest += ' ';
	(*iter)->getInput()->addToStringPrefix(dest);
	(*++iter)->getInput()->addToStringPrefix(dest);
}

void AndGate::addToStringPostfix(std::string& dest)
{
	list<Link*>::iterator iter(inputs.begin());
	(*iter)->getInput()->addToStringPostfix(dest);
	(*++iter)->getInput()->addToStringPostfix(dest);
	dest += '*';
	dest += ' ';
}

Gate::GatePtr AndGate::getNegatedTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;

		GatePtr oldInp1((*iter1)->getInput());
		GatePtr newInp1 ( oldInp1->getNegatedTwin());
		GatePtr oldInp2((*iter2)->getInput());
		GatePtr newInp2 ( oldInp2->getNegatedTwin());

		GatePtr twinPtr(make_shared<OrGate>(newInp1, newInp2));
		setTwinStrong(twinPtr);
		twinPtr->setTwinWeak(shared_from_this());
	}

	return getTwinSafe();
}

Gate::GatePtr AndGate::getCopyTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;
		setTwinStrong(make_shared<AndGate>((*iter1)->getInput()->getCopyTwin(), (*iter2)->getInput()->getCopyTwin()));
	}
	return getTwinStrong();
}

Gate::GatePtr AndGate::getMultiTwin()
{
	//használja TWIN és MULTI_INPUTS flageket
	if(!isFlagged(TWIN))
	{
		MultiGate::MultiGatePtr newMulti(make_shared<MultiAnd>());
		for each(Gate* oldInput in this->getMultiInputs() )
		{
			newMulti->addInput(oldInput->getMultiTwin());
		}
		setTwinStrong(newMulti);
	}
	return getTwinStrong();
}

Gate::GatePtr AndGate::getSingleTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter(inputs.begin());
		GatePtr inp1((*iter)->getInput()->getSingleTwin());
		GatePtr inp2((*++iter)->getInput()->getSingleTwin());

		GatePtr newGate(make_shared<AndGate>(inp1, inp2));
		setTwinStrong(newGate);
		newGate->setTwinWeak(newGate);	//TODO: megnézni erre szükség van-e (getTwinSafe helyett getTwinStrong-gal)
		
	}
	return getTwinSafe();
}

Gate::GatePtr AndGate::getConstantFreeTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter(inputs.begin());
		GatePtr inp1((*iter)->getInput()->getConstantFreeTwin());
		GatePtr inp2((*++iter)->getInput()->getConstantFreeTwin());

		if(inp1->type == FALSE || inp2->type == TRUE)
		{
			setTwinStrong(inp1);
		}
		else if(inp1->type == TRUE || inp2->type == FALSE)
		{
			setTwinStrong(inp2);
		}
		else if(inp1 == inputs.front()->getInput() && inp2 == (*iter)->getInput())	//ha az eredeti inputjai konstans-mentesek, önmagával tér vissza
		{
			setTwinWeak(shared_from_this());
		}
		else
		{
			GatePtr newGate(make_shared<AndGate>(inp1, inp2));
			setTwinStrong(newGate);
			newGate->setTwinWeak(newGate);	//beállítjuk saját twin-jének, mert tudjuk hogy az inputjai és õ is konstans-mentes
		}
	}
	return getTwinSafe();
}

/*list<list<Gate*>>& AndGate::getDnf()
{
	if(!marked)
	{
		normalForm = list<list<Gate*>>();

		list<list<Gate*>>& dnf1 = inputs[0]->getInput().getDnf();
		list<list<Gate*>>& dnf2 = inputs[1]->getInput().getDnf();
		for (list<list<Gate*>>::/*const_/iterator clause1=dnf1.begin(); clause1!=dnf1.end(); ++clause1 )
		{
			for (list<list<Gate*>>::/*const_iterator clause2=dnf2.begin(); clause2!=dnf2.end(); ++clause2 )
			{
				list<Gate*> newClause;
				back_insert_iterator<list<Gate*>> resultIterator(newClause);

				list<Gate*>::/*const_/iterator begin1(clause1->begin());
				list<Gate*>::/*const_/iterator end1(clause1->end());
				list<Gate*>::/*const_/iterator begin2(clause2->begin());
				list<Gate*>::/*const_/iterator end2(clause2->end());
				set_union(begin1, end1, begin2, end2, resultIterator);	//tartalom összehasonlítása alapértelmezetten lexicographical_compare() alapján
				normalForm.push_back(newClause);
			}
		}
		normalForm.sort();
		
		marked = true;
	}
	return normalForm;
}*/


Gate::GatePtr AndGate::getDnf()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter1(inputs.begin());
		list<Link*>::iterator iter2(inputs.begin());
		++iter2;

		GatePtr dnf1((*iter1)->getInput()->getDnf());
		GatePtr dnf2((*iter2)->getInput()->getDnf());
		if(dnf1->type!=Gate::OR && dnf2->type!=Gate::OR && dnf1 == (*iter1)->getInput() && dnf2 == (*iter2)->getInput())
		{
			setTwinWeak(shared_from_this());
		}
		else
		{
			setTwinSafe(conjunctionOfDnfs(dnf1, dnf2));
			dnf1->resetFlagRecursive(Gate::MARKED);
			/*garbage.push_back(dnf1);
			garbage.push_back(dnf2);*/
		}
		setFlag(TWIN);
		
		//resetFlagRecursive(Gate::MARKED);
	}
	return getTwinSafe();
}

Gate::GatePtr AndGate::conjunctionOfDnfs(GatePtr dnf1, GatePtr dnf2)
{
	//nem ellenõrizzük hogy a kapott formulák tényleg dnf alakúak-e
	//a getDnf függvényben szükségszerûen dnf paramétereket kap, ha csak onnan hívjuk az ellenõrzés redundáns

	if(!dnf1->isFlagged(MARKED))
	{
		dnf1->setFlag(MARKED);
		GatePtr gate1, gate2;

		if(dnf1->type != Gate::OR)
		{
			gate1 = conjunctionOfClauseAndDnf(dnf1, dnf2);
			gate2 = NULL;
			dnf2->resetFlagRecursive(MARKED2);
		}
		else if(dnf2->type != Gate::OR)
		{
			gate1 = NULL;
			gate2 = conjunctionOfClauseAndDnf(dnf2, dnf1);
			dnf1->resetFlagRecursive(MARKED2);
		}
		else
		{
			list<Link*>::iterator iter(dnf1->inputs.begin());
			gate1 = (*iter)->getInput();
			gate2 = (*++iter)->getInput();

			if(gate1->type == Gate::OR)
			{
				gate1 = conjunctionOfDnfs(gate1, dnf2);
			}
			else
			{
				gate1 = conjunctionOfClauseAndDnf(gate1, dnf2);
				dnf2->resetFlagRecursive(MARKED2);
			}

			if(gate2->type == Gate::OR)
			{
				gate2 = conjunctionOfDnfs(gate2, dnf2);
			}
			else
			{
				gate2 = conjunctionOfClauseAndDnf(gate2, dnf2);
				dnf2->resetFlagRecursive(MARKED2);
			}
		}

		if(gate1)
		{
			if(gate2)
			{
				return make_shared<OrGate>(gate1, gate2);
			}
			else
			{
				return gate1;
			}
		}
		else
		{
			return gate2;	//ekvivalens: if (gate2!=NULL) return gate2; if(gate2==NULL) return NULL;
		}
	}
	return NULL;
}
Gate::GatePtr AndGate::conjunctionOfClauseAndDnf(GatePtr clause, GatePtr dnf)
{
	//nem ellenõrizzük hogy a kapott formulák tényleg dnf alakúak-e, lásd conjunctionOfDnfs()
	if(!dnf->isFlagged(MARKED2))
	{
		dnf->setFlag(MARKED2);
		GatePtr gate1, gate2;

		if(dnf->type != Gate::OR)
		{
			gate1 = make_shared<AndGate>(clause, dnf);
			gate2 = NULL;
		}
		else
		{
			list<Link*>::iterator iter(dnf->inputs.begin());
			gate1 = (*iter)->getInput();
			gate2 = (*++iter)->getInput();
			
			if(gate1->type == Gate::OR)
			{
				gate1 = conjunctionOfClauseAndDnf(clause, gate1);
			}
			else
			{
				gate1 = make_shared<AndGate>(clause, gate1);
			}
				
			if(gate2->type == Gate::OR)
			{
				gate2 = conjunctionOfClauseAndDnf(clause, gate2);
			}
			else
			{
				gate2 = make_shared<AndGate>(clause, gate2);
			}
		}

		if(gate1)
		{
			if(gate2)
			{
				return make_shared<OrGate>(gate1, gate2);
			}
			else
			{
				return gate1;
			}
		}
		else
		{
			return gate2;	//ekvivalens: if (gate2!=NULL) return gate2; if(gate2==NULL) return NULL;
		}
	}
	return NULL;
}