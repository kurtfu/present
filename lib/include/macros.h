/*! \file macros.h
 *  \brief Header file of the common macros.
 *
 *  The file contains global symbol definitions that could be used in
 *  the project.
 *
 *  All functions, type definitions, data structures, etc., that used in
 *  header or source files were documented in the file which it is declared.
 *  For further information, see its detailed documentation.
 *
 *  \author Furkan Kurt - kurtfu[at]yahoo.com
 *  \date 2018-11-12
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
#ifndef MACROS_H
#define MACROS_H

/*****************************************************************************/
/* STANDART C LIBRARIES                                                      */
/*****************************************************************************/

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <limits.h>

/*****************************************************************************/
/* COMMON MACRO FUNCTIONS                                                    */
/*****************************************************************************/

/*! \brief Transforms bit number to binary value.
 *
 *  The macro returns nth bit binary equivalent \a x to its. It shifts
 *  binary 1 to left as much as \a x to get the equivalent.
 *
 *  \param x The bit number.
 *
 *  \return Binary equivalent.
 */
#define BIT(x) (1u << (x))

/*! \brief Gets bit value from a variable.
 *
 *  The macro gets the nth bit value of \a var. It returns either 0 or 1.
 *
 *  \param var The variable.
 *  \param bit The bit number.
 *
 *  \return The bit value.
 */
#define BITVAL(var, bit) (((var) & BIT(bit)) >> (bit))

#endif /* MACROS_H */
