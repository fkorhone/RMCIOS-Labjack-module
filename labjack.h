/*
 Function pointer typedefs for calling labjack library functions for u12 DAQs.
*/

#ifndef labjack_h
#define labjack_h

#include <wtypes.h>

typedef long (CALLBACK *tEAnalogIn)(long*,long,long,long,long*,float*);
typedef long (CALLBACK *tEAnalogOut)(long*,long,float,float);
typedef long (CALLBACK *tECount)(long*,long,double*);
typedef long (CALLBACK *tEDigitalIn)(long*,long,long,long,long*);
typedef long (CALLBACK *tEDigitalOut)(long*,long,long,long,long);
typedef long (CALLBACK *tAISample)(long*,long,long*,long,long,long,long*,long*,long,long*,float*);
typedef long (CALLBACK *tAIBurst)(long*,long,long*,long,long,long,long*,long*,long*,long,long,long,long,long,float**,long*,long*,long);
typedef long (CALLBACK *tAIStreamStart)(long*,long,long*,long,long,long,long*,long*,long*,long,long,long);
typedef long (CALLBACK *tAIStreamRead)(long,long,long,float**,long*,long*,long*,long*);
typedef long (CALLBACK *tAOUpdate)(long*,long,long,long,long*,long*,long,long,unsigned long*,float,float);
typedef long (CALLBACK *tAIStreamClear)(long);
typedef long (CALLBACK *tAsynchConfig)(long*,long,long,long,long,long,long,long,long,long,long,long);
typedef long (CALLBACK *tAsynch)(long*,long,long,long,long,long,long,long,long,long*);
typedef long (CALLBACK *tBitstoVolts)(long,long,long,float*);
typedef long (CALLBACK *tVoltstoBits)(long,long,float,long*);
typedef long (CALLBACK *tCounter)(long*,long,long*,long*,long,long,unsigned long*);
typedef long (CALLBACK *tDigitalIO)(long*,long,long*,long,long*,long*,long,long*);
typedef float (CALLBACK *tGetDriverVersion)(void);
typedef void (CALLBACK *tGetErrorString)(long errorcode, char *errorString);
typedef float (CALLBACK *tGetFirmwareVersion)(long *idnum);
typedef long (CALLBACK *tGetWinVersion)(unsigned long*,unsigned long*,unsigned long*,unsigned long*,unsigned long*,unsigned long*);
typedef long (CALLBACK *tListAll)(long*,long*,long*,long*,long**,long*,long*,long*);
typedef long (CALLBACK *tLocalID)(long*,long);
typedef long (CALLBACK *tNoThread)(long*,long);
typedef long (CALLBACK *tPulseOut)(long*,long,long,long,long,long,long,long,long);
typedef long (CALLBACK *tPulseOutStart)(long*,long,long,long,long,long,long,long,long);
typedef long (CALLBACK *tPulseOutCalc)(float*,long*,long*);
typedef long (CALLBACK *tPulseOutFinish)(long*,long,long);
typedef long (CALLBACK *tPulseOutCalc)(float*,long*,long*);
typedef long (CALLBACK *tReEnum)(long*);
typedef long (CALLBACK *tReset)(long*) ;

#endif


