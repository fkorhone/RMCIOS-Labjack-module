/**
 * Name: LJM_Utilities.c
 * Desc: Provides some basic helper functions
**/

#ifndef LJM_UTILITIES
#define LJM_UTILITIES


#ifdef _WIN32
	#include <Winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <unistd.h> // For sleep() (with Mac OS or Linux).
	#include <arpa/inet.h>  // For inet_ntoa()
	#include <sys/time.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "LabJackM.h"

#define COMMAND 1
#define RESPONSE 0

#define INITIAL_ERR_ADDRESS -2
// This is just something negative so normal addresses are not confused with it


// Prompts the user to press enter
void WaitForUser();

// Calls WaitForUser if this is compiled on Windows. This call is intended to be placed
// at the end of a program to prevent output from being lost in the case that it is being called
// from Visual Studio.
void WaitForUserIfWindows();

// Returns a new handle or exits on error with a description of the error.
int OpenOrDie(int deviceType, int connectionType, const char * identifier);

// Closes the handle or exits on error with a description of the error.
void CloseOrDie(int handle);

// Prints the Feedback command or response
void PrintFeedbackCommand(const unsigned char * aMBFB, const char * description);
void PrintFeedbackResponse(const unsigned char * aMBFB, const char * description);
void PrintFeedbackBytes(const unsigned char * aMBFB, const char * description,
	int commandOrResponse);

// Print device info
void PrintDeviceInfoFromHandle(int handle);
void PrintDeviceInfo(int deviceType, int connectionType, int serialNumber, int ipAddressInt,
	int portOrPipe, int MaxBytesPerMB);

// Queries the device for value and prints the results
void GetAndPrint(int handle, const char * valueName);

// Queries the device for value and prints the results as an IP string
void GetAndPrintIPAddress(int handle, const char * valueName);

// Queries the device for value and prints the results as an MAC string. valueName
// is just a description, while valueAddress the address that determines what value
// is actually read from the device. valueAddress must be of type UINT64
void GetAndPrintMACAddressFromValueAddress(int handle, const char * valueName,
	int valueAddress);

// Queries the device for an address and prints the results
void GetAndPrintAddressAndType(int handle, const char * valueDescription, int address, int type);

// Queries LJM for the config value via LJM_ReadLibraryConfigS.
// Passes configParameter as Parameter and prints Value.
void GetAndPrintConfigValue(const char * configParameter);

// Queries LJM for the config value via LJM_ReadLibraryConfigStringS.
// Passes configParameter as Parameter and prints Value.
void GetAndPrintConfigString(const char * configParameter);

// Prints the current LJM logging configurations
void DisplayDebugLoggingConfigurations();

// Sets the LJM config value via LJM_WriteLibraryConfigS.
// Outputs error, if any
void SetConfigValue(const char * configParameter, double value);

// Sets the LJM config value via LJM_WriteLibraryConfigStringS.
// Outputs error, if any
void SetConfigString(const char * configParameter, const char * string);

// Performs an LJM_eWriteNames call.
// Prints error if any. The "OrDie" version exits the program upon error
void WriteNameOrDie(int handle, const char * name, double value);
int WriteName(int handle, const char * name, double value);

void WriteNameAltTypeOrDie(int handle, const char * name, int type, double value);

/**
 * Name: ErrorCheck
 * Desc: If err is not LJME_NOERROR, displays the error and exits the program.
 * Para: err, the error code being checked
 *       formattedDescription, a string representing what sort of operation returned err.
 *                             This can have formatting arguments in it, just like a string
 *                             passed to printf
**/
void ErrorCheck(int err, const char * formattedDescription, ...);

/**
 * Desc: Prints the error if there is an errror
**/
void PrintErrorIfError(int err, const char * formattedDescription, ...);

// Displays the error, error name, and value name
void CouldNotRead(int err, const char * valueName);

