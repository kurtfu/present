#------------------------------------------------------------------------------
# COMPILER FLAGS
#------------------------------------------------------------------------------

# Add search path for header files.
CC_FLAGS += $(addprefix -I , ${INC_PATH})

# Add language standard flag.
ifeq (${STD}, ANSI)
	CC_FLAGS += -ansi
else ifeq (${STD}, C99)
	CC_FLAGS += -std=c99
else ifeq (${STD}, C11)
	CC_FLAGS += -std=c11
endif

# Add strict ISO C warnings flag.
ifeq (${STRICT_ISO}, YES)
	CC_FLAGS += -pedantic
endif

# Add the all warnings flag.
ifeq (${WARN_ALL}, YES)
	CC_FLAGS += -Wall
endif

# Add the extra warnings flag.
ifeq (${WARN_EXTRA}, YES)
	CC_FLAGS += -Wextra
endif

#------------------------------------------------------------------------------
# BUILD RULES
#------------------------------------------------------------------------------

${EXEC}:
	${CL} ${CL_FLAGS} -o $@ ${OBJ}

${SLIB}:
	${AR} -crv $@ ${OBJ}

${TEST_OUT}: ${TEST_DEPS}
	${CL} ${CL_FLAGS} -o $@ ${OBJ} ${TEST_OBJ}

${BIN_PATH}/%.o: ${PROJ_PATH}/%.c
	@${MKDIR} "$(dir $@)" ||:
	${CC} ${CC_FLAGS} -c -o $@ $<

${BIN_PATH}/%.o: ${PROJ_PATH}/%.s
	@${MKDIR} "$(dir $@)" ||:
	${CC} ${CC_FLAGS} -c -o $@ $<

${BIN_PATH}/%.s: ${PROJ_PATH}/%.c
	@${MKDIR} "$(dir $@)" ||:
	${CC} ${CC_FLAGS} -S -o $@ $<

${BIN_PATH}/%.o: ${TEST_PATH}/%.c
	@${MKDIR} "$(dir $@)" ||:
	${CC} ${CC_FLAGS} -c -o $@ $<

${BIN_PATH}/%.s: ${TEST_PATH}/%.c
	@${MKDIR} "$(dir $@)" ||:
	${CC} ${CC_FLAGS} -S -o $@ $<
