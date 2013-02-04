#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <cstdlib>

#include "Halozat.h"
#include "Kapu.h"
#include "Input.h"
#include "NotGate.h"
#include "AndGate.h"
#include "OrGate.h"

#include "Quine_McClusky\ImplicantChart.h"

using namespace std;

Halozat::Halozat(unsigned int inputokSzama)
{
	inputok = vector<Input*>(inputokSzama);
	for(unsigned int i=0; i<inputokSzama; i++)
	{
		inputok[i] = new Input(i);
	}
}

Halozat::Halozat(unsigned int inputokSzama, unsigned int kapukSzama)
{
	vector<Kapu*> osszesElem;
	osszesElem.reserve(inputokSzama + kapukSzama);
	set<Kapu*> zsakutcak;


	inputok = vector<Input*>(inputokSzama);
	for(unsigned int i=0; i<inputokSzama; i++)
	{
		Input* ujInput = new Input(i);
		inputok[i] = ujInput;
		osszesElem.push_back(ujInput);
		zsakutcak.insert(ujInput);
	}

	//generalunk veletlenszeru kapukat, amik inputkent a korabbi elemeket kapjak
	//elso szakaszban szabadon valaszthatunk:
	for(unsigned int i=0; kapukSzama >= i+zsakutcak.size(); i++)
	{

		Kapu *ujKapu, *inp1, *inp2;
		int kapuTipus = rand()%3;

		switch(kapuTipus)
		{
		case 0:
			inp1 = osszesElem[rand()%osszesElem.size()];
			ujKapu = new NotGate(*inp1);
			zsakutcak.erase(inp1);
			zsakutcak.insert(ujKapu);
			osszesElem.push_back(ujKapu);
			break;
		case 1:
			inp1 = osszesElem[rand()%osszesElem.size()];
			inp2 = osszesElem[rand()%osszesElem.size()];
			ujKapu = new AndGate(*inp1, *inp2);
			zsakutcak.erase(inp1);
			zsakutcak.erase(inp2);
			zsakutcak.insert(ujKapu);
			osszesElem.push_back(ujKapu);
			break;
		case 2:
			inp1 = osszesElem[rand()%osszesElem.size()];
			inp2 = osszesElem[rand()%osszesElem.size()];
			ujKapu = new OrGate(*inp1, *inp2);
			zsakutcak.erase(inp1);
			zsakutcak.erase(inp2);
			zsakutcak.insert(ujKapu);
			osszesElem.push_back(ujKapu);
			break;
		}
	}
	//masodik szakaszban ugy valasztunk, hogy fogyjon az output nelkuli kapuk szama:
	map<int, Kapu*> veletlenRendezett;
	//mindhez rendelunk egy veletlen int-et, ez lesz a kulcs ami szerint rendezzuk
	for(set<Kapu*>::iterator iter=zsakutcak.begin(); iter!=zsakutcak.end(); iter++)
	{
		veletlenRendezett.insert(pair<int, Kapu*>(rand(), *iter));
	}
	while(veletlenRendezett.size()>1)
	{
		Kapu *ujKapu, *inp1, *inp2;

		map<int, Kapu*>::iterator iter(veletlenRendezett.begin());
		inp1 = (iter->second);
		veletlenRendezett.erase(iter);
		iter = veletlenRendezett.begin();
		inp2 = (iter->second);
		veletlenRendezett.erase(iter);

		if(rand()%2)
		{
			ujKapu = new AndGate(*inp1, *inp2);
		}
		else
		{
			ujKapu = new OrGate(*inp1, *inp2);
		}

		veletlenRendezett.insert(pair<int, Kapu*>(rand(), ujKapu));
	}
	output = veletlenRendezett.begin()->second;
}

void Halozat::doForEachGate( function<void(Kapu*)> funct )
{
	queue<Kapu*> ertekelheto;

	//betesszük az inputokat a queue-ba
	for(unsigned int i=0; i<inputok.size(); i++)
	{
		ertekelheto.push(inputok[i]);
	}

	//olyan sorrendben végezzük a bejárást, hogy minden kapu elõtt bejárjuk az inputjait
	while(!ertekelheto.empty())
	{
		Kapu* p_aktualisKapu = ertekelheto.front();
		ertekelheto.pop();

		list<Kapu*> outs = p_aktualisKapu->getOutputs();
		funct(p_aktualisKapu);

		//for (Kapu* p_out : outs )		// c++11 standard
		for each(Kapu* p_out in outs )	//VS2010 syntax
		{
			if(p_out->incrementElozoekCount(true))
			{
				ertekelheto.push(p_out);
			}
		}
	}
}

