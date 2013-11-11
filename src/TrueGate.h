#ifndef TRUEGATE_H
#define TRUEGATE_H

#include "Gate.h"

class TrueGate : public Gate
{
private:

public:
	TrueGate();
	~TrueGate(){}
	/*void deleteRecursively();*/
	bool getValue();

	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);
	unsigned int getInfixLength();
	unsigned int getPrefixLength();
	
	const std::set<Implicant>& getDnf(unsigned int numOfVariables);

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
	abc::Abc_Obj_t* getAbcNode(abc::Abc_Ntk_t* network);

	void replaceInputRecursively(GatePtr original, GatePtr newGate);
	GatePtr replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	GatePtr replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
};

#endif