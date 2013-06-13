#ifndef IMPLICANT_CHART_H
#define IMPLICANT_CHART_H

#include <list>
#include <set>
#include "Implicant.h"

//typedef std::list<Implicant> Bracket;

class ImplicantChart
{
private:
	std::list< std::set<Implicant> > sizeBrackets;
	unsigned int numOfMinterms;

	void mergeImplicantsInBracket( std::set<Implicant>& bracket, std::set<Implicant>& nextBracket);
	void mergeAllImplicants();
	//unsigned int countUnmarkedImplicants();

public:
	ImplicantChart();
	void addMinterm(Implicant& minterm, bool required);
	std::list<Implicant> getMinimizedDNF();
};


#endif