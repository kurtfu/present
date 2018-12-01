/*! \file assert.c
 *  \brief Source file of the assert module.
 *
 *  The file is the C implementation of the assertion control. The file
 *  contains global and static function definitions, data structures, type
 *  definitions, etc, of the module.
 *
 *  All functions, type definitions, data structures, etc., that used in
 *  header or source files were documented in the file which it is declared.
 *  For further information, see its detailed documentation.
 *
 *  \author Furkan Kurt - kurtfu[at]yahoo.com
 *  \date 2018-11-18
 *
 *  \copyright Copyright (c) 2018 Furkan Kurt
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 */
#include <assert.h>

/*****************************************************************************/
/* GLOBAL FUNCTION DEFINITIONS                                               */
/*****************************************************************************/

void
assert_failed(unsigned int id, unsigned int line)
{
    UNUSED_PARAM(id);
    UNUSED_PARAM(line);

    for(;;) {
        /* INFINITE LOOP! */
    }
}
