#include "Circuit.h"
#include "TrueGate.h"
#include "FalseGate.h"
#include "NotGate.h"
#include "AndGate.h"
#include "OrGate.h"
#include "Link.h"
#include "MultiGate.h"
#include "Logger.h"

#include "main.h"

#include <set>

#include <iostream>	//ideiglenes

using namespace std;

Circuit::Circuit(unsigned int numOfInputs):
	inputs(numOfInputs)
{
	for(unsigned int i=0; i<numOfInputs; i++)
	{
		inputs[i] = make_shared<Input>(i);
	}
}

Circuit::Circuit(unsigned int numOfInputs, unsigned int numOfGates, bool includeConstants):
	inputs(numOfInputs)
{
	vector<Gate::GatePtr> elements;
	elements.reserve(2 + numOfInputs + numOfGates);
	set<Gate::GatePtr> deadEnds;
	
	if(includeConstants)
	{
		elements.push_back(make_shared<TrueGate>());
		elements.push_back(make_shared<FalseGate>());
	}

	for(unsigned int i=0; i<numOfInputs; i++)
	{
		Input::InputPtr newInput = make_shared<Input>(i);
		inputs[i] = newInput;
		elements.push_back(newInput);
		deadEnds.insert(newInput);
	}

	//generalunk veletlenszeru kapukat, amik inputkent a korabbi elemeket kapjak
	//elso szakaszban szabadon valaszthatunk:
	for(unsigned int i=0; numOfGates >= i+deadEnds.size(); i++)
	{
		Gate::GatePtr newGate, inp1, inp2;
		int gateType = rand()%3;

		switch(gateType)
		{
		case 0:
			inp1 = elements[rand()%elements.size()];
			newGate = make_shared<NotGate>(inp1);
			deadEnds.erase(inp1);
			deadEnds.insert(newGate);
			elements.push_back(newGate);
			break;
		case 1:
			inp1 = elements[rand()%elements.size()];
			inp2 = elements[rand()%elements.size()];
			newGate = make_shared<AndGate>(inp1, inp2);
			deadEnds.erase(inp1);
			deadEnds.erase(inp2);
			deadEnds.insert(newGate);
			elements.push_back(newGate);
			break;
		case 2:
			inp1 = elements[rand()%elements.size()];
			inp2 = elements[rand()%elements.size()];
			newGate = make_shared<OrGate>(inp1, inp2);
			deadEnds.erase(inp1);
			deadEnds.erase(inp2);
			deadEnds.insert(newGate);
			elements.push_back(newGate);
			break;
		}
	}
	//masodik szakaszban ugy valasztunk, hogy fogyjon az output nelkuli kapuk szama:
	set< pair<int, Gate::GatePtr> > randomOrdered;
	//mindhez Gate-hez rendelunk egy veletlen int-et, ez lesz a pair elso tagja
	//a set-en beluli sorrendet a pair elso tagja hatarozza meg (egyezes eseten a masodik)
	for(set<Gate::GatePtr>::iterator iter=deadEnds.begin(); iter!=deadEnds.end(); iter++)
	{
		randomOrdered.insert(pair<int, Gate::GatePtr>(rand(), *iter));
	}
	while(randomOrdered.size()>1)
	{
		Gate::GatePtr newGate, inp1, inp2;

		set< pair<int, Gate::GatePtr> >::iterator iter(randomOrdered.begin());
		inp1 = (iter->second);
		randomOrdered.erase(iter);
		iter = randomOrdered.begin();
		inp2 = (iter->second);
		randomOrdered.erase(iter);

		if(rand()%2)
		{
			newGate = make_shared<AndGate>(inp1, inp2);
		}
		else
		{
			newGate = make_shared<OrGate>(inp1, inp2);
		}

		randomOrdered.insert(pair<int, Gate::GatePtr>(rand(), newGate));
	}
	output = randomOrdered.begin()->second;
}

Circuit::Circuit(Circuit& other):
	inputs(other.inputs.size())
{
	for(unsigned int i=0; i<other.inputs.size(); i++)
	{
		inputs[i] = static_pointer_cast<Input>(other.inputs[i]->getCopyTwin());
	}
	this->output = other.output->getCopyTwin();
	other.output->resetFlagRecursive(Gate::TWIN);
}

Circuit::~Circuit()
{
	/*output->deleteRecursively();
	for each(Input::InputPtr input in inputs)
	{
		delete input;
	}*/
}

