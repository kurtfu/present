/**
 * @file test_main.c
 * @brief Test file of the project.
 *
 * The file contains the test functions of the PRESENT cipher project.
 *
 * All functions, type definitions, data structures, etc., that used in
 * header or source files were documented in the file which it is declared.
 * For further information, see its detailed documentation.
 *
 * @see <a href=
 *      "https://link.springer.com/chapter/10.1007%2F978-3-540-74735-2_31">
 *      PRESENT: An Ultra-Lightweight Block Cipher</a>
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

/*****************************************************************************/
/* PROJECT LIBRARIES                                                         */
/*****************************************************************************/

#include <present.h>
#include <unity.h>

/*****************************************************************************/
/* PLAIN TEXT EXAMPLES                                                       */
/*****************************************************************************/

static uint8_t text_1[] = {0x00u, 0x00u, 0x00u, 0x00u, \
                           0x00u, 0x00u, 0x00u, 0x00u};

static uint8_t text_2[] = {0x00u, 0x00u, 0x00u, 0x00u, \
                           0x00u, 0x00u, 0x00u, 0x00u};

static uint8_t text_3[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, \
                           0xFFu, 0xFFu, 0xFFu, 0xFFu};

static uint8_t text_4[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, \
                           0xFFu, 0xFFu, 0xFFu, 0xFFu};

/*****************************************************************************/
/* CIPHER KEY EXAMPLES                                                       */
/*****************************************************************************/

static uint8_t const key_1[] = {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, \
                                0x00u, 0x00u, 0x00u, 0x00u, 0x00u};

static uint8_t const key_2[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, \
                                0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu};

static uint8_t const key_3[] = {0x00u, 0x00u, 0x00u, 0x00u, 0x00u, \
                                0x00u, 0x00u, 0x00u, 0x00u, 0x00u};

static uint8_t const key_4[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu, \
                                0xFFu, 0xFFu, 0xFFu, 0xFFu, 0xFFu};

/*****************************************************************************/
/* CIPHERED TEXT EXAMPLES                                                    */
/*****************************************************************************/

static uint8_t cipher_1[] = {0x45u, 0x84u, 0x22u, 0x7Bu, \
                             0x38u, 0xC1u, 0x79u, 0x55u};

static uint8_t cipher_2[] = {0x49u, 0x50u, 0x94u, 0xF5u, \
                             0xC0u, 0x46u, 0x2Cu, 0xE7u};

static uint8_t cipher_3[] = {0x7Bu, 0x41u, 0x68u, 0x2Fu, \
                             0xC7u, 0xFFu, 0x12u, 0xA1u};

static uint8_t cipher_4[] = {0xD2u, 0x10u, 0x32u, 0x21u, \
                             0xD3u, 0xDCu, 0x33u, 0x33u};

/*****************************************************************************/
/* DECIPHERED TEXT EXAMPLES                                                  */
/*****************************************************************************/

static uint8_t const decipher_1[] = {0x00u, 0x00u, 0x00u, 0x00u, \
                                     0x00u, 0x00u, 0x00u, 0x00u};

static uint8_t const decipher_2[] = {0x00u, 0x00u, 0x00u, 0x00u, \
                                     0x00u, 0x00u, 0x00u, 0x00u};

static uint8_t const decipher_3[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, \
                                     0xFFu, 0xFFu, 0xFFu, 0xFFu};

static uint8_t const decipher_4[] = {0xFFu, 0xFFu, 0xFFu, 0xFFu, \
                                     0xFFu, 0xFFu, 0xFFu, 0xFFu};

/*****************************************************************************/
/* TEST FUNCTIONS                                                            */
/*****************************************************************************/

/**
 * @brief Test function of the encryption.
 *
 * The function tests the encryption operation with the given test vectors.
 * Test vectors were given in the article's appendix I.
 *
 * @return None.
 */
void test_encrypt(void)
{
    present_encrypt(text_1, key_1);
    present_encrypt(text_2, key_2);
    present_encrypt(text_3, key_3);
    present_encrypt(text_4, key_4);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_1, text_1, sizeof(cipher_1));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_2, text_2, sizeof(cipher_2));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_3, text_3, sizeof(cipher_3));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_4, text_4, sizeof(cipher_4));
}  /* test_encrypt() */

/**
 * @brief Test function of the decryption.
 *
 * The function tests the decryption operation with the given test vectors.
 * Test vectors were given in the article's appendix I.
 *
 * @return None.
 */
void test_decrypt(void)
{
    present_decrypt(cipher_1, key_1);
    present_decrypt(cipher_2, key_2);
    present_decrypt(cipher_3, key_3);
    present_decrypt(cipher_4, key_4);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_1, decipher_1, sizeof(decipher_1));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_2, decipher_2, sizeof(decipher_2));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_3, decipher_3, sizeof(decipher_3));
    TEST_ASSERT_EQUAL_HEX8_ARRAY(cipher_4, decipher_4, sizeof(decipher_4));
}  /* test_decrypt() */

/**
 * @brief Test function of the project.
 *
 * The function sets and runs the test of crypt and decrypt operations.
 * Results can be seen on terminal window.
 *
 * @return System error code.
 */
int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_encrypt);
    RUN_TEST(test_decrypt);

    return UNITY_END();
}  /* test_main() */

/*** END OF FILE ***/
