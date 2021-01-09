include RMCIOS-build-scripts/utilities.mk

SOURCES:=labjack-u12-module.c
FILENAME:=labjack-module
GCC?=${TOOL_PREFIX}gcc
DLLTOOL?=${TOOL_PREFIX}dlltool
MAKE?=make
INSTALLDIR:=..${/}..
export

compile:
	$(MAKE) -f RMCIOS-build-scripts${/}module_dll.mk compile TOOL_PREFIX=${TOOL_PREFIX}

install:
	-${MKDIR} "${INSTALLDIR}${/}modules"
	${COPY} *.dll ${INSTALLDIR}${/}modules

