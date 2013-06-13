#include "ImplicantChart.h"
#include "Implicant.h"
#include "PrimeChart.h"

#include <list>

using namespace std;

ImplicantChart::ImplicantChart():
	numOfMinterms(0)
	//sizeBrackets()
{
	std::set<Implicant> mintermBracket;
	sizeBrackets.push_back(mintermBracket);
}

void ImplicantChart::addMinterm(Implicant& minterm, bool required)
{
	if(!minterm.valid)
	{
		return;
	}

	//ismétlõdéseket kiszûrjük:
	for each(Implicant existing in sizeBrackets.front())
	{
		if(minterm == existing)
		{
			return;
		}
	}

	if(required)
	{
		minterm.mintermIDs.push_back(numOfMinterms++);
	}
	sizeBrackets.front().insert(minterm);
}

void ImplicantChart::mergeImplicantsInBracket(set<Implicant>& bracket, set<Implicant>& nextBracket)
{
	for(set<Implicant>::iterator iter1 = bracket.begin(); iter1!=bracket.end(); ++iter1)
	{
		for(set<Implicant>::iterator iter2 = bracket.begin(); iter2!=bracket.end(); ++iter2)
		{
			if (iter1==iter2){continue;}

			Implicant result;
			if(result.merge(*iter1, *iter2))
			{
				iter1->marked = true;
				iter2->marked = true;
				nextBracket.insert(result);
			}

		}
	}
}

void ImplicantChart::mergeAllImplicants()
{
	for(list<set<Implicant>>::iterator currentBracket = sizeBrackets.begin(); currentBracket->size()>1; ++currentBracket)
	{
		sizeBrackets.push_back(set<Implicant>());
		mergeImplicantsInBracket(*currentBracket, *next(currentBracket));
	}
	if(sizeBrackets.back().size()==0){sizeBrackets.pop_back();}
}

list<Implicant> ImplicantChart::getMinimizedDNF()
{
	list<Implicant> result;

	PrimeChart primeChart(this->numOfMinterms);
	mergeAllImplicants();
	for(list<set<Implicant>>::reverse_iterator currentBracket=sizeBrackets.rbegin(); currentBracket!=sizeBrackets.rend(); ++currentBracket)
	{
		for(set<Implicant>::iterator implicant = currentBracket->begin(); implicant!=currentBracket->end(); ++implicant)
		{
			if (!implicant->marked)
			{
				for each(unsigned int id in implicant->mintermIDs)
				{
					primeChart.addToChart(id, &(*implicant));
				}
			}
		}
	}
	for each(const Implicant* p_implicant in primeChart.getMinimalDNF())
	{
		result.push_back(*p_implicant);
	}
	return result;
}

/*unsigned int ImplicantChart::countUnmarkedImplicants()
{
	unsigned int result = 0;
	for(list<list<Implicant>>::iterator currentBracket=sizeBrackets.begin(); currentBracket!=sizeBrackets.end(); ++currentBracket)
	{
		for(list<Implicant>::iterator implicant = currentBracket->begin(); implicant!=currentBracket->end(); ++implicant)
		{
			if (!implicant->marked){result++;}
		}
	}
	return result;
}*/