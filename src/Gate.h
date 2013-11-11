#ifndef GATE_H
#define GATE_H

#include <vector>
#include <list>
#include <string>
#include <list>
#include <climits>
#include <map>
#include <set>
#include <memory>

#include "abc.h"
#include "Implicant.h"


class Link;
class MultiGate;

class Gate : public std::enable_shared_from_this<Gate>
{
public:
	typedef std::shared_ptr<Gate> GatePtr;
	typedef std::weak_ptr<Gate> WeakGatePtr;
	//typedef std::set<Implicant> ImplicantSet;

//protected:
	static unsigned int constrCall, destrCall;

	enum gateTypes {NONE=0, INPUT=1, TRUE_GATE=2, FALSE_GATE=3, NOT=4, AND=5, OR=6, MULTI_AND=7, MULTI_OR=8};
	enum flagMeaning {RESET=1, MARKED=4, MARKED2=8, TWIN=16, LEVEL=32, MARKED_PARENTS=64, MULTI_INPUTS=128, NORMAL_FORM=256, FACTORIZED=512, MULTI_INPUTS_SINGLE_OUTPUT=1024, MULTI_INPUTS_DOUBLE_OUTPUT=2048, MULTI_INPUTS_LINK=4096, ABC_NODE=8192};

	std::list<Link*> inputs;
	std::list<Link*> outputs;
	bool value;
	gateTypes type;
	unsigned int id;

	unsigned int level;
	unsigned int markedParents;
	unsigned int stringLength;	//getInfixLength �s getPrefixLength sor�n haszn�lt
	std::set<Implicant> normalForm;
	std::set<Gate*> multiInputs;
	std::map<GatePtr, Link*> multiInputsWithLink;
	std::set<Gate*> inputIntersection;	//kiemelhet� k�z�s t�nyez�k halmaza
	//std::set<Gate>
	unsigned int flags;
	//bool marked;
	//bool marked2;	//ideiglenes, a v�gleges v�ltozatban flaggel v�ltjuk ki
	abc::Abc_Obj_t* pAbcNode;
private:
	GatePtr twinStrong;		//algoritmusok sor�n ideiglenes kapcsolatok t�rol�s�ra
	WeakGatePtr twinWeak;
public:


	Gate();
	virtual ~Gate();//=0;
	//void addOutput(Gate& out);
	virtual bool getValue() =0;
	/*virtual void deleteRecursively() =0;*/
	bool isFlagged(unsigned int flag);
	void setFlag(unsigned int flag);
	void resetFlag(unsigned int flag);
	void resetFlagRecursive(unsigned int flag=UINT_MAX);
	//void resetAllFlagsRecursive();

	//void resetFlagRecursive(unsigned int flag);
	//void resetMarking2Recursively();	//ideiglenes, a v�gleges v�ltozatban flaggel v�ltjuk ki
	//void resetMarkingAndTwinRecursively();
	void setMarkingRecursively();

	//stringes ki�rat�shoz haszn�lt fgv-ek
	virtual void addToStringInfix(std::string& dest) =0;
	virtual void addToStringPrefix(std::string& dest) =0;
	virtual void addToStringPostfix(std::string& dest) =0;
	virtual unsigned int getInfixLength() =0;
	virtual unsigned int getPrefixLength() =0;	//egyenl� a postfix hosszal
	virtual unsigned int printInputsRecursively(unsigned int& nextId);
	unsigned int getLevel();
	virtual unsigned int getSize();
	virtual unsigned int getSizeAbcStyle();
	unsigned int getMarkedParents();
	unsigned int incrementMarkedParents();
	virtual const std::set<Gate*>& getMultiInputs();
	virtual const std::map<GatePtr,Link*>& getMultiInputsWithLink();
	virtual const std::set<Gate*>& getMultiInputsWithSingleOutput();
	virtual const std::set<Gate*>& getMultiInputsWithDoubleOutput();



	virtual GatePtr getNegatedTwin() =0;
	virtual GatePtr getCopyTwin() =0;
	virtual GatePtr getMultiTwin();	//a h�l�zat belsej�ben nem lehetnek NOT kapuk
	virtual GatePtr getSingleTwin();
	virtual GatePtr getConstantFreeTwin();
	virtual abc::Abc_Obj_t* getAbcNode(abc::Abc_Ntk_t* network) =0;
	virtual gateTypes getIndirectType();
	void removeNotGatesRecursively();
	void removeConstGatesRecursively();
	void removeConstGatesRecursivelyIfFlagged(int flag);
	virtual void removeMultiGatesRecursively();
	virtual void addOccurrencesRecursive(std::map<std::pair<GatePtr,GatePtr>,std::list<std::shared_ptr<MultiGate>>>& occurrences, int type);
	//virtual GatePtr replaceWithSingleRecursively();
	virtual void simplifyRecursively();
	virtual void simplifyRecursively2();
	virtual void simplifyRecursively3();
	virtual void replaceInputRecursively(GatePtr original, GatePtr newGate);
	virtual GatePtr replaceInputRecursively2(Gate* original, GatePtr newGate, Link* protectedLink, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	virtual GatePtr replaceInputRecursively3(Gate* original, GatePtr newGate, Link* protectedLink1, Link* protectedLink2, std::map<GatePtr,Link*>& sourceInputs, std::list<GatePtr>& separatedGates);
	virtual bool factorizeRecursively();	//return: volt-e v�ltoz�s az adott r�szf�ban

	virtual const std::set<Implicant>& getDnf(unsigned int numOfVariables) =0;
	//virtual ClauseSet& getDnf2();

	GatePtr getTwinStrong();	//olyan esetekben, ahol nem j�het l�tre k�rk�r�s hivatkoz�s
	void setTwinStrong(GatePtr twin);	//ha biztosan nem lesz k�r
	void setTwinWeak(GatePtr twin);	//ha biztosan k�r lesz
	GatePtr getTwinSafe();	//ha (csak twin-eken kereszt�li) k�rk�r�s hivatkoz�s el�fordulhat, de az algoritmus v�g�ig kapu nem t�rl�dik semelyik k�rb�l (k�l�nben a weak_ptr �rv�nytelenn� v�lhat)
	void setTwinSafe(GatePtr twin);

protected:
	void removeKeyAndValueFromMap(GatePtr key, Link* value, std::map<GatePtr,Link*>& source);
	std::string typeToString(int type);

};

#endif