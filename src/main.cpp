#include "Circuit.h"
#include "DnfCircuit.h"
#include "AndGate.h"
#include "OrGate.h"
#include "NotGate.h"
#include "Logger.h"

#include <iostream>
#include <ctime>

using namespace std;

int main(int argc, char** argv)
{
	/*bool a = true;
	time_t seed = 0;
	while(a)
	{
	//srand((unsigned int)time(&seed));
	//srand(seed = 1370629149);
	srand(++seed);
	cout <<"Random seed: "<<seed<<endl;/*
	
	//Circuit halozat2(2, 3, true);
	Circuit halozat1(5, 12, true);
	DnfCircuit dnf1(halozat1);
	//cout << halozat1.toStringInfix() << endl;
	//cout << dnf1.toStringInfix() << endl;
	DnfCircuit dnf2(dnf1);
	a = (halozat1 == dnf2);
	halozat1.simplify();
	cout << "simplify done" << endl;
	dnf1.minimizeEspresso();
	cout << "espresso done" << endl;
	dnf2.minimizeQuineMcClusky();
	cout << "QM done" << endl;
	a = (halozat1 == dnf1) && (halozat1 == dnf2);
	cout << a << endl;*/

	{
	time_t seed;
	srand((unsigned int)time(&seed));
	cout <<"Random seed: "<<seed<<endl;

	Circuit original(6, 50, false);
	cout << original.toStringInfix() << endl;

	Circuit circ1(original);
	Logger::resetEntry();
	Logger::currentEntry.startingSize = original.getSize();
	Logger::currentEntry.startTime = clock();
	circ1.simplify();
	Logger::currentEntry.finishTime = clock();
	Logger::currentEntry.finalSize = circ1.getSize();
	Logger::showStats1();

	Logger::currentEntry.startingSize = original.getSize();
	Logger::currentEntry.startTime = clock();
	DnfCircuit dnf1(original);
	Logger::currentEntry.dnfTime = clock();
	Logger::currentEntry.dnfSize = dnf1.getSize();
	dnf1.minimizeEspresso();
	Logger::currentEntry.finishTime = clock();
	Logger::currentEntry.finalSize = dnf1.getSize();
	Logger::showStats1();
	
	Logger::resetEntry();
	Logger::currentEntry.startingSize = original.getSize();
	Logger::currentEntry.startTime = clock();
	DnfCircuit dnf2(original);
	Logger::currentEntry.dnfTime = clock();
	Logger::currentEntry.dnfSize = dnf2.getSize();
	dnf2.minimizeQuineMcClusky();
	Logger::currentEntry.finishTime = clock();
	Logger::currentEntry.finalSize = dnf2.getSize();
	Logger::showStats1();


	}
	//cout << Gate::constrCall<<" "<<Gate::destrCall<<endl;
	system("PAUSE");
}