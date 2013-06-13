#ifndef LOGGER
#define LOGGER

#include <ctime>

class Logger
{
public:
	struct Entry
	{
		clock_t startTime;
		clock_t dnfTime;
		clock_t inputConversionTime;
		clock_t finishTime;
		unsigned int startingSize;
		unsigned int dnfSize;
		unsigned int finalSize;

		Entry():
			startTime(0),
			dnfTime(0),
			inputConversionTime(0),
			finishTime(0),
			startingSize(0),
			dnfSize(0),
			finalSize(0)
		{}
	};
	static Entry currentEntry;

	static void resetEntry();
	static void showStats1();
};

#endif