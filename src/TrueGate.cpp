#include "TrueGate.h"
#include "FalseGate.h"

using namespace std;

TrueGate::TrueGate()
{
	this->type = TRUE_GATE;
	this->value = true;
}

/*void TrueGate::deleteRecursively()
{
	delete this;
}*/

bool TrueGate::getValue()
{
	return true;
}

unsigned int TrueGate::getInfixLength()
{
	return 1;
}

unsigned int TrueGate::getPrefixLength()
{
	return 2;
}

void TrueGate::addToStringInfix(std::string& dest)
{
	dest += 'I';
}

void TrueGate::addToStringPrefix(std::string& dest)
{
	dest += "I ";
}

void TrueGate::addToStringPostfix(std::string& dest)
{
	dest += "I ";
}

const set<Implicant>& TrueGate::getDnf(unsigned int numOfVariables)
{
	if(!isFlagged(NORMAL_FORM))
	{
		setFlag(NORMAL_FORM);
		vector<int> values(numOfVariables, Implicant::DONT_CARE);
		normalForm.clear();
		normalForm.insert(Implicant(values));
	}
	return normalForm;
}

Gate::GatePtr TrueGate::getNegatedTwin()
{
	if(!isFlagged(TWIN))
	{
		GatePtr twinPtr(make_shared<FalseGate>());
		setTwinStrong(twinPtr);
		twinPtr->setTwinWeak(shared_from_this());
		/*twinPtr->setFlag(TWIN);
		setFlag(TWIN);*/
	}
	return getTwinSafe();
}

Gate::GatePtr TrueGate::getCopyTwin()
{
	if(!isFlagged(TWIN))
	{
		setTwinStrong(make_shared<TrueGate>());
		//setFlag(TWIN);
	}
	return getTwinStrong();
}

abc::Abc_Obj_t* TrueGate::getAbcNode(abc::Abc_Ntk_t* pNetwork)
{
	if(!isFlagged(ABC_NODE))
	{
		setFlag(ABC_NODE);
		pAbcNode = abc::Abc_AigConst1(pNetwork);
	}
	return pAbcNode;
}



void TrueGate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{}

Gate::GatePtr TrueGate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates)
{
	return shared_from_this();
}
Gate::GatePtr TrueGate::replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates)
{
	return shared_from_this();
}