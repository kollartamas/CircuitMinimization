#include "Circuit.h"
#include "DnfCircuit.h"
#include "AndGate.h"
#include "OrGate.h"
#include "NotGate.h"
#include "Logger.h"
#include "Link.h"

#include "main.h"

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

int main(int argc, char** argv)
{
	Logger& logger(Logger::getLogger());
	/*ofstream logfile("log.txt", ios::out);		//logger outputot fájlba
	logger.setOutput(logfile);*/

	//abc inicializálása
	abc::Abc_Start();
	abc::Abc_Frame_t* pFrame = abc::Abc_FrameGetGlobalFrame();
	abc::Cmd_CommandExecute( pFrame, string("source abc.rc").c_str() );

	time_t seed=0;
	time(&seed);
	cout <<"Random seed: "<<seed<<endl;
	srand(seed++);

	for(int i=0; i<50; i++)
	{
		cout <<"Circuit number: "<< i <<endl;
		Circuit original(20, 100, false);

		//Saját alg. implementáció:
		logger.startEntry("Sajat");
		CircuitWithoutNotGate circ1(original);
		circ1.optimizeIteratively();
		logger.storeEntry();
		//circ1.toStringInfix();		//kiírás formulaként
		//circ1.printStructure();		//a hálózat felépítését írja ki

		//Abc:
		logger.startEntry("Abc");
		Circuit circ2(original);
		circ2.optimizeAbc();
		logger.storeEntry();
		logger.printStats();

		//Espresso:
		logger.startEntry("Espresso");
		logger.setData(Logger::STARTING_SIZE, original.getSize());
		logger.setData(Logger::STARTING_LEVEL, original.getLevel());
		logger.updateTimer();
		DnfCircuit circ3(original);
		circ3.minimizeEspresso();
		logger.storeEntry();
		//logger.printStats();

		//Quine-McClusky:
		/*logger.startEntry("QMC");
		logger.setData(Logger::STARTING_SIZE, original.getSize());
		logger.setData(Logger::STARTING_LEVEL, original.getLevel());
		logger.updateTimer();
		DnfCircuit circ4(original);
		circ4.minimizeQuineMcClusky();
		logger.storeEntry();*/

		logger.printStats();
		
		/*if(i==0){logger.printCsvHeader();}			//CSV formátumú loghoz
		logger.printCsvLine();*/

		logger.clearEntries();
		//if(!(original==circ1)) {system("PAUSE"); break;}
		//if(!(original==circ2)) {system("PAUSE"); break;}
		//if(!(original==circ3)) {system("PAUSE"); break;}
		//if(!(original==circ4)) {system("PAUSE"); break;}
	}
	abc::Abc_Stop();
	system("PAUSE");
}
