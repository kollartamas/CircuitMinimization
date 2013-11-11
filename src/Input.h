#ifndef INPUT_H
#define INPUT_H

#include "Gate.h"
#include <string>

class Input: public Gate
{
public:
	typedef std::shared_ptr<Input> InputPtr;	
//private:
public:
	Input(unsigned int id=0);
	Input(Input& other);
	~Input(){}
	/*void deleteRecursively(){};*/

	unsigned int getSizeAbcStyle();

	bool getValue();
	void setValue(bool val);

	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);
	unsigned int getInfixLength();
	unsigned int getPrefixLength();
	unsigned int printInputsRecursively(unsigned int& nextId);

	const std::set<Implicant>& getDnf(unsigned int numOfVariables);

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
	abc::Abc_Obj_t* getAbcNode(abc::Abc_Ntk_t* network);
};

#endif