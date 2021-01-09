include RMCIOS-build-scripts/utilities.mk

SOURCES:=ljm_channels.c
FILENAME:=ljm-module
GCC?=${TOOL_PREFIX}gcc
DLLTOOL?=${TOOL_PREFIX}dlltool
MAKE?=make
INSTALLDIR:=..${/}..
CFLAGS+=liblabjackm.a 
CFLAGS+=-I./linklib
LINKDEF?=LabJackM.def
export

compile:
	$(DLLTOOL) -d ./linklib/${LINKDEF} -l liblabjackm.a 
	$(MAKE) -f RMCIOS-build-scripts${/}module_dll.mk compile TOOL_PREFIX=${TOOL_PREFIX}

