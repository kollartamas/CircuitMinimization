#ifndef LINK_H
#define LINK_H

#include "Gate.h"
#include <list>
#include <vector>

class Link
{
private:
	Gate::GatePtr inputGate;
	Gate* outputGate;
	std::list<Link*>::iterator iteratorInInput;
	std::list<Link*>::iterator iteratorInOutput;
public:
	Link(Gate::GatePtr input, Gate* output);
	~Link();

	Gate::GatePtr getInput();
	Gate* getOutput();
	void setInput(Gate::GatePtr newInput);
	//void setOutput(Gate& newOutput);
};

#endif