#ifndef CIRCUIT_WITHOUT_NOT_GATE_H
#define CIRCUIT_WITHOUT_NOT_GATE_H

#include "Circuit.h"

class CircuitWithoutNotGate : public Circuit
{
	public:
		CircuitWithoutNotGate(Circuit& other);
		CircuitWithoutNotGate(CircuitWithoutNotGate& other);
		CircuitWithoutNotGate(unsigned int inputokSzama);
		
		void toDnf();
		void reorderGates();
		void convertToSingleOptimized();
		void updateOccurrences(MultiGate::occurrenceIterator iter);
		void simplify();
		void simplifyIterative();
		void simplify2();
		void simplifyIterative2();
		void simplify3();
		void simplifyIterative3();
		void factorize();
		void factorizeIteratively();
		void optimizeIteratively();

};

#endif