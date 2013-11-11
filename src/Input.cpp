#include "Input.h"
#include "NotGate.h"

#include <cmath>
#include <iostream>	//TODO: kiszervezni innen

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

unsigned int Input::getSizeAbcStyle()	//NOT �s INPUT kapuk n�lk�l
{
	return 0;
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
	dest += to_string(static_cast<unsigned long long>(id));	//cast VS2010 kompatibilit�s miatt
}

void Input::addToStringPrefix(std::string& dest)
{
	dest += to_string(static_cast<unsigned long long>(id));	//cast VS2010 kompatibilit�s miatt
	dest += ' ';
}

void Input::addToStringPostfix(std::string& dest)
{
	dest += to_string(static_cast<unsigned long long>(id));	//cast VS2010 kompatibilit�s miatt
	dest += ' ';
}

const set<Implicant>& Input::getDnf(unsigned int numOfVariables)
{
	if(!isFlagged(NORMAL_FORM))
	{
		setFlag(NORMAL_FORM);
		vector<int> values(numOfVariables, Implicant::DONT_CARE);
		values[id] = Implicant::TRUE;
		normalForm.clear();
		normalForm.insert(Implicant(values));
	}
	return normalForm;
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

unsigned int Input::printInputsRecursively(unsigned int& nextId)
{
	if(!isFlagged(MARKED))
	{
		setFlag(MARKED);
		cout << id <<": input" << endl;
	}
	return id;
}

abc::Abc_Obj_t* Input::getAbcNode(abc::Abc_Ntk_t* pNetwork)
{
	if(!isFlagged(ABC_NODE))
	{
		setFlag(ABC_NODE);
		pAbcNode = abc::Abc_NtkCreatePi( pNetwork );
		/*char* pName;
		sprintf(pName, "%ud", id);*/
		abc::Abc_ObjAssignName( pAbcNode, (char*)to_string(static_cast<unsigned long long>(id)).c_str(), NULL );	//TODO: copy-t t�rol?
	}
	return pAbcNode;
}