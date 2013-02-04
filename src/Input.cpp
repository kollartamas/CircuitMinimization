#include "Input.h"
#include "NotGate.h"

#include <cmath>

using namespace std;

Input::Input(unsigned int id)
{
	this->id = id;
	this->tipus = INPUT;
	this->value = false;

	inputs = vector<Kapu*>();
	outputs = list<Kapu*>();
}

void Input::setValue(bool value)
{
	this->value = value;
}

void Input::erteketFrissit()
{
}

void Input::calculateInfixLength()
{
	stringLength = static_cast<unsigned int>(log10((double)(id+1)));
}

void Input::calculatePrefixLength()
{
	stringLength = 1 + static_cast<unsigned int>(log10((double)(id+1)));
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

void Input::createNegatedTwin()
{
	twin = new NotGate(*this);
	this->addOutput(twin);
	//twin->twin = this;
}