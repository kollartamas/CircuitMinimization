#ifndef NOTGATE_H
#define NOTGATE_H

#include "Gate.h"

class NotGate : public Gate
{
public:
	NotGate(GatePtr in);
	~NotGate();
	/*void deleteRecursively();*/

	bool getValue();

	unsigned int getSizeAbcStyle();

	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	unsigned int getInfixLength();
	unsigned int getPrefixLength();
	
	const std::set<Implicant>& getDnf(unsigned int numOfVariables);

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
	GatePtr getConstantFreeTwin();
	abc::Abc_Obj_t* getAbcNode(abc::Abc_Ntk_t* network);
	gateTypes getIndirectType();

	void removeNotGate();
	GatePtr replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	GatePtr replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	bool factorizeRecursively();	//return: volt-e változás az adott részfában
};

#endif