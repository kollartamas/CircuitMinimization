#include "TrueGate.h"
#include "FalseGate.h"

using namespace std;

TrueGate::TrueGate()
{
	this->type = TRUE;
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