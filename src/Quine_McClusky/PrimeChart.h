#ifndef PRIME_CHART_H
#define PRIME_CHART_H

#include "Implicant.h"

#include <vector>
#include <list>


class PrimeChart
{
private:
	std::vector<std::list<const Implicant*>> coveringImplicants;
	std::vector<bool> covered;
	std::list<const Implicant*> getEssentialPrimes();
	void buildSubtreeFromPosition(std::list<const Implicant*>& currentDNF, unsigned int position);

	std::list<const Implicant*> bestDNF;
	unsigned int bestNumOfTerms;
	unsigned int bestNumOfLiterals;
	bool foundMinimal;
	
public:
	PrimeChart(unsigned int numOfMinterms);
	void addToChart(unsigned int coveredMinterm, const Implicant* implicant);
	std::list<const Implicant*> getMinimalDNF();
};



#endif