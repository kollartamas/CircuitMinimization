#ifndef PRIME_CHART_H
#define PRIME_CHART_H

#include "Implicant.h"

#include <vector>
#include <list>


class PrimeChart
{
private:
	std::vector<std::list<Implicant*>> coveringImplicants;
	std::vector<bool> covered;
	std::list<Implicant*> getEssentialPrimes();
	void buildSubtreeFromPosition(std::list<Implicant*>& currentDNF, unsigned int position);

	std::list<Implicant*> bestDNF;
	unsigned int bestNumOfTerms;
	unsigned int bestNumOfLiterals;
	bool foundMinimal;
	
public:
	PrimeChart(unsigned int numOfMinterms);
	void addToChart(unsigned int coveredMinterm, Implicant* implicant);
	std::list<Implicant*> getMinimalDNF();
};



#endif