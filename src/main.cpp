#include "Halozat.h"
#include "Kapu.h"
#include "AndGate.h"
#include "OrGate.h"
#include "NotGate.h"

#include <iostream>
#include <ctime>

using namespace std;

int main()
{
	srand((unsigned int)time(NULL));

	//manuálisan felépítve:
	//Halozat halozat(6);

	Kapu *egyik, *masik;
	/*egyik = new OrGate(halozat[0], halozat[1]);
	masik = new OrGate(halozat[2], halozat[3]);

	egyik = new OrGate(*egyik, *masik);
	masik = new OrGate(halozat[4], halozat[5]);
	masik = new AndGate(*egyik, *masik);
	masik = new NotGate(*masik);
	egyik = new AndGate(*egyik, *masik);

	halozat.setOutput(egyik);*/

	Halozat halozat(4);
	/*egyik = new OrGate(halozat[0], halozat[1]);
	masik = new AndGate(halozat[1], halozat[2]);
	egyik = new AndGate(*egyik, *masik);
	egyik = new NotGate(*egyik);
	halozat.setOutput(egyik);*/

	egyik = new AndGate(halozat[0], halozat[2]);
	masik = new AndGate(halozat[2], *new NotGate(halozat[3]));
	egyik = new OrGate(*egyik, *masik);

	masik = new AndGate(*new NotGate(halozat[0]), *new NotGate(halozat[2]));
	egyik =new OrGate(*egyik, *masik);

	masik = new AndGate(*new NotGate(halozat[2]), halozat[3]);
	egyik =new OrGate(*egyik, *masik);
	halozat.setOutput(egyik);

	cout << halozat.toStringInfix() << endl;

	halozat.minimizeQuineMcClusky();
	cout << halozat.toStringInfix() << endl;
	//halozat.removeNotGates();
	/*cout << halozat.toStringPrefix() << endl;
	cout << halozat.toStringPostfix() << endl <<endl;*/

	/*//véletlenszerûen:
	Halozat halozat2(5, 10);
	cout << halozat2.toStringInfix() << endl;
	cout << halozat2.kiertekel() << endl;

	//halozat2.removeNotGates();
	cout << halozat2.toStringInfix() << endl;*/



	system("PAUSE");
}