/**
 * Name: ErrorCheckWithAddress
 * Desc: If err is not LJME_NOERROR, displays the error and exits the program. If errAddress
 *       is not negative, outputs the error address corresponding to err
 * Para: err, the error code being checked
 *       formattedDescription, a string representing what sort of operation returned err.
 *                             This can have formatting arguments in it, just like a string
 *                             passed to printf
 *       errAddress, the address corresponding to err. If this is negative, it will not be output.
 *                   Note that LJM functions such as LJM_eReadAddresses will not change the
 *                   ErrorAddress unless relevant
**/
void ErrorCheckWithAddress(int err, int errAddress, const char * formattedDescription, ...);

// Sleeps for the given number of milliseconds
void MillisecondSleep(unsigned int milliseconds);

// Turns on the specified level of logging
void EnableLoggingLevel(double logLevel);

/**
 * Name: NumberToDeviceType
 * Desc: Takes an integer representing a device type and returns the device name
 *       as a string
 * Para: deviceType, the device type whose name will be returned
 * Retr: the device name, or "Unknown device type" if deviceType is not recognized
**/
const char * NumberToDeviceType(int deviceType);

/**
 * Name: NumberToConnectionType
 * Desc: Takes an integer representing a connection type and returns the connection type
 *       as a string
 * Para: connectionType, the connection type whose name will be returned
 * Retr: the connection type, or "Unknown connection type" if connectionType is not recognized
**/
const char * NumberToConnectionType(int connectionType);

/**
 * Name: NumberToDebugLogMode
 * Desc: Takes an integer representing a LJM_DEBUG_LOG_MODE and returns the mode name
 *       as a string
 * Retr: the mode name, or "Unknown LJM_DEBUG_LOG_MODE" if mode is not recognized
 * Note: See LJM_DEBUG_LOG_MODE in LabJackM.h
**/
const char * NumberToDeviceType(int mode);

/**
 * Desc: Returns the current CPU time in milliseconds
**/
unsigned int GetCurrentTimeMS();

/**
 * Desc: Returns IPv4String in integer form, handling error by calling ErrorAddress
**/
unsigned int IPToNumber(const char * IPv4String);

/**
 * Desc: Returns 0 (false) if v1 and v2 are not within delta of each other,
 *       returns 1 (true) if they are within delta each other
**/
int EqualFloats(double v1, double v2, double delta);

/**
 * Desc: Returns 1 (true) if connectionType is TCP-based,
 *       returns 0 (false) if not.
**/
int IsTCP(int connectionType);
 
/**
 * Desc: Returns the address of named register.
**/
int GetAddressFromNameOrDie(const char * name);


// Source

const char * NumberToDebugLogMode(int mode)
{
	static const char * LJM_DEBUG_LOG_MODE_NEVER_STRING = "LJM_DEBUG_LOG_MODE_NEVER";
	static const char * LJM_DEBUG_LOG_MODE_CONTINUOUS_STRING = "LJM_DEBUG_LOG_MODE_CONTINUOUS";
	static const char * LJM_DEBUG_LOG_MODE_ON_ERROR_STRING = "LJM_DEBUG_LOG_MODE_ON_ERROR";
	static const char * unknown_string = "Unknown LJM_DEBUG_LOG_MODE";

	if (mode == LJM_DEBUG_LOG_MODE_NEVER)
		return LJM_DEBUG_LOG_MODE_NEVER_STRING;
	if (mode == LJM_DEBUG_LOG_MODE_CONTINUOUS)
		return LJM_DEBUG_LOG_MODE_CONTINUOUS_STRING;
	if (mode == LJM_DEBUG_LOG_MODE_ON_ERROR)
		return LJM_DEBUG_LOG_MODE_ON_ERROR_STRING;

	return unknown_string;
}

const char * NumberToConnectionType(int connectionType)
{
	static const char * LJM_ctANY_string = "LJM_ctANY";
	static const char * LJM_ctUSB_string = "LJM_ctUSB";
	static const char * LJM_ctTCP_string = "LJM_ctTCP";
	static const char * LJM_ctETHERNET_string = "LJM_ctETHERNET";
	static const char * LJM_ctWIFI_string = "LJM_ctWIFI";
	static const char * unknown_string = "Unknown connection type";

	if (connectionType == LJM_ctANY)
		return LJM_ctANY_string;
	if (connectionType == LJM_ctUSB)
		return LJM_ctUSB_string;
	if (connectionType == LJM_ctTCP)
		return LJM_ctTCP_string;
	if (connectionType == LJM_ctETHERNET)
		return LJM_ctETHERNET_string;
	if (connectionType == LJM_ctWIFI)
		return LJM_ctWIFI_string;

	return unknown_string;
}

