/*! \file present.h
 *  \brief Header file of the PRESENT crypt module.
 *
 *  The file is the C/C++ interface of the PRESENT crypt module. The file
 *  contains global symbol and function declarations, data structures, type
 *  definitions, etc, of the module.
 *
 *  All functions, variables, algorithms, etc., that used in the module are
 *  designed with respect to the specifications of the article "PRESENT: An
 *  Ultra-Lightweight Block Cipher". For further information, see the article.
 *
 *  All functions, type definitions, data structures, etc., that used in
 *  header or source files were documented in the file which it is declared.
 *  For further information, see its detailed documentation.
 *
 *  \note Before use the module, configure the \ref conf.h file with respect
 *        to the system specifications.
 *
 *  \see <a href=
 *       "https://link.springer.com/chapter/10.1007%2F978-3-540-74735-2_31">
 *       PRESENT: An Ultra-Lightweight Block Cipher</a>
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
#ifndef PRESENT_H
#define PRESENT_H

#include <conf.h>

/*****************************************************************************/
/* PROJECT LIBRARIES                                                         */
/*****************************************************************************/

#include <macros.h>

/*****************************************************************************/
/* GLOBAL SYMBOL DEFINITIONS                                                 */
/*****************************************************************************/

/*! PRESENT crypt block size in bit. */
#define PRESENT_CRYPT_BIT_SIZE (64u)

/*! PRESENT crypt block size in byte. */
#define PRESENT_CRYPT_SIZE (PRESENT_CRYPT_BIT_SIZE / 8u)

/*! PRESENT key block size in bit. */
#if PRESENT_USE_KEY80
    #define PRESENT_KEY_BIT_SIZE (80u)
#elif PRESENT_USE_KEY128
    #define PRESENT_KEY_BIT_SIZE (128u)
#endif /* PRESENT_USE_KEY128 */

/*! PRESENT key block size in byte. */
#define PRESENT_KEY_SIZE (PRESENT_KEY_BIT_SIZE / 8u)

/*! Minimum possible count of the PRESENT loop. */
#define PRESENT_ROUND_COUNT_MIN (1u)

/*! Maximum possible count of the PRESENT loop. */
#define PRESENT_ROUND_COUNT_MAX (31u)

/*****************************************************************************/
/* GLOBAL FUNCTION PROTOTYPES                                                */
/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! \brief Encrypts the raw text block.
 *
 *  The function encrypts the raw text block pointed by \a p_text with the
 *  key value pointed by parameter \a p_key. The function encrypts only one
 *  block of data with length of \ref PRESENT_CRYPT_SIZE per call. Algorithm
 *  of the function is described in the article. For further information,
 *  see the article.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE and parameter \a p_key
 *           points a memory block with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *  \param p_key  Pointer of the crypt key.
 *
 *  \return None.
 */
void
present_encrypt(uint8_t *p_text, uint8_t const *p_key);

/*! \brief Decrypts the crypted text block.
 *
 *  The function decrypts the crypted text block pointed by \a p_text with
 *  the key value pointed by parameter \a p_key. The function decrypts only
 *  one block of data with length of \ref PRESENT_CRYPT_SIZE per call.
 *  Algorithm of the function is described in the article. For further
 *  information, see the article.
 *
 *  \warning The function assumes parameter \a p_text points a memory block
 *           with length of \ref PRESENT_CRYPT_SIZE and parameter \a p_key
 *           points a memory block with length of \ref PRESENT_KEY_SIZE.
 *
 *  \param p_text Pointer of the text block.
 *  \param p_key  Pointer of the crypt key.
 *
 *  \return None.
 */
void
present_decrypt(uint8_t *p_text, uint8_t const *p_key);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PRESENT_H */
