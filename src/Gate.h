#ifndef GATE_H
#define GATE_H

#include <vector>
#include <list>
#include <string>
#include <list>
#include <climits>
#include <map>
#include <set>


class Link;
class MultiGate;

class Gate : public std::enable_shared_from_this<Gate>
{
public:
	typedef std::shared_ptr<Gate> GatePtr;
	typedef std::weak_ptr<Gate> WeakGatePtr;
	typedef std::set<GatePtr> Clause;
	typedef std::set<Clause> ClauseSet;

//protected:
	static unsigned int constrCall, destrCall;

	enum gateTypes {NONE=0, INPUT=1, TRUE=2, FALSE=3, NOT=4, AND=5, OR=6, MULTI_AND=7, MULTI_OR=8};
	enum flagMeaning {RESET=1, MARKED=4, MARKED2=8, TWIN=16, LEVEL=32, MARKED_PARENTS=64, MULTI_INPUTS=128, NORMAL_FORM=256};

	std::list<Link*> inputs;
	std::list<Link*> outputs;
	bool value;
	int type;

	unsigned int level;
	unsigned int markedParents;
	unsigned int stringLength;	//getInfixLength és getPrefixLength során használt
	ClauseSet normalForm;	//használat után mindig resetelni kell, különben shared_ptr maradhat önmagára
	std::set<Gate*> multiInputs;
	unsigned int flags;
	//bool marked;
	//bool marked2;	//ideiglenes, a végleges változatban flaggel váltjuk ki
private:
	GatePtr twinStrong;		//algoritmusok során ideiglenes kapcsolatok tárolására
	WeakGatePtr twinWeak;	
public:


	Gate();
	virtual ~Gate(){destrCall++;}//=0;
	//void addOutput(Gate& out);
	virtual bool getValue() =0;
	/*virtual void deleteRecursively() =0;*/
	bool isFlagged(unsigned int flag);
	void setFlag(unsigned int flag);
	void resetFlag(unsigned int flag);
	void resetFlagRecursive(unsigned int flag=UINT_MAX);
	//void resetAllFlagsRecursive();

	//void resetFlagRecursive(unsigned int flag);
	//void resetMarking2Recursively();	//ideiglenes, a végleges változatban flaggel váltjuk ki
	//void resetMarkingAndTwinRecursively();
	void setMarkingRecursively();

	//stringes kiíratáshoz használt fgv-ek
	virtual void addToStringInfix(std::string& dest) =0;
	virtual void addToStringPrefix(std::string& dest) =0;
	virtual void addToStringPostfix(std::string& dest) =0;
	virtual unsigned int getInfixLength() =0;
	virtual unsigned int getPrefixLength() =0;	//egyenlõ a postfix hosszal
	unsigned int getLevel();
	unsigned int getSize();
	unsigned int getMarkedParents();
	unsigned int incrementMarkedParents();
	virtual const std::set<Gate*>& getMultiInputs(); 



	virtual GatePtr getNegatedTwin() =0;
	virtual GatePtr getCopyTwin() = 0;
	virtual GatePtr getMultiTwin();
	virtual GatePtr getSingleTwin();
	virtual GatePtr getConstantFreeTwin();
	void removeNotGatesRecursively();
	virtual void removeMultiGatesRecursively();
	virtual void addOccurrencesRecursive(std::map<std::pair<GatePtr,GatePtr>,std::list<std::shared_ptr<MultiGate>>>& occurrences, int type);
	//virtual GatePtr replaceWithSingleRecursively();
	virtual void simplifyRecursively();
	virtual void replaceInputRecursively(GatePtr original, GatePtr newGate);

	virtual GatePtr getDnf();
	//virtual ClauseSet& getDnf2();

	GatePtr getTwinStrong();	//olyan esetekben, ahol nem jöhet létre körkörös hivatkozás
	void setTwinStrong(GatePtr twin);	//ha biztosan nem lesz kör
	void setTwinWeak(GatePtr twin);	//ha biztosan kör lesz
	GatePtr getTwinSafe();	//ha (csak twin-eken keresztüli) körkörös hivatkozás elõfordulhat, de az algoritmus végéig kapu nem törlõdik semelyik körbõl (különben a weak_ptr érvénytelenné válhat)
	void setTwinSafe(GatePtr twin);
};

#endif