const char * NumberToDeviceType(int deviceType)
{
	static const char * LJM_dtANY_string = "LJM_dtANY";
	static const char * LJM_dtUE9_string = "LJM_dtUE9";
	static const char * LJM_dtU3_string = "LJM_dtU3";
	static const char * LJM_dtU6_string = "LJM_dtU6";
	static const char * LJM_dtT7_string = "LJM_dtT7";
	static const char * LJM_dtSKYMOTE_BRIDGE_string = "LJM_dtSKYMOTE_BRIDGE";
	static const char * LJM_dtDIGIT_string = "LJM_dtDIGIT";
	static const char * unknown_string = "Unknown device type";

	// These devices are not currently supported for LJM, but may be added
	static const int LJM_dtUE9 = 9;
	static const int LJM_dtU3 = 3;
	static const int LJM_dtU6 = 6;
	static const int LJM_dtSKYMOTE_BRIDGE = 1000;

	if (deviceType == LJM_dtANY)
		return LJM_dtANY_string;
	if (deviceType == LJM_dtUE9)
		return LJM_dtUE9_string;
	if (deviceType == LJM_dtU3)
		return LJM_dtU3_string;
	if (deviceType == LJM_dtU6)
		return LJM_dtU6_string;
	if (deviceType == LJM_dtT7)
		return LJM_dtT7_string;
	if (deviceType == LJM_dtSKYMOTE_BRIDGE)
		return LJM_dtSKYMOTE_BRIDGE_string;
	if (deviceType == LJM_dtDIGIT)
		return LJM_dtDIGIT_string;

	return unknown_string;
}

void PrintDeviceInfoFromHandle(int handle)
{
	int DeviceType, ConnectionType, SerialNumber, IPAddress, Port, MaxBytesPerMB;

	int err = LJM_GetHandleInfo(handle, &DeviceType, &ConnectionType, &SerialNumber,
		&IPAddress, &Port, &MaxBytesPerMB);

	ErrorCheckWithAddress(err, INITIAL_ERR_ADDRESS, "PrintDeviceInfoFromHandle (LJM_GetHandleInfo)");

	PrintDeviceInfo(DeviceType, ConnectionType, SerialNumber, IPAddress, Port, MaxBytesPerMB);
}

void PrintDeviceInfo(int deviceType, int connectionType, int serialNumber, int ipAddressInt, int portOrPipe,
	int packetMaxBytes)
{
	char ipAddressString[LJM_IPv4_STRING_SIZE];

	// Print
	printf("deviceType: %s\n", NumberToDeviceType(deviceType));
	printf("connectionType: %s\n", NumberToConnectionType(connectionType));
	printf("serialNumber: %d\n", serialNumber);

    if (IsTCP(connectionType)) {
		LJM_NumberToIP(ipAddressInt, ipAddressString);
		printf("IP address: %s\n", ipAddressString);
	}

	if (connectionType == LJM_ctUSB) {
		printf("pipe: %d\n", portOrPipe);
	}
	else {
		printf("port: %d\n", portOrPipe);
	}

	printf("The maximum number of bytes you can send to or receive from this device in one packet is %d bytes.\n",
		packetMaxBytes);
}

void WaitForUserIfWindows()
{
	#ifdef _WIN32
	WaitForUser();
	#endif
}

void WaitForUser()
{
	printf("Press enter to continue\n");
	getchar();

	// C++
	// std::cin.ignore();
}

int OpenOrDie(int deviceType, int connectionType, const char * identifier)
{
	int handle, err;
	err = LJM_Open(deviceType, connectionType, identifier, &handle);
	ErrorCheck(err, "LJM_Open(%d, %d, %s, ...)", deviceType, connectionType, identifier);
	return handle;
}