void Halozat::doForEachGateReverse( function<void(Kapu*)> funct )
{
	queue<Kapu*> ertekelheto;

	ertekelheto.push(output);

	//olyan sorrendben végezzük a bejárást, hogy minden kapu elõtt bejárjuk az outputjait
	while(!ertekelheto.empty())
	{
		Kapu* p_aktualisKapu = ertekelheto.front();
		ertekelheto.pop();

		vector<Kapu*> ins = p_aktualisKapu->inputs;
		funct(p_aktualisKapu);

		//for (Kapu* p_out : outs )		// c++11 standard
		for each(Kapu* p_in in ins )	//VS2010 syntax
		{
			if(p_in->incrementElozoekCount(false))
			{
				ertekelheto.push(p_in);
			}
		}
	}
}

Halozat::~Halozat()
{
	doForEachGate([] (Kapu* kapu) {delete kapu;});
}

Input& Halozat::operator[](unsigned int sorszam)
{
	return *inputok[sorszam];
}

void Halozat::setOutput(Kapu* out)
{
	this->output = out;
}

bool Halozat::kiertekel()
{
	doForEachGate([](Kapu* kapu)->void{kapu->erteketFrissit();});
	return output->getValue();
}


unsigned int Halozat::calculateInfixLength()
{
	doForEachGate([](Kapu* kapu)->void{kapu->calculateInfixLength();});
	return output->getStringLength();
}

unsigned int Halozat::calculatePrefixLength()
{
	doForEachGate([](Kapu* kapu)->void{kapu->calculatePrefixLength();});
	return output->getStringLength();
}
unsigned int Halozat::calculatePostfixLength()
{
	return calculatePrefixLength();
}


std::string Halozat::toStringInfix()
{
	string formula;
//unsigned int a=5;
	formula.reserve(calculateInfixLength());
//unsigned int b=7;
	output->addToStringInfix(formula);
	return formula;
}

std::string Halozat::toStringPrefix()
{
	string formula;
	formula.reserve(calculatePrefixLength());
	output->addToStringPrefix(formula);
	return formula;
}

std::string Halozat::toStringPostfix()
{
	string formula;
	formula.reserve(calculatePostfixLength());
	output->addToStringPostfix(formula);
	return formula;
}

void Halozat::removeNotGates()
{
	list<NotGate*> notGates;

	doForEachGate( [](Kapu* kapu){kapu->createNegatedTwin();} );
	
	doForEachGate([](Kapu* kapu){kapu->removeNotGate();} );
	if(output->getTipus() == Kapu::NOT)
	{
		output = output->getTwin()->getTwin();
	}
	removeExtraGates();
	doForEachGate([](Kapu* kapu){kapu->twin=NULL;} );
}

void Halozat::removeExtraGates()
{
	stack<Kapu*> ertekelheto;

	ertekelheto.push(output);

	while(!ertekelheto.empty())
	{
		Kapu* p_aktualisKapu = ertekelheto.top();
		ertekelheto.pop();

		vector<Kapu*> ins = p_aktualisKapu->inputs;
		p_aktualisKapu->marked = true;

		//for (Kapu* p_out : outs )		// c++11 standard
		for each(Kapu* p_in in ins )	//VS2010 syntax
		{
			if(!p_in->marked)
			{
				ertekelheto.push(p_in);
			}
		}
	}

	doForEachGate([](Kapu* kapu){kapu->removeIfNotMarked();});
	doForEachGate([](Kapu* kapu){kapu->marked = false;});


}

void Halozat::minimizeQuineMcClusky()
{
	ImplicantChart implicantChart;
	for each(Implicant implicant in getMintermList())
	{
		implicantChart.addMinterm(implicant, true);
	}
	doForEachGate([](Kapu* kapu)
	{
		if(kapu->tipus!=Kapu::INPUT)
		{
			delete kapu;
		}
		else
		{
			kapu->outputs.clear();
		}
	});

	setOutput( buildFromImplicantList(implicantChart.getMinimizedDNF()) );
}

