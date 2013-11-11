#include "OrGate.h"
#include "BivariateGate.h"
#include "Gate.h"
#include "AndGate.h"
#include "Link.h"
#include "MultiOr.h"
#include "FalseGate.h"
#include "TrueGate.h"

#include <algorithm>
#include <iterator>
#include <cassert>

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

const set<Implicant>& OrGate::getDnf(unsigned int numOfVariables)
{
	if(!isFlagged(NORMAL_FORM))
	{
		setFlag(NORMAL_FORM);
		normalForm.clear();
		const set<Implicant>& dnf1(inputs.front()->getInput()->getDnf(numOfVariables));
		const set<Implicant>& dnf2((*++inputs.begin())->getInput()->getDnf(numOfVariables));
		set_union(dnf1.begin(), dnf1.end(), dnf2.begin(), dnf2.end(), inserter(normalForm, normalForm.end()));
	}
	return normalForm;
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

		if(inp1->type == TRUE_GATE || inp2->type == FALSE_GATE)
		{
			setTwinStrong(inp1);
		}
		else if(inp1->type == FALSE_GATE || inp2->type == TRUE_GATE)
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

/*Gate::GatePtr OrGate::getDnf()
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
}*/

bool OrGate::factorizeRecursively()
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

		if(input1->type!=AND || input2->type!=AND)
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
		set_intersection(input1->getMultiInputsWithSingleOutput().begin(), input1->getMultiInputsWithSingleOutput().end(), input2->getMultiInputsWithDoubleOutput().begin(), input2->getMultiInputsWithDoubleOutput().end(), back_inserter(factors));
		if(!factors.empty()){changed = true;}
		for(list<Gate*>::iterator iter(factors.begin()); iter!=factors.end(); ++iter)
		{
			Gate* currentFactor(*iter);		//TODO: átgondolni, nem szûnhet-e meg korábban a pointer által mutatott gate
			//a kiemelendõ kaput this és az outputjai közé kötjük egy OR kapuval
			GatePtr newGate(make_shared<AndGate>(shared_from_this(), currentFactor->shared_from_this()));
			for(list<Link*>::iterator parentLink(outputs.begin()); (*parentLink)->getOutput()->shared_from_this()!=newGate; parentLink=outputs.begin())
				//setInput(newGate) mindig törli az aktuális parentLink-et outputs közül, ezért outputs.begin() a következõ
				//newGate az utolsó,ezért  (*parentLink)->getOutput()...!=newGate  az összes többi output bejárásáig teljesül
			{
				(*parentLink)->setInput(newGate);
				input1 = static_pointer_cast<AndGate>(input1)->removeFactor(currentFactor);
				inputs.front()->setInput(input1);
				input2 = static_pointer_cast<AndGate>(input2)->extractFactor(currentFactor);
				(*++inputs.begin())->setInput(input2);
			}
		}

		if(input1->type!=AND || input2->type!=AND)	//mert az elõzõek eredményeképp változhattak inputok
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
			//a kiemelendõ kaput this és az outputjai közé kötjük egy AND kapuval
			GatePtr newGate(make_shared<AndGate>(shared_from_this(), currentFactor->shared_from_this()));
			for(list<Link*>::iterator parentLink(outputs.begin()); (*parentLink)->getOutput()->shared_from_this()!=newGate; parentLink=outputs.begin())
				//setInput(newGate) mindig törli az aktuális parentLink-et outputs közül, ezért outputs.begin() a következõ
				//newGate az utolsó,ezért  (*parentLink)->getOutput()...!=newGate  az összes többi output bejárásáig teljesül
			{
				(*parentLink)->setInput(newGate);
				input1 = static_pointer_cast<AndGate>(input1)->extractFactor(currentFactor);
				inputs.front()->setInput(input1);
				input2 = static_pointer_cast<AndGate>(input2)->removeFactor(currentFactor);
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

Gate::GatePtr OrGate::extractFactor(Gate* factor)
//bár elõfordulhat, hogy 2 úton is eljut azonos kapuhoz, ez csak 1 kapunál fordul elõ, ezért nem lesz exponenciális a bejárt kapuk száma
{
	set<Gate*>::iterator factorInSet = getMultiInputsWithDoubleOutput().find(factor);
	if( factorInSet == getMultiInputsWithDoubleOutput().end() )
	{
		return shared_from_this();
	}

	if(outputs.size()>1)
	{
		multiInputsWithSingleOutput.insert(factor);	//különben removeFactor nem csinál semmit
		GatePtr reduced(removeFactor(factor));
		GatePtr newGate(make_shared<OrGate>(reduced, factor->shared_from_this()));
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
		return input2;//->type == this->type ? static_pointer_cast<OrGate>(input2)->extractFactor(factor) : input2;	//TODO: átírni polimorfikusra
	}
	else if(input2 == factor->shared_from_this())
	{
		return input1;//->type == this->type ? static_pointer_cast<OrGate>(input1)->extractFactor(factor) : input1;	//TODO: átírni polimorfikusra
	}
	else
	{
		if(input1->type == this->type)	//TODO: átírni polimorfikusra
		{
			GatePtr retVal( static_pointer_cast<OrGate>(input1)->extractFactor(factor) );
			if(retVal != input1)	//TODO: tesztelni nélküle
			{
				inputs.front()->setInput(retVal);
			}
		}
		if(input2->type == this->type)	//TODO: átírni polimorfikusra
		{
			GatePtr retVal( static_pointer_cast<OrGate>(input2)->extractFactor(factor) );
			if(retVal != input2)	//TODO: tesztelni nélküle
			{
				(*++inputs.begin())->setInput(retVal);
			}
		}
	}
	return shared_from_this();
}

/*void OrGate::simplifyRecursively2()
{
	if(!isFlagged(MARKED))
	{
		GatePtr constFalse = make_shared<FalseGate>();
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
							input2->setInput(constFalse);
						}
						else
						{
							removeOriginal = true;
						}
					}
					else if(input2->getInput()->type == this->type)
					{
						removeOriginal = input2->getInput()->replaceInputRecursively2(input1, constFalse, removeOriginal);
					}
					else
					{
						input2->getInput()->replaceInputRecursively2(input1, constFalse, true);
					}
				}
			}
			resetFlagRecursive(MARKED_PARENTS);
		}
		setFlag(MARKED);
	}
}*/

void OrGate::simplifyRecursively2()
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
		for (pair<GatePtr,Link*> i : getMultiInputsWithLink())
#endif
		
		{
			Gate* input1 = i.first.get();
			Link* protectedLink = i.second;
			//input1->simplifyRecursively2();
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			//if(input1->type != TRUE_GATE)	//nem kell, mert getMultiInputsWithLink()-ben nincsenek konstansok
			{
				GatePtr replacement(constFalse);

				list<GatePtr> separatedGates;		//TODO: egy szinttel fentebb?
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
				resetFlagRecursive(TWIN | MARKED_PARENTS);	//TODO: elõre hívni, akkor eggyel kevesebbszer kell
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
					replacement = constTrue;
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
					resetFlagRecursive(TWIN | MARKED_PARENTS);	//TODO: elõre hívni, akkor eggyel kevesebbszer kell
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

Gate::GatePtr OrGate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)	//TODO: original Gate* vagy GatePtr legyen?
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
			if(newGate->type == TRUE_GATE)
			{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<TrueGate>());		//TODO: make_shared helyett newGate
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
		else if(firstLink->getInput()->type==NOT && firstLink->getInput()->inputs.front()!=protectedLink)
		{
			//GatePtr grandson(firstLink->getInput()->inputs.front()->getInput());
			if(firstLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == FALSE_GATE)
				{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<TrueGate>());
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
		}

		if(getTwinSafe()!=shared_from_this()) {return getTwinSafe();}
		//különben a másik inputra is ugyanezeket vizsgáljuk:

		if(secondLink->getInput() == original->shared_from_this())
		{
			if(newGate->type == TRUE_GATE)
			{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<TrueGate>());
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
		else if(secondLink->getInput()->type==NOT && secondLink->getInput()->inputs.front()!=protectedLink)	//TODO: kell-e a második feltétel?
		{
			//GatePtr grandson(secondLink->getInput()->inputs.front()->getInput());
			if(secondLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == FALSE_GATE && firstLink!=protectedLink && secondLink!=protectedLink)
				{
separatedGates.push_back(shared_from_this());
removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<TrueGate>());
				}
				else if(newGate->type == TRUE_GATE && firstLink!=protectedLink && secondLink!=protectedLink)
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

	return getTwinSafe()==shared_from_this() ? Gate::replaceInputRecursively2(original, newGate, protectedLink, sourceInputs, separatedGates) : getTwinSafe();
}

void OrGate::simplifyRecursively3()
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
		for (pair<GatePtr,Link*> i : getMultiInputsWithLink())
#endif
		
		{
			Gate* input1 = i.first.get();
			Link* protectedLink = i.second;
			//input1->simplifyRecursively2();
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			//if(input1->type != TRUE_GATE)	//nem kell, mert getMultiInputsWithLink()-ben nincsenek konstansok
			{
				GatePtr replacement(constFalse);

				list<GatePtr> separatedGates;		//TODO: egy szinttel fentebb?
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
					replacement = constTrue;
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
					removeConstGatesRecursivelyIfFlagged(TWIN | MARKED_PARENTS);	//TODO: elõre hívni, akkor eggyel kevesebbszer kell
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
		}
		setFlag(MARKED);
		resetFlag(MULTI_INPUTS_LINK);
		inputs.front()->getInput()->simplifyRecursively3();
		(*++inputs.begin())->getInput()->simplifyRecursively3();
	}
}

Gate::GatePtr OrGate::replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, map<GatePtr,Link*>& sourceInputs, list<GatePtr>& separatedGates)	//TODO: original Gate* vagy GatePtr legyen?
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
			if(newGate->type == TRUE_GATE)
			{
			separatedGates.push_back(shared_from_this());
			removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
			removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<TrueGate>());
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
		else if(firstLink->getInput()->type==NOT && firstLink->getInput()->inputs.front()!=protectedLink1&& firstLink->getInput()->inputs.front()!=protectedLink2)	//TODO: kell-e check protectedLink2-re?
		{
			//GatePtr grandson(firstLink->getInput()->inputs.front()->getInput());
			if(firstLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == FALSE_GATE)
				{
				separatedGates.push_back(shared_from_this());
				removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
				removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<TrueGate>());
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
		}

		if(getTwinSafe()!=shared_from_this()) {return getTwinSafe();}
		//különben a másik inputra is ugyanezeket vizsgáljuk:

		if(secondLink->getInput() == original->shared_from_this())
		{
			if(newGate->type == TRUE_GATE)
			{
			separatedGates.push_back(shared_from_this());
			removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
			removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
				setTwinStrong(make_shared<TrueGate>());
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
		else if(secondLink->getInput()->type==NOT && secondLink->getInput()->inputs.front()!=protectedLink1 && secondLink->getInput()->inputs.front()!=protectedLink2)	//TODO: kell-e az utolsó feltétel?
		{
			//GatePtr grandson(secondLink->getInput()->inputs.front()->getInput());
			if(secondLink->getInput()->inputs.front()->getInput() == original->shared_from_this())
			{
				if(newGate->type == FALSE_GATE)
				{
				separatedGates.push_back(shared_from_this());
				removeKeyAndValueFromMap(firstLink->getInput(), firstLink, sourceInputs);
				removeKeyAndValueFromMap(secondLink->getInput(), secondLink, sourceInputs);
					setTwinStrong(make_shared<TrueGate>());
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
		}
	}

	return getTwinSafe()==shared_from_this() ? Gate::replaceInputRecursively3(original, newGate, protectedLink1, protectedLink2, sourceInputs, separatedGates) : getTwinSafe();
}

abc::Abc_Obj_t* OrGate::getAbcNode(abc::Abc_Ntk_t* pNetwork)
{
	if(!isFlagged(ABC_NODE))
	{
		setFlag(ABC_NODE);
		list<Link*>::iterator inp1(inputs.begin());
		list<Link*>::iterator inp2(inp1);
		++inp2;
		pAbcNode = abc::Abc_AigOr( (abc::Abc_Aig_t*)(pNetwork->pManFunc), (*inp1)->getInput()->getAbcNode(pNetwork), (*inp2)->getInput()->getAbcNode(pNetwork) );
	}
	return pAbcNode;
}

Gate::gateTypes OrGate::getIndirectType()
{
	gateTypes inpType1 = inputs.front()->getInput()->type;
	gateTypes inpType2 = (*++inputs.begin())->getInput()->type;
	if(inpType1==TRUE_GATE || inpType2==TRUE_GATE)
	{
		return TRUE_GATE;
	}
	else if(inpType1==FALSE_GATE && inpType2==FALSE_GATE)
	{
		return FALSE_GATE;
	}
	return this->type;
}