bool Circuit::operator == (Circuit& other)
{
	if(this->inputs.size() != other.inputs.size())	return false;

	for(unsigned int i=0; i<inputs.size(); i++)
	{
		(*this)[i].setValue(false);
		other[i].setValue(false);
	}
/*for(unsigned int i=0; i<inputs.size(); i++)
{
cout << (*this)[i].getValue() <<" ";
}
cout << endl;	*/
	if(this->getOutputValue() != other.getOutputValue()) {return false;}
	
	unsigned int currentIndex = 0;
	while(currentIndex<inputs.size())
	{
		if((*this)[currentIndex].getValue())
		{
			(*this)[currentIndex].setValue(false);
			other[currentIndex++].setValue(false);
			//átvitel van, változtatjuk a következõ inputot is
		}
		else
		{
			(*this)[currentIndex].setValue(true);
			other[currentIndex].setValue(true);
			currentIndex = 0;
			//nincs több átvitel, tesztelhetõ az output értéke
/*for(unsigned int i=0; i<inputs.size(); i++)
{
cout << (*this)[i].getValue() <<" ";
}
cout << endl;*/
			if(this->getOutputValue() != other.getOutputValue())
			{
				return false;
			}
		}
	}
	return true;
}

Input& Circuit::operator[](unsigned int id)
{
	return *inputs[id];
}

void Circuit::setOutput(Gate::GatePtr out)
{
	this->output = out;
}

bool Circuit::getOutputValue()
{
	bool r = output->getValue();
	output->resetFlagRecursive(Gate::MARKED);
	return r;
}

unsigned int Circuit::getSize()
{
	unsigned int size = output->getSize();
	output->resetFlagRecursive(Gate::MARKED);
	return size;
}

unsigned int Circuit::getLevel()
{
	unsigned int level = output->getLevel();
	output->resetFlagRecursive(Gate::LEVEL);
	return level;
}

unsigned int Circuit::getSizeAbcStyle()
{
	unsigned int size = output->getSizeAbcStyle();
	output->resetFlagRecursive(Gate::MARKED);
	return size;
}

std::string Circuit::toStringInfix()
{
	string formula;
	formula.reserve(output->getInfixLength());
	output->resetFlagRecursive(Gate::MARKED);
	output->addToStringInfix(formula);
	return formula;
}

std::string Circuit::toStringPrefix()
{
	string formula;
	formula.reserve(output->getPrefixLength());
	output->resetFlagRecursive(Gate::MARKED);
	output->addToStringPrefix(formula);
	return formula;
}

std::string Circuit::toStringPostfix()
{
	string formula;
	formula.reserve(output->getPrefixLength());	//pre- és postfix hossz megegyezik
	output->resetFlagRecursive(Gate::MARKED);
	output->addToStringPostfix(formula);
	return formula;
}

void Circuit::removeNotGates()
{
	Gate::GatePtr oldOutput(output);	//az eredetit megtartjuk, és a másolatán hajtjuk végre az algoritmust, ezért akkor is marad minden kapura strong reference, ha a másolatból törlünk
	output = output->getCopyTwin();		//TODO: normális megoldást keresni
	for(unsigned int i=0; i<inputs.size(); i++){inputs[i]=static_pointer_cast<Input>(inputs[i]->getCopyTwin());}	//az inputokat is átállítjuk a másolatra
	while(output->type == Gate::NOT)
	{
		if(output->inputs.front()->getInput()->type == Gate::INPUT)	//ha a NOT közvetlenül egy INPUT-hoz kapcsolódik 
		{
			break;
		}
		else
		{
			output = output->getNegatedTwin()->getNegatedTwin();
		}
	}
	output->removeNotGatesRecursively();
	output->resetFlagRecursive(Gate::MARKED | Gate::TWIN);
}

void Circuit::removeMultiGates()
{
	while((output->type == Gate::MULTI_AND || output->type == Gate::MULTI_OR) && output->inputs.size()==1)
	{
		output = output->inputs.front()->getInput();
	}
	output->removeMultiGatesRecursively();

	output->resetFlagRecursive(Gate::MARKED | Gate::TWIN);
}

Gate::GatePtr Circuit::buildFromDnf(const std::set<Implicant>& dnf)
{
	/*if(dnf.empty())
	{
		return make_shared<FalseGate>();
	}
	else
	{*/
	Gate::GatePtr retVal(make_shared<FalseGate>());
		/*set<Implicant>::iterator iter(dnf.begin());
		Gate::GatePtr top(buildFromImplicant(*iter));
		for(++iter; iter!=dnf.end(); ++iter)
		{
			top = make_shared<OrGate>(top, buildFromImplicant(*iter));
		}*/
	list<Gate::GatePtr> gateQueue;
	for(set<Implicant>::iterator iter(dnf.begin()); iter!=dnf.end(); ++iter)
	{
		gateQueue.push_back(buildFromImplicant(*iter));
	}
	if(gateQueue.size()==1)
	{
		retVal = gateQueue.front();
	}
	else if(!gateQueue.empty())	//kiegyensúlyozva építjük fel
		for(;;)
		{
			Gate::GatePtr inp1(gateQueue.front());
			gateQueue.pop_front();
			Gate::GatePtr inp2(gateQueue.front());
			gateQueue.pop_front();
			if(gateQueue.empty())
			{
				retVal = make_shared<OrGate>(inp1, inp2);
				break;
			}
			else
			{
				gateQueue.push_back(make_shared<OrGate>(inp1, inp2));
			}
		}
	return retVal;
}

