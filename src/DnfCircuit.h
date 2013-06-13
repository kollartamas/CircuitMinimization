#ifndef DNF_CIRCUIT_H
#define DNF_CIRCUIT_H

#include "Circuit.h"
#include "Quine_McClusky\Implicant.h"

#include <cstdio>

class DnfCircuit : public Circuit
{
private:
	Gate::GatePtr buildFromImplicantList(std::list<Implicant>& implicantList);
	Gate::GatePtr buildFromImplicant(Implicant& implicant);
	std::list<Implicant> getMintermList();
	std::list<Implicant> getImplicantList();

	void listClausesInSubgraph(Gate& rootGate, std::list<Gate*>& clauseList);
	Implicant getImplicantFromSubgraph(Gate& rootGate);
	void collectInputsFromSubgraph(Gate& rootGate, std::vector<int>& currentValues);
public:
	DnfCircuit(std::list<Implicant>& implicantList);
	DnfCircuit(unsigned int variables, unsigned int clauses);
	DnfCircuit(DnfCircuit& other);
	DnfCircuit(Circuit& other);

	FILE* toPlaFormat();
	void readPlaFormat(FILE* plaFile);
	
	void minimizeEspresso();
	void minimizeQuineMcClusky();
};

#endif