/* LINTLIBRARY */
#include "copyright.h"
#include "port.h"
#include "utility.h"

// modified by alanmi - Dec 18, 2001 

/*
 *   util_cpu_time -- return a long which represents the elapsed processor
 *   time in milliseconds since some constant reference
 */
long 
util_cpu_time()
{
	return clock();
}
