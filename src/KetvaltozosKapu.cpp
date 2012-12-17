#include "KetvaltozosKapu.h"
#include "Kapu.h"


KetvaltozosKapu::KetvaltozosKapu(Kapu& in1, Kapu& in2)
{
	inputs.push_back(&in1);
	in1.addOutput(this);
	inputs.push_back(&in2);
	in2.addOutput(this);
}

void KetvaltozosKapu::calculateInfixLength()
{
	stringLength = inputs[0]->getStringLength() + 3 + inputs[1]->getStringLength();
}

void KetvaltozosKapu::calculatePrefixLength()
{
	stringLength = 2 + inputs[0]->getStringLength() + inputs[1]->getStringLength();
}