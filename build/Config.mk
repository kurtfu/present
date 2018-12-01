#------------------------------------------------------------------------------
# PROJECT CONFIGURATIONS
#------------------------------------------------------------------------------

# The tag describes the name of the project. Project name must be a single
# word. Whitespaces are not allowed. To use word sequences, separate the
# words with underscore.

PROJ      = present

# The tag describes the output type of the project. Before build the project,
# output type must be specified.
# Supported types:
# EXEC -> Executable output.
# SLIB -> Static library.

TYPE      = SLIB

#------------------------------------------------------------------------------
# SOURCE CONFIGURATIONS
#------------------------------------------------------------------------------

# The tag specifies assembly file generation option. If the tag set as 'YES',
# build system generates assembly files in addition to object files.
# Otherwise, build system generates only object files.

KEEP_ASM  = YES

#------------------------------------------------------------------------------
# COMPILER AND LINKER CONFIGURATIONS
#------------------------------------------------------------------------------

# The tag describes the search path for header files of the project. Multiple
# paths could be added.

INC_PATH  = ${PROJ_PATH}/include ${PROJ_PATH}/conf

# The tag describes the search path for library files of the project. Multiple
# paths could be added.

LIB_PATH =

# The tag describes the library files to be included to the project. Multiple
# libraries could be added.

LIB      =
