#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug_win64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/line.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/tests.o \
	${OBJECTDIR}/tinyrenderer-files/geometry.o \
	${OBJECTDIR}/tinyrenderer-files/model.o \
	${OBJECTDIR}/tinyrenderer-files/tgaimage.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/my_tiny_renderer

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/my_tiny_renderer: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/my_tiny_renderer ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/line.o: line.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Itinyrenderer-files -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/line.o line.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Itinyrenderer-files -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/tests.o: tests.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Itinyrenderer-files -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tests.o tests.cpp

${OBJECTDIR}/tinyrenderer-files/geometry.o: tinyrenderer-files/geometry.cpp
	${MKDIR} -p ${OBJECTDIR}/tinyrenderer-files
	${RM} "$@.d"
	$(COMPILE.cc) -g -Itinyrenderer-files -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tinyrenderer-files/geometry.o tinyrenderer-files/geometry.cpp

${OBJECTDIR}/tinyrenderer-files/model.o: tinyrenderer-files/model.cpp
	${MKDIR} -p ${OBJECTDIR}/tinyrenderer-files
	${RM} "$@.d"
	$(COMPILE.cc) -g -Itinyrenderer-files -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tinyrenderer-files/model.o tinyrenderer-files/model.cpp

${OBJECTDIR}/tinyrenderer-files/tgaimage.o: tinyrenderer-files/tgaimage.cpp
	${MKDIR} -p ${OBJECTDIR}/tinyrenderer-files
	${RM} "$@.d"
	$(COMPILE.cc) -g -Itinyrenderer-files -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/tinyrenderer-files/tgaimage.o tinyrenderer-files/tgaimage.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
