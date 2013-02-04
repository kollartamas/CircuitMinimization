#ifndef IMPLICANT_CHART_H
#define IMPLICANT_CHART_H

#include <list>
#include "Implicant.h"

//typedef std::list<Implicant> Bracket;

class ImplicantChart
{
private:
	std::list< std::list<Implicant> > sizeBrackets;
	unsigned int numOfMinterms;

	void mergeImplicantsInBracket( std::list<Implicant>& bracket, std::list<Implicant>& nextBracket);
	void mergeAllImplicants();
	//unsigned int countUnmarkedImplicants();

public:
	ImplicantChart();
	void addMinterm(Implicant& minterm, bool required);
	std::list<Implicant> getMinimizedDNF();
};


#endif