#include "Link.h"
//#include <iostream>

using namespace std;

unsigned int Link::constrCall=0;
unsigned int Link::destrCall=0;

/* Konstruktor
 * Input-output kapcsolatot hoz létre két kapu közt, beállítja a kapuk megfelelõ adattagjait is.
 */
Link::Link(Gate::GatePtr input, Gate* output):
	inputGate(input),
	outputGate(output)
{
	input->outputs.push_back(this);
	iteratorInInput = --input->outputs.end();
	output->inputs.push_back(this);
	iteratorInOutput = --output->inputs.end();
	constrCall++;
}

Link::~Link()
{
	inputGate->outputs.erase(iteratorInInput);
	outputGate->inputs.erase(iteratorInOutput);
	destrCall++;
}

	
Gate::GatePtr Link::getInput()
{
	return inputGate;
}

Gate* Link::getOutput()
{
	return outputGate;
}

void Link::setInput(Gate::GatePtr newInput)
{
	inputGate->outputs.erase(iteratorInInput);
	inputGate = newInput;
	newInput->outputs.push_back(this);
	iteratorInInput = --newInput->outputs.end();
}

/*void Link::setOutput(Gate& newOutput)
{
	outputGate->inputs.erase(iteratorInOutput);
	outputGate = &newOutput;
	newOutput.inputs.push_back(this);
	iteratorInOutput = --newOutput.inputs.end();
}*/