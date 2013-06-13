#ifndef INPUT_H
#define INPUT_H

#include "Gate.h"
#include <string>

class Input: public Gate
{
public:
	typedef std::shared_ptr<Input> InputPtr;	
//private:
	unsigned int id;
public:
	Input(unsigned int id=0);
	Input(Input& other);
	~Input(){}
	/*void deleteRecursively(){};*/

	bool getValue();
	void setValue(bool val);

	
	void addToStringInfix(std::string& dest);
	void addToStringPrefix(std::string& dest);
	void addToStringPostfix(std::string& dest);
	unsigned int getInfixLength();
	unsigned int getPrefixLength();

	GatePtr getNegatedTwin();
	GatePtr getCopyTwin();
};

#endif