void CloseOrDie(int handle)
{
	int err = LJM_Close(handle);
	ErrorCheck(err, "LJM_Close(%d)", handle);
}

void PrintFeedbackCommand(const unsigned char * aMBFB, const char * description)
{
	PrintFeedbackBytes(aMBFB, description, COMMAND);
}

void PrintFeedbackResponse(const unsigned char * aMBFB, const char * description)
{
	PrintFeedbackBytes(aMBFB, description, RESPONSE);
}

void PrintFeedbackBytes(const unsigned char * aMBFB, const char * description,
	int commandOrResponse)
{
	int i, frameSize, frameOffset, frameCounter, numRemainingBytes;
	int reportedSize = (int)((int)aMBFB[4]+(int)aMBFB[5]);

	printf("%s:\n", description);
	printf("\tHeader:   ");
	for (i = 0; i<8; i++) {
		printf("0x%.2x ", aMBFB[i]);
	}
	printf("\n");
	if (reportedSize < 3) {
		printf("\t(No frames)\n");
	}
	else {
		if (commandOrResponse == COMMAND) {
			frameOffset = 8;
			frameCounter = 0;
			numRemainingBytes = reportedSize - 2;
			while (numRemainingBytes > 0) {
				if (aMBFB[frameOffset] == 1) { // If it's a write frame, it has data to output
					frameSize = aMBFB[frameOffset + 3] * 2 + 4;
				}
				else { // Else it's a read frame and is just a frame header
					frameSize = 4;
				}
				printf("\tframe %02d: ", frameCounter);
				for (i = 0; i<frameSize; i++) {
					printf("0x%.2x ", aMBFB[i+frameOffset]);
				}
				printf("\n");
				frameOffset += frameSize;
				numRemainingBytes -= frameSize;
				frameCounter++;
			}
		}
		else { // Response
			numRemainingBytes = reportedSize - 2;
			printf("\tdata:     ");
			for (i = 8; i<numRemainingBytes+8; i++) {
				printf("0x%.2x ", aMBFB[i]);
			}
			printf("\n");
		}
	}
}

void PrintErrorAddressHelper(int errAddress)
{
	if (!(errAddress < 0))
		printf("\terror address: %d\n", errAddress);
}

typedef enum {
	ACTION_PRINT_AND_EXIT,
	ACTION_PRINT
} ErrorAction;

// The "internal" print function for ErrorCheck and ErrorCheckWithAddress
void _ErrorCheckWithAddress(int err, int errAddress, ErrorAction action,
	const char * description, va_list args)
{
	char errName[LJM_MAX_NAME_SIZE];
	if (err >= LJME_WARNINGS_BEGIN && err <= LJME_WARNINGS_END) {
		LJM_ErrorToString(err, errName);
		vfprintf (stdout, description, args);
		printf(" warning: \"%s\" (Warning code: %d)\n", errName, err);
		PrintErrorAddressHelper(errAddress);
	}
	else if (err != LJME_NOERROR)
	{
		LJM_ErrorToString(err, errName);
		vfprintf (stdout, description, args);
		printf(" error: \"%s\" (ErrorCode: %d)\n", errName, err);
		PrintErrorAddressHelper(errAddress);

		if (action == ACTION_PRINT_AND_EXIT) {
			printf("Closing all devices and exiting now\n");
			WaitForUserIfWindows();
			LJM_CloseAll();
			exit(err);
		}
	}
}

void PrintErrorIfError(int err, const char * formattedDescription, ...)
{
	va_list args;

	va_start (args, formattedDescription);
	_ErrorCheckWithAddress(err, INITIAL_ERR_ADDRESS, ACTION_PRINT, formattedDescription, args);
	va_end (args);
}

void ErrorCheck(int err, const char * formattedDescription, ...)
{
	va_list args;

	va_start (args, formattedDescription);
	_ErrorCheckWithAddress(err, INITIAL_ERR_ADDRESS, ACTION_PRINT_AND_EXIT, formattedDescription, args);
	va_end (args);
}

