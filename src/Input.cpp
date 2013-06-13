#include "Input.h"
#include "NotGate.h"

#include <cmath>

using namespace std;

Input::Input(unsigned int id)
{
	this->id = id;
	this->type = INPUT;
	this->value = false;
}

Input::Input(Input& other)
{
	this->id = other.id;
	this->type = INPUT;
	this->value = other.value;
}

void Input::setValue(bool value)
{
	this->value = value;
}

bool Input::getValue()
{
	return value;
}

unsigned int Input::getInfixLength()
{
	if(!isFlagged(MARKED))
	{
		stringLength = 1 + static_cast<unsigned int>(log10((double)(id==0?1:id)));
		setFlag(MARKED);
	}
	return stringLength;
}

unsigned int Input::getPrefixLength()
{
	if(!isFlagged(MARKED))
	{
		stringLength = 2 + static_cast<unsigned int>(log10((double)(id==0?1:id)));
		setFlag(MARKED);
	}
	return stringLength;
}

void Input::addToStringInfix(std::string& dest)
{
	dest += to_string(static_cast<unsigned long long>(id));	//cast VS2010 kompatibilitás miatt
}

void Input::addToStringPrefix(std::string& dest)
{
	dest += to_string(static_cast<unsigned long long>(id));	//cast VS2010 kompatibilitás miatt
	dest += ' ';
}

void Input::addToStringPostfix(std::string& dest)
{
	dest += to_string(static_cast<unsigned long long>(id));	//cast VS2010 kompatibilitás miatt
	dest += ' ';
}

Gate::GatePtr Input::getNegatedTwin()
{
	if(!isFlagged(TWIN))
	{
		GatePtr twinPtr(make_shared<NotGate>(shared_from_this()));
		setTwinStrong(twinPtr);
		twinPtr->setTwinWeak(shared_from_this());
	}

	return getTwinStrong();
}

Gate::GatePtr Input::getCopyTwin()
{
	if(!isFlagged(TWIN))
	{
		setTwinStrong(make_shared<Input>(*this));
		setFlag(TWIN);
	}
	return getTwinStrong();
}