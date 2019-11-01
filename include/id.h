/**
 * @file id.h
 * @brief Header file of the file IDs.
 *
 * The file ID numbers of the source files that could be used in the project.
 *
 * All functions, type definitions, data structures, etc., that used in
 * header or source files were documented in the file which it is declared.
 * For further information, see its detailed documentation.
 *
 * @author Furkan Kurt - kurtfu[at]yahoo.com
 * @date 2018-11-26
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
#ifndef ID_H
#define ID_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @brief Source file ID type.
 *
 * This type is used for source file identification. Defining file IDs in an
 * enumaration type prevents any possible ID conflicts.
 */
typedef enum {
    /*! ID of the \ref main.c */
    FILE_ID_MAIN    = 1u,
    /*! ID of the \ref present.c */
    FILE_ID_PRESENT = 2u
} file_id_t;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* ID_H */

/*** END OF FILE ***/
