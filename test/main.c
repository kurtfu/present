/*! \file main.c
 *  \brief Test file of the project.
 *
 *  The file contains the test functions of the PRESENT cipher project.
 *
 *  All functions, type definitions, data structures, etc., that used in
 *  header or source files were documented in the file which it is declared.
 *  For further information, see its detailed documentation.
 *
 *  \see <a href=
 *       "https://link.springer.com/chapter/10.1007%2F978-3-540-74735-2_31">
 *       PRESENT: An Ultra-Lightweight Block Cipher</a>
 *
 *  \author Furkan Kurt - kurtfu[at]yahoo.com
 *  \date 2018-11-26
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

/*****************************************************************************/
/* STANDART C LIBRARIES                                                      */
/*****************************************************************************/

#include <stdio.h>

/*****************************************************************************/
/* PROJECT LIBRARIES                                                         */
/*****************************************************************************/

#include <present.h>

/*****************************************************************************/
/* TEST FUNCTION                                                             */
/*****************************************************************************/

/*! \brief Test function of the project.
 *
 *  The function crypts the test vector given in the article's appendix I.
 *  Results can be seen on terminal windows.
 *
 *  \return System error code.
 */
int main(void)
{
    uint8_t text_1[] = {0x00u, 0x00u, 0x00u, 0x00u,
                        0x00u, 0x00u, 0x00u, 0x00u};

    uint8_t text_2[] = {0x00u, 0x00u, 0x00u, 0x00u,
                        0x00u, 0x00u, 0x00u, 0x00u};

    uint8_t text_3[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu,
                        0xFFu, 0xFFu, 0xFFu, 0xFFu};

    uint8_t text_4[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu,
                        0xFFu, 0xFFu, 0xFFu, 0xFFu};

#if PRESENT_USE_KEY80
    uint8_t const key_1[] = {0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u, 0x00u};

    uint8_t const key_2[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu};

    uint8_t const key_3[] = {0x00u, 0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u, 0x00u};

    uint8_t const key_4[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu};
#else /* PRESENT_USE_KEY128 */
    uint8_t const key_1[] = {0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u};

    uint8_t const key_2[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu};

    uint8_t const key_3[] = {0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u,
                             0x00u, 0x00u, 0x00u, 0x00u};

    uint8_t const key_4[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu,
                             0xFFu, 0xFFu, 0xFFu, 0xFFu};
#endif /* PRESENT_USE_KEY128 */

    present_encrypt(text_1, key_1);
    printf("Cipher Text 1: %lx\n", *(uint64_t *)text_1);

    present_encrypt(text_2, key_2);
    printf("Cipher Text 2: %lx\n", *(uint64_t *)text_2);

    present_encrypt(text_3, key_3);
    printf("Cipher Text 3: %lx\n", *(uint64_t *)text_3);

    present_encrypt(text_4, key_4);
    printf("Cipher Text 4: %lx\n", *(uint64_t *)text_4);

    printf("------------------------------\n");

    present_decrypt(text_1, key_1);
    printf("Decipher Text 1: %lx\n", *(uint64_t *)text_1);

    present_decrypt(text_2, key_2);
    printf("Decipher Text 2: %lx\n", *(uint64_t *)text_2);

    present_decrypt(text_3, key_3);
    printf("Decipher Text 3: %lx\n", *(uint64_t *)text_3);

    present_decrypt(text_4, key_4);
    printf("Decipher Text 4: %lx\n", *(uint64_t *)text_4);

    return 0;
}
