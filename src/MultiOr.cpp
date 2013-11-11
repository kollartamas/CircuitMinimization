#include "MultiOr.h"
#include "Link.h"
#include "FalseGate.h"
#include <set>

using namespace std;

MultiOr::MultiOr()
{
	type = MULTI_OR;
}

MultiOr::MultiOr(set<GatePtr>& inputSet) : MultiGate(inputSet)
{
	type = MULTI_OR;
}

bool MultiOr::getValue()
{
	if(!isFlagged(MARKED))
	{
		value = false;
#ifdef VS2010
		for each(Link* inp in inputs)
#else
		for(Link* inp : inputs)
#endif
		{
			if(inp->getInput()->getValue())
			{
				value = true;
				break;
			}
		}
		setFlag(MARKED);
	}
	return value;
}

Gate::GatePtr MultiOr::getSingleTwin()
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
			MultiGatePtr newMulti(make_shared<MultiOr>());
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

Gate::GatePtr MultiOr::getConstantFreeTwin()
{
	if(!isFlagged(TWIN))
	{
		MultiGatePtr newGate(make_shared<MultiOr>());
		for(list<Link*>::iterator iter(inputs.begin()); iter!=inputs.end(); ++iter)
		{
			GatePtr newInput((*iter)->getInput()->getConstantFreeTwin());
			if(newInput->type == TRUE_GATE)	//ha TrueGate van az inputjai között, a twin konstans true
			{
				setTwinStrong(newInput);
				return newInput;
			}
			else if(newInput->type != FALSE_GATE)
			{
				newGate->addInput(newInput);
			}
		}
		//feltöltöttük új inputokkal (a FalseGate-ek kivételével), ha inputs üres, mind FalseGate volt
		if(newGate->inputs.empty())
		{
			setTwinStrong(make_shared<FalseGate>());
		}
		else
		{
			setTwinStrong(newGate);
		}
	}
	return getTwinStrong();
}

void MultiOr::simplifyRecursively()
{
	if(!isFlagged(MARKED))
	{
		GatePtr constFalse = make_shared<FalseGate>();
#ifdef VS2010
		for each(Link* input1 in inputs)
#else
		for(Link* input1 : inputs)
#endif
		{
			input1->getInput()->simplifyRecursively();
			//TODO: összehasonlítani teljesítményt, ha itt történik a konstansok kiváltása
			if(input1->getInput()->type != FALSE_GATE)	//TODO: megnézni szükség van-e erre
			{
#ifdef VS2010
				for each(Link* input2 in inputs)
#else
				for(Link* input2 : inputs)
#endif
				{
					if(/*&*/input1/*->getInput()*/ != /*&*/input2/*->getInput()*/)
					{
						input2->getInput()->replaceInputRecursively(input1->getInput(), constFalse);
						//input2->getInput()->resetFlagRecursive(MARKED2);
					}
				}
			}
			resetFlagRecursive(MARKED_PARENTS);
		}
		setFlag(MARKED);
	}
}