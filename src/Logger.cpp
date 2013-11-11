#include "Logger.h"

#include <iostream>
#include <fstream>

//Logger::Entry Logger::currentEntry;
using namespace std;

Logger::Logger():
output(&cout)
{
	startEntry("");
}

Logger& Logger::getLogger()
{
	static Logger loggerInstance;
	return loggerInstance;
}

void Logger::setOutput(std::ostream& output)
{
	this->output = &output;
}

void Logger::updateTimer()
{
	lastTime = clock();
}

void Logger::setTime(DataField field)
{
	long prev = lastTime;
	lastTime = clock();
	currentEntry.data[field] = lastTime-prev;
}

void Logger::setData(DataField field, long val)
{
	currentEntry.data[field] = val;
}
	
void Logger::setMaxGates(long val)
{
	if(val>currentEntry.data[MAX_GATENUM])
	{
		currentEntry.data[MAX_GATENUM] = val;
	}
}

void Logger::startEntry(std::string name)
{
	currentEntry = Entry();
	currentEntry.name = name;
}

void Logger::storeEntry()
{
	entries.push_back(currentEntry);
}

void Logger::printStats()
{
	(*output) <<'\t';
#ifdef VS2010
	for each(Entry entry in entries)
#else
	for(Entry entry : entries)
#endif
	{
		(*output) <<"\t\t"<<entry.name;
	}
	(*output) << endl;
	
	for(int i=DNF_TIME; i<=FINISH_TIME; i++)
	{
		(*output) << fieldIdToString(i);
#ifdef VS2010
		for each(Entry entry in entries)
#else
		for(Entry entry : entries)
#endif
		{
			(*output) <<"\t\t"<<entry.data[i]/(double)CLOCKS_PER_SEC;
		}
		(*output) << endl;
	}
	for(int i=STARTING_SIZE; i<COUNT_OF_FIELDS; i++)
	{
		(*output) << fieldIdToString(i);
#ifdef VS2010
		for each(Entry entry in entries)
#else
		for(Entry entry : entries)
#endif
		{
			(*output) <<"\t\t"<<entry.data[i];
		}
		(*output) << endl;
	}
	(*output).flush();
}

string Logger::fieldIdToString(int id)
{
	switch(id)
	{
	case DNF_TIME:
		return "DNF time";
	case FINISH_TIME:
		return "Finish time";
	case STARTING_SIZE:
		return "Starting size";
	case STARTING_LEVEL:
		return "Starting level";
	case DNF_SIZE:
		return "DNF size";
	case FINAL_SIZE:
		return "Final size";
	case FINAL_LEVEL:
		return "Final level";
	case STARTING_GATENUM:
		return "Start gate cnt";
	case MAX_GATENUM:
		return "Max gate count";
	default:
		return NULL;
	}
}

void Logger::clearEntries()
{
	entries.clear();
}

void Logger::printCsvHeader()
{
#ifdef VS2010
	for each(Entry entry in entries)
#else
	for(Entry entry : entries)
#endif
	{
		for(int i=DNF_TIME; i<COUNT_OF_FIELDS; i++)
		{
			(*output) << entry.name <<" "<< fieldIdToString(i)<<',';
			output->flush();
		}
	}
	(*output).seekp((std::streamoff)(*output).tellp()-1);
	(*output) << endl;
	output->flush();
}

void Logger::printCsvLine()
{
#ifdef VS2010
	for each(Entry entry in entries)
#else
	for(Entry entry : entries)
#endif
	{
		for(int i=DNF_TIME; i<=FINISH_TIME; i++)
		{
			(*output) << entry.data[i]/(double)CLOCKS_PER_SEC <<',';
		}
		for(int i=STARTING_SIZE; i<COUNT_OF_FIELDS; i++)
		{
			(*output) << entry.data[i] <<',';
		}
	}
	(*output).seekp((std::streamoff)(*output).tellp()-1);
	(*output) << endl;
	output->flush();
}