#include "PrimeChart.h"
#include "Implicant.h"

#include <list>
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
			for each(unsigned int coveredID in implicant->mintermIDs)
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
		buildSubtreeFromPosition(getEssentialPrimes(), 0);
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
			//ez az eset nem fordulhat elõ helyes adatokkal:
			if(coveringImplicants[i].size()==0)
			{
				throw exception("Hiba: nem talaltam megfelelo DNF-et");
			}

			for each(const Implicant* implicant in coveringImplicants[i])
			{
				currentDNF.push_back(implicant);
				//beállítjuk a megfelelõ mintermeket lefedettre, és feljegyezzük a backtracking miatt
				list<int> setToCovered;
				for each(unsigned int coveredID in implicant->mintermIDs)
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
				for each(unsigned int coveredID in setToCovered)
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