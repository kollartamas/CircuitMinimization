#ifndef HALOZAT_H
#define HALOZAT_H

#include <vector>
#include <string>
#include <functional>

#include "Input.h"
#include "Kapu.h"

class Halozat
{
private:
	std::vector<Input*> inputok;
	Kapu* output;
	void doForEachGate(std::function<void(Kapu*)> funct);	//minden kapura hívja a megadott fuggvenyt

	unsigned int calculateInfixLength();
	unsigned int calculatePrefixLength();
	unsigned int calculatePostfixLength();

public:
	Halozat(unsigned int inputokSzama);
	Halozat(unsigned int inputokSzama, unsigned int kapukSzama);
	~Halozat();

	Input& operator[](unsigned int sorszam);
	void setOutput(Kapu* out);
	
	bool kiertekel();

	std::string toStringInfix();
	std::string toStringPrefix();
	std::string toStringPostfix();

};

#endif