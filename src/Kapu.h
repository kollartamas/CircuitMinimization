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

	unsigned int elozoekCount;	//h�ny (k�zvetlen�l) megel�z� kaput j�rtunk m�r be
	unsigned int stringLength;	//csak calculate...Length ut�n t�rolja a val�di �rt�ket
public:
	Kapu();

	//getter/setter:
	bool getValue();
	int getTipus();
	unsigned int getStringLength();
	void addOutput(Kapu* out);
	const std::list<Kapu*> getOutputs();
	
	
	virtual void erteketFrissit() =0;	//ki�rt�kel�shez
	bool incrementElozoekCount(bool iranyElore);	//inkrement�l, return true ha el�ri inputs/outputs.lenght-et

	//stringes ki�rat�shoz haszn�lt fgv-ek
	virtual void addToStringInfix(std::string& dest) =0;
	virtual void addToStringPrefix(std::string& dest) =0;
	virtual void addToStringPostfix(std::string& dest) =0;
	virtual void calculateInfixLength() =0;
	virtual void calculatePrefixLength() =0;
};

#endif