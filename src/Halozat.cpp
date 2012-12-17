#include <vector>
#include <queue>
#include <set>
#include <map>
#include <cstdlib>

#include "Halozat.h"
#include "Kapu.h"
#include "Input.h"
#include "NotGate.h"
#include "AndGate.h"
#include "OrGate.h"

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
		for each(Kapu* p_out in outs )
		{
			if(p_out->incrementElozoekCount(true))
			{
				ertekelheto.push(p_out);
			}
		}
	}
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
unsigned int a=5;
	formula.reserve( a=calculateInfixLength());
unsigned int b=7;
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