void ErrorCheckWithAddress(int err, int errAddress, const char * description, ...)
{
	va_list args;

	va_start (args, description);
	_ErrorCheckWithAddress(err, errAddress, ACTION_PRINT_AND_EXIT, description, args);
	va_end (args);
}

void MillisecondSleep(unsigned int milliseconds)
{
	#ifdef _WIN32
		Sleep(milliseconds);
	#else
		usleep(milliseconds*1000);
	#endif
}

void GetAndPrint(int handle, const char * valueName)
{
	double value;
	int err;

	err = LJM_eReadName(handle, valueName, &value);
	if (err == LJME_NOERROR) {
		printf("%s: %f\n", valueName, value);
	}
	else {
		CouldNotRead(err, valueName);
	}
}

void GetAndPrintIPAddress(int handle, const char * valueName)
{
	double ip;
	char IP_STRING[LJM_IPv4_STRING_SIZE];
	int err;

	err = LJM_eReadName(handle, valueName, &ip);
	if (err == LJME_NOERROR) {
		err = LJM_NumberToIP((unsigned int)ip, IP_STRING);
		ErrorCheck(err, "Converting ip from device to a string");
		printf("%s: %s\n", valueName, IP_STRING);
	}
	else {
		CouldNotRead(err, valueName);
	}
}

void GetAndPrintMACAddressFromValueAddress(int handle, const char * valueName, int valueAddress)
{
	int err, i;
	enum { NUM_BYTES = 8 };
	enum { NUM_FRAMES = 1 };
	double mac[NUM_BYTES];
	int aAddresses[NUM_FRAMES] = {valueAddress};
	int aTypes[NUM_FRAMES] = {LJM_BYTE};
	int aWrites[NUM_FRAMES] = {LJM_READ};
	int aNumValues[NUM_FRAMES] = {NUM_BYTES};
	int ErrorAddress = INITIAL_ERR_ADDRESS;

	// This is what we would do if the MAC registers had a usable type
	// err = LJM_NameToAddress(valueName, aAddresses, aTypes);
	// if (err != LJME_NOERROR) {
	// 	char errString[LJM_MAX_NAME_SIZE];
	// 	LJM_ErrorToString(err, errString);
	// 	printf("Could not resolve %s to address. Error was %s (%d)\n", valueName, errString, err);
	// 	return;
	// }

	err = LJM_eAddresses(handle, 1, aAddresses, aTypes, aWrites, aNumValues, mac, &ErrorAddress);
	if (err != LJME_NOERROR) {
		CouldNotRead(err, valueName);
	}

	printf("%s: ", valueName);
	for (i=0; i<NUM_BYTES-1; i++) {
		printf("%02x:", (unsigned int)mac[i]);
	}
	printf("%02x\n", (unsigned int)mac[i]);
}

void GetAndPrintAddressAndType(int handle, const char * valueDescription, int address, int type)
{
	double value;
	int err;

	err = LJM_eReadAddress(handle, address, type, &value);
	if (err == LJME_NOERROR) {
		printf("%s: %f\n", valueDescription, value);
	}
	else {
		CouldNotRead(err, valueDescription);
	}
}

void GetAndPrintConfigValue(const char * configParameter)
{
	double value;
	int err;

	err = LJM_ReadLibraryConfigS(configParameter, &value);
	if (err == LJME_NOERROR) {
		printf("%s: %f\n", configParameter, value);
	}
	else {
		CouldNotRead(err, configParameter);
	}
}

void GetAndPrintConfigString(const char * configParameter)
{
	char string[LJM_MAX_NAME_SIZE];
	int err;

	err = LJM_ReadLibraryConfigStringS(configParameter, string);
	if (err == LJME_NOERROR) {
		printf("%s: %s\n", configParameter, string);
	}
	else {
		CouldNotRead(err, configParameter);
	}
}

void SetConfigValue(const char * configParameter, double value)
{
	int err = LJM_WriteLibraryConfigS(configParameter, value);
	PrintErrorIfError(err, "[LJM_WriteLibraryConfigS(Parameter=%s, Value=%f)]", configParameter, value);
}

