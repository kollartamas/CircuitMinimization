#ifndef KAPU_H
#define KAPU_H

#include <vector>
#include <list>
#include <string>

class Kapu
{
protected:
	enum tipusok {INPUT=0, NOT=1, AND=2, OR=3};

	std::vector<Kapu*> inputs;
	std::list<Kapu*> outputs;
	bool value;
	int tipus;

	unsigned int elozoekCount;	//hány (közvetlenül) megelõzõ kaput jártunk már be
	unsigned int stringLength;	//csak calculate...Length után tárolja a valódi értéket
public:
	Kapu();

	//getter/setter:
	bool getValue();
	int getTipus();
	unsigned int getStringLength();
	void addOutput(Kapu* out);
	const std::list<Kapu*> getOutputs();
	
	
	virtual void erteketFrissit() =0;	//kiértékeléshez
	bool incrementElozoekCount(bool iranyElore);	//inkrementál, return true ha eléri inputs/outputs.lenght-et

	//stringes kiíratáshoz használt fgv-ek
	virtual void addToStringInfix(std::string& dest) =0;
	virtual void addToStringPrefix(std::string& dest) =0;
	virtual void addToStringPostfix(std::string& dest) =0;
	virtual void calculateInfixLength() =0;
	virtual void calculatePrefixLength() =0;
};

#endif