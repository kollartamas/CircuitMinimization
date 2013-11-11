#include "PrimeChart.h"
#include "Implicant.h"

#include <list>
#include <cassert>
#include <climits>
//#include <exception>
//#include <iostream>

using namespace std;

PrimeChart::PrimeChart(unsigned int numOfMinterms):
	coveringImplicants(numOfMinterms, list<const Implicant*>()),
	covered(numOfMinterms, false),
	bestNumOfTerms(UINT_MAX),
	bestNumOfLiterals(UINT_MAX),
	foundMinimal(false)
{}

void PrimeChart::addToChart(unsigned int coveredMinterm, const Implicant* implicant)
{
	coveringImplicants[coveredMinterm].push_back(implicant);
}

list<const Implicant*> PrimeChart::getEssentialPrimes()
{
	list<const Implicant*> essentialPrimes;

	for(unsigned int i=0; i<coveringImplicants.size(); i++)
	{
		//ha egy mintermet csak egyetlen implicant fed le:
		if(!covered[i] && coveringImplicants[i].size()==1)
		{
			const Implicant* implicant = coveringImplicants[i].front();
			essentialPrimes.push_back(implicant);
#ifdef VS2010
			for each(unsigned int coveredID in implicant->mintermIDs)
#else
			for(unsigned int coveredID : implicant->mintermIDs)
#endif
			{
				covered[coveredID] = true;
			}
		}
	}
	return essentialPrimes;
}

list<const Implicant*> PrimeChart::getMinimalDNF()
{
	if(!foundMinimal)
	{
		list<const Implicant*> essentialPrimes(getEssentialPrimes());	//move constructor
		buildSubtreeFromPosition(essentialPrimes, 0);
		foundMinimal = true;
	}
	return bestDNF;
}

void PrimeChart::buildSubtreeFromPosition(list<const Implicant*>& currentDNF, unsigned int position)
{
	for(unsigned int i=position; i<coveringImplicants.size(); i++)
	{
		if(!covered[i])
		{
			assert(!coveringImplicants[i].empty());

#ifdef VS2010
			for each(const Implicant* implicant in coveringImplicants[i])
#else
			for(const Implicant* implicant : coveringImplicants[i])
#endif
			{
				currentDNF.push_back(implicant);
				//beállítjuk a megfelelõ mintermeket lefedettre, és feljegyezzük a backtracking miatt
				list<int> setToCovered;
#ifdef VS2010
				for each(unsigned int coveredID in implicant->mintermIDs)
#else
				for(unsigned int coveredID : implicant->mintermIDs)
#endif
				{
					if(!covered[coveredID])
					{
						covered[coveredID] = true;
						setToCovered.push_back(coveredID);
					}
				}

				//rekurzívan végrehajtjuk az aktuális ágon
				//de csak ha az aktuális részfában még elõfordulhat rövidebb DNF:
				if(currentDNF.size()<=bestNumOfTerms)	//TODO: literálokra is ellenõrizni?
				{
					buildSubtreeFromPosition(currentDNF, i+1);
				}

				//visszaállítjuk a mintermek fedettségét az eredetire
#ifdef VS2010
				for each(unsigned int coveredID in setToCovered)
#else
				for(unsigned int coveredID : setToCovered)
#endif
				{
					covered[coveredID] = false;
				}

				currentDNF.pop_back();
			}
			return;
		}
	}

	//ha ide jutunk, minden szükséges Minterm le van fedve, megfelelõ DNF-et kaptunk
	//megvizsgáljuk jobb-e a korábbiaknál:
	if(currentDNF.size()<bestNumOfTerms)	//TODO: bestNumOfLiterals összehasonlítás itt és az érték frissítése késõbb
	{
		bestNumOfTerms = currentDNF.size();
		bestDNF = currentDNF;
	}

}