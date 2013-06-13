#ifndef MULTIGATE_H
#define MULTIGATE_H

#include "Gate.h"
#include <map>
#include <list>
#include <set>
//#include <utility>

class MultiGate : public Gate
{
protected:
	std::map<GatePtr, std::list<Link*>::iterator> inputIndex;
public:
	typedef std::shared_ptr<MultiGate> MultiGatePtr;
	typedef std::map<std::pair<GatePtr,GatePtr>,std::list<MultiGatePtr>> occurrenceList;
	typedef std::map<std::pair<GatePtr,GatePtr>,std::list<MultiGatePtr>>::iterator occurrenceIterator;
	MultiGate(){}
	MultiGate(std::set<GatePtr>& inputs);
	//TODO: copy constructor
	~MultiGate();
	
	
	const std::set<Gate*>& getMultiInputs(); 
	void addOccurrencesRecursive(occurrenceList& occurrences, int type);
	bool hasInput(GatePtr input);
	void replaceInputs(GatePtr input1, GatePtr input2, GatePtr newInput, occurrenceList& occurrences, std::list<MultiGate::occurrenceIterator>& newElements);
	//void swapInput(GatePtr original, GatePtr newInput);
	void addInput(GatePtr newInput);
	void removeInput(GatePtr oldInput);
	//virtual GatePtr replaceWithSingleRecursively();	//ha egyetlen inputja van, az kerül a helyére
	void replaceInputRecursively(GatePtr original, GatePtr newGate);
	void removeMultiGatesRecursively();



	//ideiglenes az implementációjuk, hogy ne abstract class legyen:
	//virtual bool getValue();
	/*virtual void deleteRecursively();*/
	virtual void addToStringInfix(std::string& dest);
	virtual void addToStringPrefix(std::string& dest);
	virtual void addToStringPostfix(std::string& dest);
	virtual unsigned int getInfixLength();
	virtual unsigned int getPrefixLength();
	virtual GatePtr getNegatedTwin();
	virtual GatePtr getCopyTwin();
};

#endif