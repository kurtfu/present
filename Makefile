#------------------------------------------------------------------------------
# DEPENDENCY INCLUDES
#------------------------------------------------------------------------------

include build/Config.mk
include build/Tools.mk

#------------------------------------------------------------------------------
#  DEFAULT CONFIGURATION CHECKS
#------------------------------------------------------------------------------

ifndef PROJ
    PROJ = $(notdir ${CURDIR})  # If not defined, set the project name same
                                # with the root folder name.
endif

#------------------------------------------------------------------------------
# SHELL COMMANDS
#------------------------------------------------------------------------------

ifeq (${OS}, Windows_NT)
    SHELL = cmd             # Shell that used by the host.

    MKDIR = mkdir 2>nul     # Make directory and suspend any error.
    RMDIR = rd /s /q 2>nul  # Remove directory and suspend any error.

    # Replace slashes due to the Windows path notation.
    CURDIR := $(subst /,\,${CURDIR})

    # Function that used to search source files.
    define find
        $(subst ${CURDIR},${PROJ_PATH},$(shell "where" 2>nul /r ${1} ${2}))
    endef
else
    SHELL = /bin/sh         # Shell that used by the host.

    MKDIR = mkdir -p        # Make directory and suspend any error.
    RMDIR = rm -rf          # Remove directory and suspend any error.

    # Function that used to search source files.
    define find
        $(shell find ${1} -name "${2}")
    endef
endif

#------------------------------------------------------------------------------
# PATH DEFINITIONS
#------------------------------------------------------------------------------

PROJ_PATH  = .
BIN_PATH   = $(addsuffix /bin, ${PROJ_PATH})

#------------------------------------------------------------------------------
# INPUT & OUTPUT FILE DEFINITIONS
#------------------------------------------------------------------------------

SRC   = $(call find, ${PROJ_PATH},*.c)
SRC  += $(call find, ${PROJ_PATH},*.s)

# Replace slashes due to the Windows path notation.
ifeq (${OS}, Windows_NT)
	SRC := $(subst \,/,${SRC})
endif

SRC  := $(filter-out ${EXL_FILE}, ${SRC})
SRC  := $(filter-out ${BIN_PATH}/%.s,${SRC})

ASM   = $(patsubst ${PROJ_PATH}/%.c,${BIN_PATH}/%.s, ${SRC})

OBJ   = $(patsubst ${PROJ_PATH}/%.c,${BIN_PATH}/%.o, ${SRC})
OBJ  := $(patsubst ${PROJ_PATH}/%.s,${BIN_PATH}/%.o, ${OBJ})

EXEC  = $(addprefix ${BIN_PATH}/, ${PROJ})
EXEC := $(addsuffix ${OUT_EXT}, ${EXEC})

SLIB  = $(addprefix ${BIN_PATH}/, ${PROJ})
SLIB := $(addsuffix .a, ${SLIB})

BUILD_DEPS = ${OBJ}

ifeq (${KEEP_ASM}, YES)
	BUILD_DEPS += ${ASM}
endif

ifndef TYPE
	OUT = ${EXEC}
else
    ifeq (${TYPE}, EXEC)
        OUT = ${EXEC}
    else ifeq (${TYPE}, SLIB)
        OUT = ${SLIB}
    endif
endif

#------------------------------------------------------------------------------
# MAKE RULES
#------------------------------------------------------------------------------

.PHONY: all build clean rebuild

all: build ${OUT}
	@echo "Project Build Successfully"

build: ${BUILD_DEPS}
	@echo "Objects Build Successfully"

clean:
	@${RMDIR} "${BIN_PATH}" ||:
	@echo "Project Cleaned Successfully"

rebuild: clean all

#------------------------------------------------------------------------------
# RULE INCLUDES
#------------------------------------------------------------------------------

include build/Rules.mk
