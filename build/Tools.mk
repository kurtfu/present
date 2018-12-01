#------------------------------------------------------------------------------
# BUILD TOOLS
#------------------------------------------------------------------------------

CC = gcc  # C Compiler
CL = gcc  # Linker
AR = ar   # Archiver

#------------------------------------------------------------------------------
# COMPILER OPTIONS
#------------------------------------------------------------------------------

# The tag describes the language standard. To use a language standard while
# compiling, specify it before build. If the tag left blank, compiler's
# default standard is used. For GCC, default standard is GNU90.
# Supported standards:
# ANSI -> ANSI C standard
# C99  -> 1999 ISO C standard
# C11  -> 2011 ISO C standard

STD = C99

# The tag specifies strict ISO C standard feature. If the tag set as 'YES',
# all the warnings demanded by ISO C to be issued while compiling.

STRICT_ISO = YES

# The tag specifies all warning flag feature. If the tag set as 'YES',
# all warning flags are enabled while compiling.

WARN_ALL   = YES

# The tag specifies extra warning flag feature. If the tag set as 'YES',
# extra warning flags are enabled while compiling.

WARN_EXTRA = YES

#------------------------------------------------------------------------------
# EXTENSIONS
#------------------------------------------------------------------------------

ifeq (${OS}, Windows_NT)
    OUT_EXT = .exe  # Output extension on Microsof Windows
else
    OUT_EXT = .out  # Output extension on Unix-like systems
endif