void SetConfigString(const char * configParameter, const char * string)
{
	int err = LJM_WriteLibraryConfigStringS(configParameter, string);
	PrintErrorIfError(err, "[LJM_WriteLibraryConfigS(Parameter=%s, String=%s)]", configParameter, string);
}

void WriteNameOrDie(int handle, const char * name, double value)
{
	int err = WriteName(handle, name, value);
	if (err) {
		WaitForUserIfWindows();
		exit(err);
	}
}

int WriteName(int handle, const char * name, double value)
{
	int err = LJM_eWriteName(handle, name, value);
	PrintErrorIfError(err, "LJM_eWriteName(Handle=%d, Name=%s, Value=%f)", handle, name, value);
	return err;
}

void WriteNameAltTypeOrDie(int handle, const char * name, int type, double value)
{
	int address;
	int err = LJM_NameToAddress(name, &address, NULL);
	ErrorCheck(err, "WriteNameAltTypeOrDie: LJM_NameToAddress(Name=%s, ...)", name);
	err = LJM_eWriteAddress(handle, address, type, value);
	ErrorCheck(err, "WriteNameAltTypeOrDie: LJM_eWriteAddress(Handle=%d, Address=%d, Type=%d, Value=%f)",
		handle, address, type, value);
}

void CouldNotRead(int err, const char * valueName)
{
	char errString[LJM_MAX_NAME_SIZE];
	LJM_ErrorToString(err, errString);
	printf("Could not read %s. Error was %s (%d)\n", valueName, errString, err);
}

void EnableLoggingLevel(double logLevel)
{
	// Warning: These calls may change
	ErrorCheck(LJM_WriteLibraryConfigS(LJM_DEBUG_LOG_MODE, 2.0), "Setting log mode to continuous");
	ErrorCheck(LJM_WriteLibraryConfigS(LJM_DEBUG_LOG_LEVEL, logLevel), "Setting log level");
}

unsigned int GetCurrentTimeMS()
{
	#ifdef _WIN32
		return GetTickCount();
	#else
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * 1000 + tv.tv_usec / 1000;
	#endif
}

unsigned int IPToNumber(const char * IPv4String)
{
	unsigned int number = 0;
	int err = LJM_IPToNumber(IPv4String, &number);
	ErrorCheck(err, "LJM_IPToNumber - %s", IPv4String);

	return number;
}

int EqualFloats(double v1, double v2, double delta)
{
	if (v1 - v2 < delta && v2 - v1 < delta) {
		return 1; // True
	}

	return 0; // False
}

int IsTCP(int connectionType)
{
	if (connectionType == LJM_ctTCP ||
		connectionType == LJM_ctETHERNET ||
		connectionType == LJM_ctWIFI)
	{
		return 1;
	}
	return 0;
}

void DisplayDebugLoggingConfigurations()
{
	double mode;
	int err = LJM_ReadLibraryConfigS(LJM_DEBUG_LOG_MODE, &mode);
	ErrorCheck(err, "LJM_ReadLibraryConfigS(LJM_DEBUG_LOG_MODE, ...)");
	printf("LJM_DEBUG_LOG_MODE: %s\n", NumberToDebugLogMode((int)mode));

	if ((int)mode != LJM_DEBUG_LOG_MODE_NEVER) {
		// GetAndPrintConfigString and GetAndPrintConfigValue are defined in LJM_Utilities.h
		GetAndPrintConfigString(LJM_DEBUG_LOG_FILE);

		GetAndPrintConfigValue(LJM_DEBUG_LOG_FILE_MAX_SIZE);
		GetAndPrintConfigValue(LJM_DEBUG_LOG_LEVEL);
	}
}

int GetAddressFromNameOrDie(const char * name)
{
	int address = -1;
	int err = LJM_NameToAddress(name, &address, NULL);
	ErrorCheck(err, "GetAddressFromNameOrDie > LJM_NameToAddress(%s, ...)", name);
	return address;
}

#endif // #define LJM_UTILITIES
