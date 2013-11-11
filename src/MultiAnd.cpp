#include "MultiAnd.h"
#include "Link.h"
#include "TrueGate.h"
#include "FalseGate.h"

#include <set>


using namespace std;

MultiAnd::MultiAnd()
{
	type = MULTI_AND;
}

MultiAnd::MultiAnd(set<GatePtr>& inputSet) : MultiGate(inputSet)
{
	type = MULTI_AND;
}

bool MultiAnd::getValue()
{
	if(!isFlagged(MARKED))
	{
		value = true;
#ifdef VS2010
		for each(Link* inp in inputs)
#else
		for(Link* inp : inputs)
#endif
		{
			if(!inp->getInput()->getValue())
			{
				value = false;
				break;
			}
		}
		setFlag(MARKED);
	}
	return value;
}

Gate::GatePtr MultiAnd::getSingleTwin()
{
	if(!isFlagged(TWIN))
	{
		list<Link*>::iterator iter(inputs.begin());
		if(inputs.size() == 1)
		{
			setTwinStrong((*iter)->getInput()->getSingleTwin());
		}
		else
		{
			MultiGatePtr newMulti(make_shared<MultiAnd>());
#ifdef VS2010
			for each(Link* input in inputs)
#else
			for(Link* input : inputs)
#endif
			{
				newMulti->addInput(input->getInput()->getSingleTwin());
			}
			setTwinStrong(newMulti);
			newMulti->setTwinWeak(newMulti);	//TODO: megnézni erre szükség van-e (getTwinSafe helyett getTwinStrong-gal)
		}
	}
	return getTwinSafe();
}

Gate::GatePtr MultiAnd::getConstantFreeTwin()
{
	if(!isFlagged(TWIN))
	{
		MultiGatePtr newGate(make_shared<MultiAnd>());
		for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); ++iter)
		{
			GatePtr newInput((*iter)->getInput()->getConstantFreeTwin());
			if(newInput->type == FALSE_GATE)	//ha FalseGate van az inputjai között, a twin konstans false
			{
				setTwinStrong(newInput);
				return newInput;
			}
			else if(newInput->type != TRUE_GATE)
			{
				newGate->addInput(newInput);
			}
		}
		//feltöltöttük új inputokkal (a TrueGate-ek kivételével), ha inputs üres, mind TrueGate volt
		if(newGate->inputs.empty())
		{
			setTwinStrong(make_shared<TrueGate>());
		}
		else
		{
			setTwinStrong(newGate);
		}
	}
	return getTwinStrong();
}

void MultiAnd::simplifyRecursively()
{
	if(!isFlagged(MARKED))
	{
		GatePtr constTrue = make_shared<TrueGate>();
#ifdef VS2010
		for each(Link* input1 in inputs)
#else
		for(Link* input1 : inputs)
#endif
		{
			input1->getInput()->simplifyRecursively();
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			if(input1->getInput()->type != TRUE_GATE)
			{
#ifdef VS2010
				for each(Link* input2 in inputs)
#else
				for(Link* input2 : inputs)
#endif
				{
					if(/*&*/input1/*->getInput()*/ != /*&*/input2/*->getInput()*/)
					{
						input2->getInput()->replaceInputRecursively(input1->getInput(), constTrue);
						//input2->getInput()->resetFlagRecursive(MARKED2);
					}
				}
			}
			resetFlagRecursive(MARKED_PARENTS);
		}
		setFlag(MARKED);
	}
}