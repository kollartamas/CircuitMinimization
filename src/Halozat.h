#ifndef HALOZAT_H
#define HALOZAT_H

#include <vector>
#include <string>
#include <functional>

#include "Input.h"
#include "Kapu.h"

#include "Quine_McClusky\ImplicantChart.h"

class Halozat
{
private:
	std::vector<Input*> inputok;
	Kapu* output;
	void doForEachGate(std::function<void(Kapu*)> funct);	//minden kapura hívja a megadott fuggvenyt
	void doForEachGateReverse(std::function<void(Kapu*)> funct);	//az outputtól az inputok felé

	unsigned int calculateInfixLength();
	unsigned int calculatePrefixLength();
	unsigned int calculatePostfixLength();

	void listClausesInSubgraph(Kapu& rootGate, std::list<Kapu*>& clauseList);
	Implicant getImplicantFromSubgraph(Kapu& rootGate);
	void collectInputsFromSubgraph(Kapu& rootGate, std::vector<int>& currentValues);
	std::list<Implicant> getImplicantList();
	std::list<Implicant> getMintermList();

	Kapu* buildFromImplicantList(std::list<Implicant>& implicantList);
	Kapu* buildFromImplicant(Implicant& implicant);

public:
	Halozat(unsigned int inputokSzama);
	Halozat(unsigned int inputokSzama, unsigned int kapukSzama);
	~Halozat();

	Input& operator[](unsigned int sorszam);
	void setOutput(Kapu* out);
	
	bool kiertekel();

	std::string toStringInfix();
	std::string toStringPrefix();
	std::string toStringPostfix();

	void removeNotGates();
	void removeExtraGates();

	void minimizeQuineMcClusky();



};

#endif