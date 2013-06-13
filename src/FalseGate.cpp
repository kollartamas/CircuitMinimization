#include "FalseGate.h"
#include "TrueGate.h"
//#include "Link.h"

using namespace std;

FalseGate::FalseGate()
{
	this->type = FALSE;
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

