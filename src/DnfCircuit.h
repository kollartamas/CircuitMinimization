#ifndef DNF_CIRCUIT_H
#define DNF_CIRCUIT_H

#include "Circuit.h"
#include "CircuitWithoutNotGate.h"
#include "Implicant.h"
#include "PlaFormatException.h"

#include <cstdio>

class DnfCircuit : public CircuitWithoutNotGate
{
private:
	Gate::GatePtr buildFromImplicantList(const std::list<Implicant>& implicantList);
	Gate::GatePtr buildFromImplicant(const Implicant& implicant);
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
	DnfCircuit(CircuitWithoutNotGate& other);

	FILE* toPlaFormat() throw (PlaFormatException);
	void readPlaFormat(FILE* plaFile);
	
	void minimizeEspresso();
	void minimizeQuineMcClusky();
};

#endif
