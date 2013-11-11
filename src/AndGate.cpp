#include "AndGate.h"
#include "BivariateGate.h"
#include "Gate.h"
#include "OrGate.h"
#include "FalseGate.h"
#include "Link.h"
#include "MultiAnd.h"
#include "TrueGate.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <list>
#include <set>
#include <cassert>

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

const set<Implicant>& AndGate::getDnf(unsigned int numOfVariables)
{
	if(!isFlagged(NORMAL_FORM))
	{
		setFlag(NORMAL_FORM);
		normalForm.clear();

		const set<Implicant>& dnf1(inputs.front()->getInput()->getDnf(numOfVariables));
		const set<Implicant>& dnf2((*++inputs.begin())->getInput()->getDnf(numOfVariables));

		for(set<Implicant>::iterator i(dnf1.begin()); i!=dnf1.end(); ++i)
		{
			for(set<Implicant>::iterator j(dnf2.begin()); j!=dnf2.end(); ++j)
			{
				normalForm.insert((*i)&(*j));
			}
		}
	}
	return normalForm;
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
#ifdef VS2010
		for each(Gate* oldInput in this->getMultiInputs() )
#else
		for(Gate* oldInput : this->getMultiInputs() )
#endif
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

		if(inp1->type == FALSE_GATE || inp2->type == TRUE_GATE)
		{
			setTwinStrong(inp1);
		}
		else if(inp1->type == TRUE_GATE || inp2->type == FALSE_GATE)
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


/*Gate::GatePtr AndGate::getDnf()
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
			garbage.push_back(dnf2);
		}
		setFlag(TWIN);
		
		//resetFlagRecursive(Gate::MARKED);
	}
	return getTwinSafe();
}*/

/*Gate::GatePtr AndGate::conjunctionOfDnfs(GatePtr dnf1, GatePtr dnf2)
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
}*/

bool AndGate::factorizeRecursively()
{
	bool changed(false);
	if( !isFlagged(FACTORIZED) )
	{
		for (std::list<Link*>::iterator iter = inputs.begin(); iter != inputs.end(); ++iter)
		{
			changed = (*iter)->getInput()->factorizeRecursively() || changed;
		}
		
		GatePtr input1((*inputs.begin())->getInput());
		GatePtr input2((*++inputs.begin())->getInput());

		if(input1->type!=OR || input2->type!=OR)
		{
			setFlag(FACTORIZED);
			return changed;
		}
		if(input1->outputs.size()>1 && input2->outputs.size()>1)
		{
			setFlag(FACTORIZED);
			return changed;
		}

		list<Gate*> factors;
		/*{
			list<Gate*> temp1;
			list<Gate*> temp2;
			set_intersection(input1->getMultiInputsWithSingleOutput().begin(), input1->getMultiInputsWithSingleOutput().end(), input2->getMultiInputsWithDoubleOutput().begin(), input2->getMultiInputsWithDoubleOutput().end(), temp1.begin());
			set_intersection(input1->getMultiInputsWithDoubleOutput().begin(), input1->getMultiInputsWithDoubleOutput().end(), input2->getMultiInputsWithSingleOutput().begin(), input2->getMultiInputsWithSingleOutput().end(), temp2.begin());
			set_union(temp1.begin(), temp1.end(), temp2.begin(), temp2.end(), factors.begin());
		}*/
		
		set_intersection(input1->getMultiInputsWithSingleOutput().begin(), input1->getMultiInputsWithSingleOutput().end(), input2->getMultiInputsWithDoubleOutput().begin(), input2->getMultiInputsWithDoubleOutput().end(), back_inserter(factors));
		if(!factors.empty()){changed = true;}
		for(list<Gate*>::iterator iter(factors.begin()); iter!=factors.end(); ++iter)
		{
			Gate* currentFactor(*iter);		//TODO: átgondolni, nem szûnhet-e meg korábban a pointer által mutatott gate
			//a kiemelendõ kaput this és az outputjai közé kötjük egy OR kapuval
			GatePtr newGate(make_shared<OrGate>(shared_from_this(), currentFactor->shared_from_this()));
			for(list<Link*>::iterator parentLink(outputs.begin()); (*parentLink)->getOutput()->shared_from_this()!=newGate; parentLink=outputs.begin())
				//setInput(newGate) mindig törli az aktuális parentLink-et outputs közül, ezért outputs.begin() a következõ
				//newGate az utolsó,ezért  (*parentLink)->getOutput()...!=newGate  az összes többi output bejárásáig teljesül
			{
				(*parentLink)->setInput(newGate);
				input1 = static_pointer_cast<OrGate>(input1)->removeFactor(currentFactor);
				inputs.front()->setInput(input1);
				input2 = static_pointer_cast<OrGate>(input2)->extractFactor(currentFactor);
				(*++inputs.begin())->setInput(input2);
			}
		}

		if(input1->type!=OR || input2->type!=OR)	//mert az elõzõek eredményeképp változhattak inputok
		{
			setFlag(FACTORIZED);
			return changed;
		}
		if(input1->outputs.size()>1 && input2->outputs.size()>1)	//TODO: ez szükséges?
		{
			setFlag(FACTORIZED);
			return changed;
		}

		factors.clear();
		set_intersection(input1->getMultiInputsWithDoubleOutput().begin(), input1->getMultiInputsWithDoubleOutput().end(), input2->getMultiInputsWithSingleOutput().begin(), input2->getMultiInputsWithSingleOutput().end(), back_inserter(factors));
		if(!factors.empty()){changed = true;}
		for(list<Gate*>::iterator iter(factors.begin()); iter!=factors.end(); ++iter)
		{
			Gate* currentFactor(*iter);		//TODO: átgondolni, nem szûnhet-e meg korábban a pointer által mutatott gate
			//a kiemelendõ kaput this és az outputjai közé kötjük egy OR kapuval
			GatePtr newGate(make_shared<OrGate>(shared_from_this(), currentFactor->shared_from_this()));
			for(list<Link*>::iterator parentLink(outputs.begin()); (*parentLink)->getOutput()->shared_from_this()!=newGate; parentLink=outputs.begin())
				//setInput(newGate) mindig törli az aktuális parentLink-et outputs közül, ezért outputs.begin() a következõ
				//newGate az utolsó,ezért  (*parentLink)->getOutput()...!=newGate  az összes többi output bejárásáig teljesül
			{
				(*parentLink)->setInput(newGate);
				input1 = static_pointer_cast<OrGate>(input1)->extractFactor(currentFactor);
				inputs.front()->setInput(input1);
				input2 = static_pointer_cast<OrGate>(input2)->removeFactor(currentFactor);
				(*++inputs.begin())->setInput(input2);
			}
		}
		
		/*assert(!isFlagged(MULTI_INPUTS_SINGLE_OUTPUT));
		assert(!isFlagged(MULTI_INPUTS_DOUBLE_OUTPUT));
		assert(multiInputsWithSingleOutput.empty());
		assert(multiInputsWithDoubleOutput.empty());*/

		setFlag(FACTORIZED);
	}
	return changed;
}

Gate::GatePtr AndGate::extractFactor(Gate* factor)
//bár elõfordulhat, hogy 2 úton is eljut azonos kapuhoz, ez csak 1 kapunál fordul elõ, ezért nem lesz exponenciális a bejárt kapuk száma
{
	set<Gate*>::iterator factorInSet = getMultiInputsWithDoubleOutput().find(factor);
	if( factorInSet == getMultiInputsWithDoubleOutput().end() )
	{
		return shared_from_this();
	}

	if(outputs.size()>1)
	{
		multiInputsWithSingleOutput.insert(factor);
		GatePtr reduced(removeFactor(factor));
		GatePtr newGate(make_shared<AndGate>(reduced, factor->shared_from_this()));
		for(list<Link*>::iterator parentLink(outputs.begin()); parentLink!=outputs.end();)
		{
			if ((*parentLink)->getOutput()->shared_from_this() == newGate)
			{
				++parentLink;
			}
			else
			{
				(*parentLink++)->setInput(newGate);
			}
		}
		return reduced;
	}
	else
	{
		multiInputsWithDoubleOutput.erase(factorInSet);
		multiInputsWithSingleOutput.erase(factor);
	}
	
	GatePtr input1 (inputs.front()->getInput()->shared_from_this());
	GatePtr input2((*++inputs.begin())->getInput()->shared_from_this());
	if(input1 == factor->shared_from_this())
	{
		return input2;//->type == this->type ? static_pointer_cast<AndGate>(input2)->extractFactor(factor) : input2;	//TODO: átírni polimorfikusra
	}
	else if(input2 == factor->shared_from_this())
	{
		return input1;//->type == this->type ? static_pointer_cast<AndGate>(input1)->extractFactor(factor) : input1;	//TODO: átírni polimorfikusra
	}
	else
	{
		if(input1->type == this->type)	//TODO: átírni polimorfikusra
		{
			GatePtr retVal( static_pointer_cast<AndGate>(input1)->extractFactor(factor) );
			if(retVal != input1)	//TODO: tesztelni nélküle
			{
				inputs.front()->setInput(retVal);
			}
		}
		if(input2->type == this->type)	//TODO: átírni polimorfikusra
		{
			GatePtr retVal( static_pointer_cast<AndGate>(input2)->extractFactor(factor) );
			if(retVal != input2)	//TODO: tesztelni nélküle
			{
				(*++inputs.begin())->setInput(retVal);
			}
		}
	}
	return shared_from_this();
}

/*void AndGate::simplifyRecursively2()
{
	if(!isFlagged(MARKED))
	{
		GatePtr constTrue = make_shared<TrueGate>();
		for each(Gate* input1 in getMultiInputs())
		{
			input1->simplifyRecursively2();
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			if(input1->type != TRUE_GATE)
			{
				bool removeOriginal(false);
				for each(Link* input2 in inputs)
				{
					if(input1->shared_from_this() == input2->getInput())
					{
						//TODO: tesztelni enélkül? (optimizeIteratively() általánosabban megoldja ezt)
						//ha többször elõfordul ugyanaz az input, csak az ismétléseket töröljük
						if(removeOriginal)
						{
							input2->setInput(constTrue);
						}
						else
						{
							removeOriginal = true;
						}
					}
					else if(input2->getInput()->type == this->type)
					{
						removeOriginal = input2->getInput()->replaceInputRecursively2(input1, constTrue, removeOriginal);
					}
					else
					{
						input2->getInput()->replaceInputRecursively2(input1, constTrue, true);
					}
				}
			}
			resetFlagRecursive(MARKED_PARENTS);
		}
		setFlag(MARKED);
	}
}*/

void AndGate::simplifyRecursively2()
{
	if(!isFlagged(MARKED))
	{
		inputs.front()->getInput()->simplifyRecursively2();
		(*++inputs.begin())->getInput()->simplifyRecursively2();

		GatePtr constTrue = make_shared<TrueGate>();
		GatePtr constFalse = make_shared<FalseGate>();
#ifdef VS2010
		for each(pair<GatePtr,Link*> i in getMultiInputsWithLink())
#else
		for(pair<GatePtr,Link*> i : getMultiInputsWithLink())
#endif
		{
			Gate* input1 = i.first.get();	//TODO: normálisabb nevet adni
			Link* protectedLink = i.second;
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			//if(input1->type != TRUE_GATE)	//nem kell, mert getMultiInputsWithLink()-ben nincsenek konstansok
			{
				GatePtr replacement(constTrue);

				list<GatePtr> separatedGates;		//TODO: egy szinttel fentebb
#ifdef VS2010
				for each(Link* input2 in inputs)
#else
				for(Link* input2 : inputs)
#endif
				{
					if(input1->shared_from_this() == input2->getInput())
					{
						//TODO: tesztelni enélkül? (optimizeIteratively() általánosabban megoldja ezt)
						//ha többször elõfordul ugyanaz az input, csak az ismétléseket töröljük
						if(input2 != protectedLink)
						{
							input2->setInput(replacement);
						}
					}
					else
					{
						input2->setInput( input2->getInput()->replaceInputRecursively2(input1, replacement, protectedLink, multiInputsWithLink, separatedGates) );
					}
				}
				setFlag(MARKED_PARENTS);
				resetFlagRecursive(TWIN | MARKED_PARENTS);	//TODO: elõre hívni, akkor eggyel kevesebbszer kell vagy megnézni egy szinttel kintebb!!!
#ifdef VS2010
				for each(GatePtr gate in separatedGates)
#else
				for(GatePtr gate : separatedGates)
#endif
				{
					gate->resetFlagRecursive(TWIN);
				}
				
				if(input1->type == NOT)		//TODO: refactor (ez majdnem pontosan az eddigiek megismétlése)
				{
					protectedLink = input1->inputs.front();
					input1 = protectedLink->getInput().get();
					replacement = constFalse;
#ifdef VS2010
					for each(Link* input2 in inputs)
#else
					for(Link* input2 : inputs)
#endif
					{
						if(input1->shared_from_this() == input2->getInput())
						{
							//TODO: tesztelni enélkül? (optimizeIteratively() általánosabban megoldja ezt)
							//ha többször elõfordul ugyanaz az input, csak az ismétléseket töröljük
							if(input2 != protectedLink)
							{
								input2->setInput(replacement);
							}
						}
						else
						{
							input2->setInput( input2->getInput()->replaceInputRecursively2(input1, replacement, protectedLink, multiInputsWithLink, separatedGates) );
						}
					}
					setFlag(MARKED_PARENTS);
					resetFlagRecursive(TWIN | MARKED_PARENTS);	//TODO: elõre hívni, akkor eggyel kevesebbszer kell vagy megnézni egy szinttel kintebb!!!
#ifdef VS2010
					for each(GatePtr gate in separatedGates)
#else
					for(GatePtr gate : separatedGates)
#endif
					{
						gate->resetFlagRecursive(TWIN);
					}
				}
			}
		}
		setFlag(MARKED);
	}
}

Gate::GatePtr AndGate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)
{
	//TODO: simplifyRecursively2()-ben rekurziót elõrébb hozni és ezt törölni
	if(outputs.size()==1)	//enélkül törlõdnének olyan elemek, amik szerepelnek multiInputsWithLink-ben
	{
		setTwinWeak(shared_from_this());
		return Gate::replaceInputRecursively2(original, newGate, protectedLink, sourceInputs, separatedGates);
	}

	if(!isFlagged(TWIN))
	{
		setTwinWeak(shared_from_this());
		Link* firstLink = inputs.front();
		Link* secondLink = *++inputs.begin();
		if(firstLink==protectedLink || secondLink==protectedLink)
		{
			return shared_from_this();
		}
		else if(firstLink->getInput() == original->shared_from_this())
		{
			if(newGate->type == FALSE_GATE)
			{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<FalseGate>());	//TODO: make_shared helyett newGate
			}
			else if(newGate->type == TRUE_GATE)
			{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(secondLink->getInput());		//TODO: rekurziót erre is
			}
			/*else
			{
				setTwinWeak(shared_from_this());
			}*/
		}
		else if(firstLink->getInput()->type==NOT && firstLink->getInput()->inputs.front()!=protectedLink)
		{
			//GatePtr grandson(firstLink->getInput()->inputs.front()->getInput());
			if(firstLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == TRUE_GATE)
				{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<FalseGate>());
				}
				else if(newGate->type == FALSE_GATE)
				{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(secondLink->getInput());		//TODO: rekurziót erre is
				}
				/*else
				{
					setTwinWeak(shared_from_this());
				}*/
			}
		}

		if(getTwinSafe()!=shared_from_this()) {return getTwinSafe();}
		//különben a másik inputra is ugyanezeket vizsgáljuk:

		if(secondLink->getInput() == original->shared_from_this())
		{
			if(newGate->type == FALSE_GATE)
			{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<FalseGate>());	//TODO: make_shared helyett newGate
			}
			else if(newGate->type == TRUE_GATE)
			{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(firstLink->getInput());		//TODO: rekurziót erre is
			}
			/*else
			{
				setTwinWeak(shared_from_this());
			}*/
		}
		else if(secondLink->getInput()->type == NOT && secondLink->getInput()->inputs.front()!=protectedLink)
		{
			//GatePtr grandson(secondLink->getInput()->inputs.front()->getInput());
			if(secondLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == TRUE_GATE)
				{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<FalseGate>());
				}
				else if(newGate->type == FALSE_GATE)
				{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(firstLink->getInput());//TODO: rekurziót erre is
				}
				/*else
				{
					setTwinWeak(shared_from_this());
				}*/
			}
		}
	}

	return getTwinSafe()==shared_from_this() ? Gate::replaceInputRecursively2(original, newGate, protectedLink, sourceInputs, separatedGates) : getTwinSafe();
}

