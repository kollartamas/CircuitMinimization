#ifndef CIRCUIT_H
#define CIRCUIT_H

#include "Gate.h"
#include "Input.h"
#include "MultiGate.h"
#include "abc.h"

#include <cstdio>

class Circuit
{
public://protected:
	std::vector<Input::InputPtr> inputs;
	Gate::GatePtr output;

	/*Implicant getImplicantFromSubgraph(Kapu& rootGate);
	void collectInputsFromSubgraph(Kapu& rootGate, std::vector<int>& currentValues);
	std::list<Implicant> getImplicantList();
	std::list<Implicant> getMintermList();

	Kapu* buildFromImplicantList(std::list<Implicant>& implicantList);
	Kapu* buildFromImplicant(Implicant& implicant);*/
	
	void removeNotGates();
	void removeMultiGates();

public:
	Circuit(unsigned int inputokSzama);
	Circuit(unsigned int inputokSzama, unsigned int kapukSzama, bool includeConstants);
	Circuit(Circuit& other);
	~Circuit();

	bool operator == (Circuit& other);

	Input& operator[](unsigned int id);
	void setOutput(Gate::GatePtr out);
	bool getOutputValue();
	unsigned int getSize();
	unsigned int getLevel();
	unsigned int getSizeAbcStyle();
	
	std::string toStringInfix();
	std::string toStringPrefix();
	std::string toStringPostfix();
	void printStructure();
	
	Gate::GatePtr buildFromDnf(const std::set<Implicant>& dnf);
	Gate::GatePtr buildFromImplicant(const Implicant& implicant);

	void optimizeAbc();
};

#endif