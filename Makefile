#------------------------------------------------------------------------------
# DEPENDENCY INCLUDES
#------------------------------------------------------------------------------

include build/Config.mk
include build/Source.mk
include build/Tools.mk

#------------------------------------------------------------------------------
#  CONFIGURATION ERROR CHECKS
#------------------------------------------------------------------------------

ifndef PROJ
    $(error "Project name must be specified!")
endif

ifndef TYPE
    $(error "Output type must be specified!")
endif

#------------------------------------------------------------------------------
# SHELL COMMANDS
#------------------------------------------------------------------------------

ifeq (${OS}, Windows_NT)
    SHELL = cmd             # Shell that used by the host.

    MKDIR = mkdir 2>nul     # Make directory and suspend any error.
    RMDIR = rd /s /q 2>nul  # Remove directory and suspend any error.
else
    SHELL = /bin/sh         # Shell that used by the host.

    MKDIR = mkdir -p        # Make directory and suspend any error.
    RMDIR = rm -rf          # Remove directory and suspend any error.
endif

#------------------------------------------------------------------------------
# PATH DEFINITIONS
#------------------------------------------------------------------------------

PROJ_PATH  = .
BIN_PATH   = $(addsuffix /bin, ${PROJ_PATH})
TEST_PATH  = $(addsuffix /test, ${PROJ_PATH})

#------------------------------------------------------------------------------
# INPUT & OUTPUT FILE DEFINITIONS
#------------------------------------------------------------------------------

SRC  := $(filter-out ${BIN_PATH}/%.s,${SRC})

ASM   = $(patsubst ${PROJ_PATH}/%.c,${BIN_PATH}/%.s, ${SRC})

OBJ   = $(patsubst ${PROJ_PATH}/%.c,${BIN_PATH}/%.o, ${SRC})
OBJ  := $(patsubst ${PROJ_PATH}/%.s,${BIN_PATH}/%.o, ${OBJ})

EXEC  = $(addprefix ${BIN_PATH}/, ${PROJ})
EXEC := $(addsuffix ${OUT_EXT}, ${EXEC})

SLIB  = $(addprefix ${BIN_PATH}/, ${PROJ})
SLIB := $(addsuffix .a, ${SLIB})

TEST_ASM  = $(patsubst ${PROJ_PATH}/%.c,${BIN_PATH}/%.s, ${TEST_SRC})
TEST_OBJ  = $(patsubst ${PROJ_PATH}/%.c,${BIN_PATH}/%.o, ${TEST_SRC})

TEST_OUT  = $(addprefix ${BIN_PATH}/test/, ${PROJ})
TEST_OUT := $(addsuffix ${OUT_EXT}, ${TEST_OUT})

BUILD_DEPS = ${OBJ}
TEST_DEPS  = ${TEST_OBJ}

ifeq (${KEEP_ASM}, YES)
	BUILD_DEPS += ${ASM}
	TEST_DEPS  += ${TEST_ASM}
endif

ifeq (${TYPE}, EXEC)
	OUT = ${EXEC}
else ifeq (${TYPE}, SLIB)
	OUT = ${SLIB}
endif

#------------------------------------------------------------------------------
# MAKE RULES
#------------------------------------------------------------------------------

.PHONY: all build clean rebuild test ${OUT}

all: build ${OUT}
	@echo "Project Build Successfully"

build: ${BUILD_DEPS}
	@echo "Objects Build Successfully"

clean:
	@${RMDIR} "${BIN_PATH}" ||:
	@echo "Project Cleaned Successfully"

rebuild: clean all

test: build ${TEST_OUT}
	@echo "Test Project Build Successfully"

#------------------------------------------------------------------------------
# RULE INCLUDES
#------------------------------------------------------------------------------

include build/Rules.mk
