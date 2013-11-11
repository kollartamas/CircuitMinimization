#ifndef ANDGATE_H
#define ANDGATE_H

#include "Gate.h"
#include "BivariateGate.h"

#include <list>
#include <map>

class AndGate : public BivariateGate
{
public:
	AndGate(GatePtr in1, GatePtr in2);
	~AndGate(){}

	bool getValue();
	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);

	const std::set<Implicant>& getDnf(unsigned int numOfVariables);

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
	GatePtr getMultiTwin();
	GatePtr getSingleTwin();
	GatePtr getConstantFreeTwin();
	abc::Abc_Obj_t* getAbcNode(abc::Abc_Ntk_t* network);
	gateTypes getIndirectType();
	
	GatePtr replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	GatePtr replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	void simplifyRecursively2();
	void simplifyRecursively3();
	bool factorizeRecursively();
	GatePtr extractFactor(Gate* factor);

	//std::list<std::list<Gate*>>& getDnf();
	/*GatePtr getDnf();
	static GatePtr conjunctionOfDnfs(GatePtr dnf1, GatePtr dnf2);
	static GatePtr conjunctionOfClauseAndDnf(GatePtr dnf1, GatePtr dnf2);*/

};

#endif