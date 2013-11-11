#include "DnfCircuit.h"
#include "Link.h"
#include "FalseGate.h"
#include "TrueGate.h"
#include "OrGate.h"
#include "AndGate.h"
#include "NotGate.h"
#include "ImplicantChart.h"
#include "PlaFormatException.h"
#include "Logger.h"

#include <cstdio>

extern "C" int espresso_main(FILE* input, FILE* output, int argc=0, char **argv=NULL);

using namespace std;

DnfCircuit::DnfCircuit(list<Implicant>& implicantList) : CircuitWithoutNotGate(implicantList.size() ? (implicantList.front().numOfVariables) : 0)
{
	output = buildFromImplicantList(implicantList);
}


DnfCircuit::DnfCircuit(unsigned int variables, unsigned int clauses) : CircuitWithoutNotGate(variables)
{
	list<Implicant> implicantList;
	for(unsigned int i=0; i<clauses; i++)
	{
		vector<int> values(variables, Implicant::DONT_CARE);
		int dontCareRate = rand();
		for(unsigned int j=0; j<variables; j++)
		{
			if(rand()>dontCareRate)
			{
				values[j]=Implicant::TRUE + rand()%2;	//ha rand==0 TRUE_GATE, rand==1 FALSE_GATE
			}
		}
		implicantList.push_back(Implicant(values));
	}
	output = buildFromImplicantList(implicantList);
}

DnfCircuit::DnfCircuit(DnfCircuit& other):CircuitWithoutNotGate(other)
{}

DnfCircuit::DnfCircuit(Circuit& other): CircuitWithoutNotGate(other)
{
	this->removeNotGates();
	this->toDnf();
}

DnfCircuit::DnfCircuit(CircuitWithoutNotGate& other): CircuitWithoutNotGate(other)
{
	this->toDnf();
}

Gate::GatePtr DnfCircuit::buildFromImplicantList(const std::list<Implicant>& implicantList)
{
	if(implicantList.empty())
	{
		return make_shared<FalseGate>();
	}

	list<Implicant>::const_iterator iter(implicantList.begin());
	Gate::GatePtr topClause(buildFromImplicant(*iter));
	++iter;
	for(; iter!=implicantList.end(); ++iter)
	{
		topClause = make_shared<OrGate>(topClause, buildFromImplicant(*iter));
	}
	return topClause;
}

Gate::GatePtr DnfCircuit::buildFromImplicant(const Implicant& implicant)
{
	if(implicant.numOfVariables==implicant.numOfDontCares)
	{
		return make_shared<TrueGate>();
	}

	Gate::GatePtr topLiteral;
	unsigned int i=0;
	//az elsõ elõforduló literált keressük:
	for(; i<implicant.values.size(); i++)
	{
		switch(implicant.values[i])
		{
			case Implicant::TRUE:
				topLiteral = inputs[i];
				break;
			case Implicant::FALSE:
				topLiteral = make_shared<NotGate>(inputs[i]);
				break;
			case Implicant::INVALID:
				topLiteral = make_shared<FalseGate>();
				break;
			default:
				continue;
		}
		break;			//TRUE_GATE/FALSE_GATE esetén break(a ciklusból), különben continue;
	}
	i++;
	//hozzákapcsoljuk a többi literált is:
	for(; i<implicant.values.size(); i++)
	{
		switch(implicant.values[i])
		{
			case Implicant::TRUE:
				topLiteral = make_shared<AndGate>(inputs[i], topLiteral);
				break;
			case Implicant::FALSE:
				topLiteral = make_shared<AndGate>(make_shared<NotGate>(inputs[i]), topLiteral);
				break;
			case Implicant::INVALID:
				topLiteral = make_shared<AndGate>(make_shared<FalseGate>(), topLiteral);
				break;
		}
	}
	return topLiteral;
}

void DnfCircuit::listClausesInSubgraph(Gate& rootGate, list<Gate*>& clauseList)
{
	if(rootGate.type == Gate::OR)
	{
#ifdef VS2010
		for each(Link* input in rootGate.inputs)
#else
		for(Link* input : rootGate.inputs)
#endif
		{
			listClausesInSubgraph(*input->getInput(), clauseList);
		}
	}
	else
	{
		clauseList.push_back(&rootGate);
	}
}


Implicant DnfCircuit::getImplicantFromSubgraph(Gate& rootGate)
{
	vector<int> values(inputs.size(), Implicant::DONT_CARE);
	collectInputsFromSubgraph(rootGate, values);
	return Implicant(values);
}

void DnfCircuit::collectInputsFromSubgraph(Gate& rootGate, std::vector<int>& currentValues)
{
	switch(rootGate.type)
	{
		case Gate::AND:
			collectInputsFromSubgraph(*rootGate.inputs.front()->getInput(), currentValues);
			collectInputsFromSubgraph(*(*++rootGate.inputs.begin())->getInput(), currentValues);
			break;
		case Gate::INPUT:
			currentValues[((Input*)(&rootGate))->id] &= Implicant::TRUE;	//bitwise AND, mert True:01 False:10 DontCare:11 (binary)
			break;
		case Gate::NOT:
			currentValues[((Input*)(rootGate.inputs.front()->getInput().get()))->id] &= Implicant::FALSE;
			break;
		case Gate::TRUE_GATE:
			break;
		case Gate::FALSE_GATE:
			if(currentValues.empty()){currentValues.push_back(Implicant::INVALID);}
			else{currentValues[0]=Implicant::INVALID;}
			break;
		default:
			throw runtime_error("DNF error");
	}
}