void AndGate::simplifyRecursively3()
{
	if(!isFlagged(MARKED))
	{
		//ha egyetlen outputja azonos típusú, akkor a fentebbi simplify hívás bõvebb volt mint ez lenne
		if(outputs.size()==1 && outputs.front()->getOutput()->type==this->type)
		{
			setFlag(MARKED);
			inputs.front()->getInput()->simplifyRecursively3();
			(*++inputs.begin())->getInput()->simplifyRecursively3();
			return;
		}

		GatePtr constTrue = make_shared<TrueGate>();
		GatePtr constFalse = make_shared<FalseGate>();

		/*getMultiInputsWithLink();
		inputs.front()->getInput()->resetFlagRecursive(MULTI_INPUTS_LINK);
		(*++inputs.begin())->getInput()->resetFlagRecursive(MULTI_INPUTS_LINK);*/

#ifdef VS2010
		for each(pair<GatePtr,Link*> i in getMultiInputsWithLink())
#else
		for(pair<GatePtr,Link*> i : getMultiInputsWithLink())
#endif
		{
			Gate* input1 = i.first.get();	//TODO: normálisabb nevet adni
			Link* protectedLink = i.second;
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			//if(input1->type != TRUE_GATE)	//nem kell, mert getMultiInputsWithLink()-ben nincsenek konstansok
			
			GatePtr replacement(constTrue);

			list<GatePtr> separatedGates;		//TODO: egy szinttel fentebb
#ifdef VS2010
			for each(Link* input2 in inputs)
#else
			for(Link* input2 : inputs)
#endif
			{
				if(input1->shared_from_this() == input2->getInput())
				{
					//TODO: tesztelni enélkül? (optimizeIteratively() általánosabban megoldja ezt)
					//ha többször elõfordul ugyanaz az input, csak az ismétléseket töröljük
					if(input2 != protectedLink)
					{
						input2->setInput(replacement);
					}
				}
				else
				{
					input2->setInput( input2->getInput()->replaceInputRecursively3(input1, replacement, protectedLink, NULL, multiInputsWithLink, separatedGates) );
				}
			}

			setFlag(MARKED_PARENTS);
			removeConstGatesRecursivelyIfFlagged(TWIN | MARKED_PARENTS);
#ifdef VS2010
			for each(GatePtr gate in separatedGates)
#else
			for(GatePtr gate : separatedGates)
#endif
			{
				//gate->resetFlagRecursive(TWIN);	//TODO összehasonlítani
				gate->removeConstGatesRecursivelyIfFlagged(TWIN | MARKED_PARENTS);
			}
				
			if(input1->type == NOT)		//TODO: refactor (ez majdnem pontosan az eddigiek megismétlése)
			{
				Link* prevProtectedLink = protectedLink;
				protectedLink = input1->inputs.front();
				input1 = protectedLink->getInput().get();
				replacement = constFalse;
#ifdef VS2010
				for each(Link* input2 in inputs)
#else
				for(Link* input2 : inputs)
#endif
				{
					if(input1->shared_from_this() == input2->getInput())
					{
						//TODO: tesztelni enélkül? (optimizeIteratively() általánosabban megoldja ezt)
						//ha többször elõfordul ugyanaz az input, csak az ismétléseket töröljük
						if(input2 != protectedLink)
						{
							input2->setInput(replacement);
						}
					}
					else
					{
						input2->setInput( input2->getInput()->replaceInputRecursively3(input1, replacement, protectedLink, prevProtectedLink, multiInputsWithLink, separatedGates) );
					}
				}

				setFlag(MARKED_PARENTS);
				removeConstGatesRecursivelyIfFlagged(TWIN | MARKED_PARENTS);
#ifdef VS2010
				for each(GatePtr gate in separatedGates)
#else
				for(GatePtr gate : separatedGates)
#endif
				{
					//gate->resetFlagRecursive(TWIN);	//TODO összehasonlítani
					gate->removeConstGatesRecursivelyIfFlagged(TWIN | MARKED_PARENTS);
				}
			}
		}
		//resetFlagRecursive(MULTI_INPUTS_LINK);
		setFlag(MARKED);
		resetFlag(MULTI_INPUTS_LINK);
		inputs.front()->getInput()->simplifyRecursively3();
		(*++inputs.begin())->getInput()->simplifyRecursively3();
	}
}

