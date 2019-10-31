/**
 * @file assert.h
 * @brief Header file of the assert module.
 *
 * The file is the C/C++ interface of the assertion control module. The file
 * contains global function declarations, data structures, type definitions,
 * etc, of the module.
 *
 * The module contains custom assertion check macros, functions and special
 * declaration that helps identify the assertion.
 *
 * All functions, type definitions, data structures, etc., that used in
 * header or source files were documented in the file which it is declared.
 * For further information, see its detailed documentation.
 *
 * @author Furkan Kurt - kurtfu[at]yahoo.com
 * @date 2018-11-18
 *
 * @copyright Copyright (c) 2018 Furkan Kurt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 */
#ifndef ASSERT_H
#define ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*****************************************************************************/
/* PROJECT LIBRARIES                                                         */
/*****************************************************************************/

#include <macros.h>

/*****************************************************************************/
/* COMMON MACRO FUNCTIONS                                                    */
/*****************************************************************************/

/*
 * If the ID of the module has not been assigned, assigne 0 by default.
 */
#ifndef ID__
#   define ID__ (0u)
#endif

/**
 * @brief Checks the assertion.
 *
 * If \a expr is true, the macro does nothing. Otherwise, it calls the
 * @ref assert_failed function to terminate the program.
 *
 * @param expr The assertion expression.
 */
#define ASSERT(expr) ((expr) ? (void)0 : assert_failed(ID__, __LINE__))

/*****************************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                                */
/*****************************************************************************/

/**
 * @brief Executes the assertion failed process.
 *
 * The function executes the assertion failed process. This function should
 * be custom designed for each application.
 *
 * @param[in] id   The module ID.
 * @param[in] line The line of the expression.
 *
 * @return None.
 */
void
assert_failed(unsigned int id, unsigned int line);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* ASSERT_H */

/*** END OF FILE ***/