void DnfCircuit::minimizeQuineMcClusky()
{
	
#ifdef LOG_MAX_GATENUM
	Logger::getLogger().setData(Logger::STARTING_GATENUM, Gate::constrCall-Gate::destrCall);
#endif
#ifdef LOG_SIZE
	Logger::getLogger().setData(Logger::DNF_SIZE, getSize());
#endif
#ifdef LOG_TIME
	Logger::getLogger().setTime(Logger::DNF_TIME);
	Logger::getLogger().updateTimer();
#endif

	ImplicantChart implicantChart;
	//list<Implicant> mintermList(getMintermList());	//move constructor
#ifdef VS2010
	for each(Implicant implicant in getMintermList())
#else
	for(Implicant& implicant : getMintermList())
#endif
	{
		implicantChart.addMinterm(implicant, true);
	}
	//output->deleteRecursively();
	setOutput( buildFromImplicantList(implicantChart.getMinimizedDNF()) );

#ifdef LOG_TIME
	Logger::getLogger().setTime(Logger::FINISH_TIME);
#endif
#ifdef LOG_SIZE
	Logger::getLogger().setData(Logger::FINAL_SIZE, getSize());
#endif
#ifdef LOG_LEVEL
	Logger::getLogger().setData(Logger::FINAL_LEVEL, getLevel());
#endif
}

list<Implicant> DnfCircuit::getMintermList()
{
	list<Implicant> mintermList;
	list<Implicant> implicantList(getImplicantList());
	for(list<Implicant>::iterator implicant(implicantList.begin()); implicant!=implicantList.end(); ++implicant)
	{
		if(implicant->valid)	//ha azonosan hamis, akkor egyszerûen elhagyhatjuk
		{
			list<Implicant> listToAdd(implicant->toMintermList());
			mintermList.splice(mintermList.end(), listToAdd);
		}
	}
	return mintermList;
}

list<Implicant> DnfCircuit::getImplicantList()
{
	list<Gate*> clauseList;
	listClausesInSubgraph(*output, clauseList);
	list<Implicant> implicantList;
#ifdef VS2010
	for each(Gate* clause in clauseList)
#else
	for(Gate* clause : clauseList)
#endif
	{
		Implicant newImplicant(getImplicantFromSubgraph(*clause));
		if(newImplicant.valid)
		{
			implicantList.push_back(newImplicant);	//TODO: így ImplicantList csak valid értékeket tartalmaz, máshonnan az ellenõrzéseket ki lehet venni
		}
	}
	return implicantList;
}

FILE* DnfCircuit::toPlaFormat()	throw (PlaFormatException)
{

	list<Implicant> impList = getImplicantList();
	if (impList.empty()) {throw PlaFormatException();}
	FILE* plaFile = fopen("input.tmp", "wb+");
	fprintf(plaFile, ".i %u .o 1\n", /*impList.empty() ? 0 :*/ impList.front().numOfVariables );
#ifdef VS2010
	for each(const Implicant& i in impList)
#else
	for(Implicant& i : impList)
#endif
	{
		if(!i.valid)
		{
			continue;
		}
#ifdef VS2010
		for each(int j in i.values)
#else
		for(int j : i.values)
#endif
		{
			switch (j)
			{
			case Implicant::TRUE:
				fprintf(plaFile, "1");
				break;
			case Implicant::FALSE:
				fprintf(plaFile, "0");
				break;
			case Implicant::DONT_CARE:
				fprintf(plaFile, "-");
				break;
			}
		}
		fprintf(plaFile, " 1\n");
	}
	fprintf(plaFile, ".e\n");
	fflush(plaFile);
	rewind(plaFile);
	return plaFile;
}

void DnfCircuit::readPlaFormat(FILE* plaFile)
{
	list<Implicant> implicantList;
	unsigned int numOfVariables, numOfImplicants;
	fscanf(plaFile, ".i %u\n.o 1\n.p %u\n", &numOfVariables, &numOfImplicants);
	for (unsigned int i=0; i<numOfImplicants; i++)
	{
		vector<int> values(numOfVariables, Implicant::DONT_CARE);
		for(unsigned int j=0; j<numOfVariables; j++)
		{
			char c = fgetc(plaFile);
			switch(c)
			{
			case '0':
				values[j] = Implicant::FALSE;
				break;
			case '1':
				values[j] = Implicant::TRUE;
				break;
			}
		}
		fscanf(plaFile, " 1\n");
		implicantList.push_back(Implicant(values));
		//output->deleteRecursively();
		output = buildFromImplicantList(implicantList);
	}
}

void DnfCircuit::minimizeEspresso()
{
	try
	{
		
#ifdef LOG_MAX_GATENUM
	Logger::getLogger().setData(Logger::STARTING_GATENUM, Gate::constrCall-Gate::destrCall);
#endif
#ifdef LOG_SIZE
		Logger::getLogger().setData(Logger::DNF_SIZE, getSize());
#endif
#ifdef LOG_TIME
		Logger::getLogger().setTime(Logger::DNF_TIME);
#endif
		FILE* inputFile = this->toPlaFormat();
		FILE* outputFile = fopen("result.tmp", "wb+");
#ifdef LOG_TIME
		Logger::getLogger().updateTimer();
#endif
		espresso_main(inputFile, outputFile);
		rewind(outputFile);
		this->readPlaFormat(outputFile);
		fclose(inputFile);
		fclose(outputFile);
		remove("input.tmp");
		remove("result.tmp");
	}
	catch(PlaFormatException)
	{
		output = make_shared<FalseGate>();
	}
	
#ifdef LOG_TIME
		Logger::getLogger().setTime(Logger::FINISH_TIME);
#endif
#ifdef LOG_SIZE
		Logger::getLogger().setData(Logger::FINAL_SIZE, getSize());
#endif
#ifdef LOG_LEVEL
		Logger::getLogger().setData(Logger::FINAL_LEVEL, getLevel());
#endif
}