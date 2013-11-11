#ifndef LOGGER
#define LOGGER

#ifdef LOG_ALL
#define LOG_TIME
#define LOG_SIZE
#define LOG_LEVEL
#define LOG_MAX_GATENUM
#endif

#include <ctime>
#include <string>
#include <list>

class Logger
{
public:
	enum DataField{ DNF_TIME=0, FINISH_TIME, STARTING_SIZE, DNF_SIZE, FINAL_SIZE, STARTING_LEVEL, FINAL_LEVEL, STARTING_GATENUM, MAX_GATENUM ,COUNT_OF_FIELDS};

	struct Entry
	{
		std::string name;
		long data[COUNT_OF_FIELDS];
	};

private:
	Entry currentEntry;
	std::list<Entry> entries;
	clock_t lastTime;
	std::ostream* output;
	
	Logger();
    Logger(Logger const&);	//hogy ne lehessen másolni 
    void operator=(Logger const&);	//hogy ne lehessen másolni 
	std::string fieldIdToString(int id);
public:
	static Logger& getLogger();	//singleton
	void setOutput(std::ostream& output);
	void updateTimer();
	void setTime(DataField field);
	void setData(DataField field, long val);
	void setMaxGates(long val);

	void startEntry(std::string name);
	void storeEntry();
	void printStats();
	void clearEntries();

	void printCsvHeader();
	void printCsvLine();
};

#endif