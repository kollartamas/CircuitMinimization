#include "Logger.h"

#include <iostream>

Logger::Entry Logger::currentEntry;

void Logger::resetEntry()
{
	currentEntry.startTime = 0;
	currentEntry.dnfTime = 0;
	currentEntry.inputConversionTime = 0;
	currentEntry.finishTime = 0;
	currentEntry.startingSize = 0;
	currentEntry.dnfSize = 0;
	currentEntry.finalSize = 0;
}


void Logger::showStats1()
{
	std::cout <<"Total time(ticks): "<<currentEntry.finishTime-currentEntry.startTime<<" Number of gates(start, dnf, final): ";
	std::cout <<currentEntry.startingSize<<", "<<currentEntry.dnfSize<<", "<<currentEntry.finalSize<<std::endl;
}