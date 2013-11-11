#include "FalseGate.h"
#include "TrueGate.h"
//#include "Link.h"

using namespace std;

FalseGate::FalseGate()
{
	this->type = FALSE_GATE;
	this->value = false;
}

/*void FalseGate::deleteRecursively()
{
	delete this;
}*/

bool FalseGate::getValue()
{
	return false;
}

unsigned int FalseGate::getInfixLength()
{
	return 1;
}

unsigned int FalseGate::getPrefixLength()
{
	return 2;
}

void FalseGate::addToStringInfix(std::string& dest)
{
	dest += 'H';
}

void FalseGate::addToStringPrefix(std::string& dest)
{
	dest += "H ";
}

void FalseGate::addToStringPostfix(std::string& dest)
{
	dest += "H ";
}

const set<Implicant>& FalseGate::getDnf(unsigned int numOfVariables)
{
	if(!isFlagged(NORMAL_FORM))
	{
		normalForm.clear();
	}
	return normalForm;
}

Gate::GatePtr FalseGate::getNegatedTwin()
{
	if(!isFlagged(TWIN))
	{
		GatePtr twinPtr(make_shared<TrueGate>());
		setTwinStrong(twinPtr);
		twinPtr->setTwinWeak(shared_from_this());
		/*twinPtr->setFlag(TWIN);
		setFlag(TWIN);*/
	}

	return getTwinSafe();
}

Gate::GatePtr FalseGate::getCopyTwin()
{
	if(!isFlagged(TWIN))
	{
		setTwinStrong(make_shared<FalseGate>());
		setFlag(TWIN);
	}
	return getTwinStrong();
}

abc::Abc_Obj_t* FalseGate::getAbcNode(abc::Abc_Ntk_t* pNetwork)
{
	if(!isFlagged(ABC_NODE))
	{
		setFlag(ABC_NODE);
		pAbcNode = abc::Abc_ObjNot( abc::Abc_AigConst1(pNetwork) );
	}
	return pAbcNode;
}

void FalseGate::replaceInputRecursively(GatePtr original, GatePtr newGate)
{}

Gate::GatePtr FalseGate::replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates)
{
	return shared_from_this();
}
Gate::GatePtr FalseGate::replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates)
{
	return shared_from_this();
}
