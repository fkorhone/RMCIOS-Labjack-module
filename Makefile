include RMCIOS-build-scripts/utilities.mk

TOOL_PREFIX:=
SOURCES:=*.c
FILENAME:=Labjack-module
GCC?=${TOOL_PREFIX}gcc
DLLTOOL?=${TOOL_PREFIX}dlltool
MAKE?=make
INSTALLDIR:=..${/}..
LINKDEF=LabJackM.def
export

all: ljm-module labjack-module

ljm-module:
	$(MAKE) -f ljm-module.mk

labjack-module:
	$(MAKE) -f labjack-module.mk

install:
	-${MKDIR} "${INSTALLDIR}${/}modules"
	${COPY} *.dll ${INSTALLDIR}${/}modules

