// Minimal header for required ljm functions
#ifndef LAB_JACK_M_HEADER
#define LAB_JACK_M_HEADER

#ifdef WIN32
   #define CONV __stdcall
#endif

#define LJM_STRING_ALLOCATION_SIZE 50
#define LJM_UINT16                 0
#define LJM_UINT32                 1
#define LJM_INT32                  2
#define LJM_FLOAT32                3
#define LJM_STRING                 98
#define LJM_BYTE                   99

int CONV LJM_OpenS(const char *, const char *, const char *, int *);
int CONV LJM_NameToAddress(const char *, int *, int *);
int CONV LJM_AddressToType(int, int *);
int CONV LJM_eReadAddressString(int, int, char *);
int CONV LJM_eWriteAddressString(int, int, const char *);
int CONV LJM_eReadAddress(int, int, int, double *);
int CONV LJM_eWriteAddress(int, int, int, double);
int CONV LJM_eReadAddressArray(int, int, int,	int , double *, int *);
int CONV LJM_eWriteAddressArray(int, int, int, int, const double *, int *);
int CONV LJM_eReadAddressByteArray(int, int, int, char *, int *);
int CONV LJM_eWriteAddressByteArray(int, int, int , const char *, int *);

#endif