list<Implicant> Halozat::getMintermList()
{
	list<Implicant> mintermList;
	for each(Implicant implicant in getImplicantList())
	{
		list<Implicant> listToAdd(implicant.toMintermList());
		mintermList.splice(mintermList.end(), listToAdd);
	}
	return mintermList;
}


list<Implicant> Halozat::getImplicantList()
{
	list<Kapu*> clauseList;
	listClausesInSubgraph(*output, clauseList);
	list<Implicant> implicantList;
	for each(Kapu* clause in clauseList)
	{
		implicantList.push_back(getImplicantFromSubgraph(*clause));
	}
	return implicantList;
}

void Halozat::listClausesInSubgraph(Kapu& rootGate, list<Kapu*>& clauseList)
{
	for each(Kapu* inputGate in rootGate.inputs)
	{
		if(inputGate->tipus == Kapu::OR)
		{
			listClausesInSubgraph(*inputGate, clauseList);
		}
		else
		{
			clauseList.push_back(inputGate);
		}
	}
}


Implicant Halozat::getImplicantFromSubgraph(Kapu& rootGate)
{
	vector<int> values(inputok.size(), Implicant::DONT_CARE);
	collectInputsFromSubgraph(rootGate, values);
	return Implicant(values);
}

void Halozat::collectInputsFromSubgraph(Kapu& rootGate, std::vector<int>& currentValues)
{
	for each(Kapu* inputGate in rootGate.inputs)
	{
		switch(inputGate->tipus)
		{
			case Kapu::AND:
				collectInputsFromSubgraph(*inputGate, currentValues);
				break;
			case Kapu::INPUT:
				currentValues[((Input*)inputGate)->id] &= Implicant::TRUE;	//bitwise, mert True:01b False:10b DontCare:11b
				break;
			case Kapu::NOT:
				currentValues[((Input*)(inputGate->inputs[0]))->id] &= Implicant::FALSE;
				break;
			/*case Kapu::TRUE:
				break;
			case Kapu::FALSE:
				if(currentValues.empty()){currentValues.push_back(Implicant::INVALID);}
				else{currentValues[0]=Implicant::INVALID;}
				break;*/
			default:
				throw exception("DNF error");
		}
	}
}


Kapu* Halozat::buildFromImplicantList(list<Implicant>& implicantList)
{
	if(implicantList.empty())
	{
//		return new FalseGate());			TODO
	}

	list<Implicant>::iterator iter(implicantList.begin());
	Kapu* topClause = buildFromImplicant(*iter);
	++iter;
	for(; iter!=implicantList.end(); ++iter)
	{
		topClause = new OrGate(*topClause, *buildFromImplicant(*iter));
	}
	return topClause;
}

Kapu* Halozat::buildFromImplicant(Implicant& implicant)
{
	if(implicant.numOfVariables==implicant.numOfDontCares)
	{
//		return new TrueGate;		TODO
	}

	Kapu* topLiteral;
	unsigned int i=0;
	for(; i<implicant.values.size(); i++)
	{
		switch(implicant.values[i])
		{
			case Implicant::TRUE:
				topLiteral = inputok[i];
				break;
			case Implicant::FALSE:
				topLiteral = new NotGate(*inputok[i]);
				break;
			/*case Implicant::INVALID:
				topLiteral = new FalseGate();		TODO
				break;*/
			default:
				continue;
		}
		i++;
		break;			//TRUE/FALSE esetén break(a ciklusból), különben continue;
	}

	for(; i<implicant.values.size(); i++)
	{
		switch(implicant.values[i])
		{
			case Implicant::TRUE:
				topLiteral = new AndGate(*inputok[i], *topLiteral);
				break;
			case Implicant::FALSE:
				topLiteral = new AndGate(*new NotGate(*inputok[i]), *topLiteral);
				break;
			/*case Implicant::INVALID:
				topLiteral = new AndGate(*new FalseGate(), *topLiteral);		TODO
				break;*/
		}
	}
	return topLiteral;
}