Gate::GatePtr Circuit::buildFromImplicant(const Implicant& implicant)
{
	if(!implicant.valid)
	{
		return make_shared<FalseGate>();
	}
	else if(implicant.numOfDontCares == implicant.numOfVariables)
	{
		return make_shared<TrueGate>();
	}
	else
	{
		list<Gate::GatePtr> gateQueue;
		for(unsigned int i=0; i<implicant.values.size(); i++)
		{
			if(implicant.values[i] == Implicant::TRUE)
			{
				gateQueue.push_back(inputs[i]);
			}
			else if(implicant.values[i] == Implicant::FALSE)
			{
				gateQueue.push_back(make_shared<NotGate>(inputs[i]));
			}
		}
		if(gateQueue.size()==1)
		{
			return gateQueue.front();
		}
		for(;;)	//kiegyensúlyozva építjük fel
		{
			Gate::GatePtr inp1(gateQueue.front());
			gateQueue.pop_front();
			Gate::GatePtr inp2(gateQueue.front());
			gateQueue.pop_front();
			if(gateQueue.empty())
			{
				return make_shared<AndGate>(inp1, inp2);
			}
			else
			{
				gateQueue.push_back(make_shared<AndGate>(inp1, inp2));
			}
		}
		return NULL;
	}
}

void Circuit::printStructure()
{
	unsigned int startIndex = inputs.size();
	output->printInputsRecursively(startIndex);
	output->resetFlagRecursive(Gate::MARKED);
}

#include "mainInt.h"

void Circuit::optimizeAbc()
{
	//abc::Abc_Start();	//main-be átrakva

	//And-Inverter gráf létrehozása
	abc::Abc_Ntk_t* pAig;
	pAig = abc::Abc_NtkAlloc( abc::ABC_NTK_STRASH, abc::ABC_FUNC_AIG, 1 );
	char * pName = "newAig";
	pAig->pName = abc::Extra_UtilStrsav( pName );
	//kapuk és output építése:
	abc::Abc_Obj_t* pOut = abc::Abc_NtkCreatePo(pAig);
	abc::Abc_ObjAddFanin( pOut, output->getAbcNode(pAig) );
	abc::Abc_AigCleanup( (abc::Abc_Aig_t*)pAig->pManFunc );	//output nélküli kapuk törlése

	abc::Abc_Frame_t* pFrame = abc::Abc_FrameGetGlobalFrame();
	abc::Abc_FrameSetCurrentNetwork(pFrame, pAig);

	//logger indítása:
#ifdef LOG_MAX_GATENUM
	Logger::getLogger().setData(Logger::STARTING_GATENUM, Gate::constrCall-Gate::destrCall);
#endif
#ifdef LOG_SIZE
	Logger::getLogger().setData(Logger::STARTING_SIZE, pFrame->pNtkCur->nObjs);	//NtkCur: current network nObjs: number of objects
	Logger::getLogger().setData(Logger::DNF_SIZE, 0);
#endif
#ifdef LOG_LEVEL
	Logger::getLogger().setData(Logger::STARTING_LEVEL, pFrame->pNtkCur->LevelMax);
#endif
#ifdef LOG_TIME
	Logger::getLogger().setData(Logger::DNF_TIME, 0);
	Logger::getLogger().updateTimer();
#endif

	//a konkrét algoritmusok hívása:
	//abc::Cmd_CommandExecute( pFrame, string("resyn").c_str() );
	abc::Cmd_CommandExecute( pFrame, string("resyn2").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("resyn2a").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("resyn3").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("compress").c_str() );
	abc::Cmd_CommandExecute( pFrame, string("compress2").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("choice").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("choice2").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("rwsat").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("drwsat2").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("share").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("&sw").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("&fx_").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("&dc3").c_str() );
	//abc::Cmd_CommandExecute( pFrame, string("&dc4").c_str() );

#ifdef LOG_TIME
	Logger::getLogger().setTime(Logger::FINISH_TIME);
#endif
	//TODO: Circuit újraépítése abc-beli AIG alapján
#ifdef LOG_SIZE
	Logger::getLogger().setData(Logger::FINAL_SIZE, pFrame->pNtkCur->nObjs);
#endif
#ifdef LOG_LEVEL
	Logger::getLogger().setData(Logger::FINAL_LEVEL, pFrame->pNtkCur->LevelMax); //TODO
#endif



	//abc::Cmd_CommandExecute( pFrame, string("print_stats").c_str() );


	//AIG törlése:
	Abc_FrameDeleteAllNetworks(pFrame);
	//abc::Abc_Stop();
}