Gate::GatePtr AndGate::replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)
{
	//TODO: simplifyRecursively2()-ben rekurziót elõrébb hozni és ezt törölni
	if(outputs.size()==1)	//enélkül törlõdnének olyan elemek, amik szerepelnek multiInputsWithLink-ben
	{
		setTwinWeak(shared_from_this());
		return Gate::replaceInputRecursively3(original, newGate, protectedLink1, protectedLink2, sourceInputs, separatedGates);
	}

	if(!isFlagged(TWIN))
	{
resetFlag(MULTI_INPUTS_LINK);
		setTwinWeak(shared_from_this());
		Link* firstLink = inputs.front();
		Link* secondLink = *++inputs.begin();
		if(firstLink==protectedLink1 || secondLink==protectedLink1 || firstLink==protectedLink2 || secondLink==protectedLink2)
		{
			return shared_from_this();
		}
		else if(firstLink->getInput() == original->shared_from_this())
		{
			if(newGate->type == FALSE_GATE)
			{
			separatedGates.push_back(shared_from_this());
			removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
			removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<FalseGate>());	//TODO: make_shared helyett newGate
			}
			else if(newGate->type == TRUE_GATE)
			{
			separatedGates.push_back(shared_from_this());
			removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(secondLink->getInput());		//TODO: rekurziót erre is
			}
			/*else
			{
				setTwinWeak(shared_from_this());
			}*/
		}
		else if(firstLink->getInput()->type==NOT && firstLink->getInput()->inputs.front()!=protectedLink1 && firstLink->getInput()->inputs.front()!=protectedLink2)
		{
			//GatePtr grandson(firstLink->getInput()->inputs.front()->getInput());
			if(firstLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == TRUE_GATE)
				{
				separatedGates.push_back(shared_from_this());
				removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
				removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<FalseGate>());
				}
				else if(newGate->type == FALSE_GATE)
				{
				separatedGates.push_back(shared_from_this());
				removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(secondLink->getInput());		//TODO: rekurziót erre is
				}
				/*else
				{
					setTwinWeak(shared_from_this());
				}*/
			}
		}

		if(getTwinSafe()!=shared_from_this()) {return getTwinSafe();}
		//különben a másik inputra is ugyanezeket vizsgáljuk:

		if(secondLink->getInput() == original->shared_from_this())
		{
			if(newGate->type == FALSE_GATE)
			{
			separatedGates.push_back(shared_from_this());
			removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
			removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<FalseGate>());	//TODO: make_shared helyett newGate
			}
			else if(newGate->type == TRUE_GATE)
			{
			separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
			removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(firstLink->getInput());		//TODO: rekurziót erre is
			}
			/*else
			{
				setTwinWeak(shared_from_this());
			}*/
		}
		else if(secondLink->getInput()->type == NOT && secondLink->getInput()->inputs.front()!=protectedLink1 && secondLink->getInput()->inputs.front()!=protectedLink2)
		{
			//GatePtr grandson(secondLink->getInput()->inputs.front()->getInput());
			if(secondLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == TRUE_GATE)
				{
				separatedGates.push_back(shared_from_this());
				removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
				removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<FalseGate>());
				}
				else if(newGate->type == FALSE_GATE)
				{
				separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
				removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(firstLink->getInput());		//TODO: rekurziót erre is
				}
				/*else
				{
					setTwinWeak(shared_from_this());
				}*/
			}
		}
	}

	return getTwinSafe()==shared_from_this() ? Gate::replaceInputRecursively3(original, newGate, protectedLink1, protectedLink2, sourceInputs, separatedGates) : getTwinSafe();
}

abc::Abc_Obj_t* AndGate::getAbcNode(abc::Abc_Ntk_t* pNetwork)
{
	if(!isFlagged(ABC_NODE))
	{
		setFlag(ABC_NODE);
		list<Link*>::iterator inp1(inputs.begin());
		list<Link*>::iterator inp2(inp1);
		++inp2;
		pAbcNode = abc::Abc_AigAnd( (abc::Abc_Aig_t*)(pNetwork->pManFunc), (*inp1)->getInput()->getAbcNode(pNetwork), (*inp2)->getInput()->getAbcNode(pNetwork) );
	}
	return pAbcNode;
}

Gate::gateTypes AndGate::getIndirectType()
{
	gateTypes inpType1 = inputs.front()->getInput()->type;
	gateTypes inpType2 = (*++inputs.begin())->getInput()->type;
	if(inpType1==TRUE_GATE && inpType2==TRUE_GATE)
	{
		return TRUE_GATE;
	}
	else if(inpType1==FALSE_GATE || inpType2==FALSE_GATE)
	{
		return FALSE_GATE;
	}
	return this->type;
}