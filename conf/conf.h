/*! \file conf.h
 *  \brief Configuration file of the project.
 *
 *  The file contains global symbol definitions that could be used in static
 *  configurations.
 *
 *  All functions, type definitions, data structures, etc., that used in
 *  header or source files were documented in the file which it is declared.
 *  For further information, see its detailed documentation.
 *
 *  \note Before use the project, configure this file with respect to the
 *        system specifications.
 *
 *  \author Furkan Kurt - kurtfu[at]yahoo.com
 *  \date 2018-11-25
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
#ifndef CONF_H
#define CONF_H

/*! PRESENT module configuration flag. */
#define CONF_PRESENT (1u)
#if CONF_PRESENT
    /*! PRESENT key size flag. To use 80-bit key, enable this flag. */
    #define PRESENT_USE_KEY80  (1u)
    /*! PRESENT key size flag. To use 128-bit key, enable this flag. */
    #define PRESENT_USE_KEY128 (0u)

    /*! PRESENT algorithm round count. */
    #define PRESENT_ROUND_COUNT (31u)
#endif /* CONF_PRESENT */

#endif /* CONF_H */
