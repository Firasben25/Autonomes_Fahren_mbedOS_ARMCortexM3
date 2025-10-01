/** \file helpers.h
 *  \brief Utility functions and macros.
 *  \copyright GNU Public License, v2. or later
 *
 * This library is based on the Arduino/chipKIT UTFT library by Henning
 * Karlsen, http://henningkarlsen.com/electronics/library.php?id=52
 *
 * Copyright (C)2010-2012 Henning Karlsen. All right reserved.
 *
 * Copyright (C)2012 Todor Todorov.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to:
 *
 * Free Software Foundation, Inc.
 * 51 Franklin St, 5th Floor, Boston, MA 02110-1301, USA
 *
 *********************************************************************/
#ifndef TFTLCD_HELPERS_H
#define TFTLCD_HELPERS_H

#include "mbed.h"

/** \def HIGH
 *  \brief User-friendly high pin level designation.
 */
#define HIGH                    1
/** \def LOW
 *  \brief User-friendly low pin level designation.
 */
#define LOW                     0

/** \def swap( type, a, b )
 *  \brief Convenience macro to swap two values.
 */
#define swap( type, a, b )      { type tmp = ( a ); ( a ) = ( b ); ( b ) = tmp; }

/** \def pulseLow( pin )
 *  \brief Toggles a pin low, then high.
 */
#define pulseLow( pin )         pin = LOW; pin = HIGH

/** \def pulseHigh( pin )
 *  \brief Toggles a pin high, then low.
 */
#define pulseHigh( pin )        pin = HIGH; pin = LOW

#ifndef ushort
typedef unsigned short ushort;
#endif

#endif /* TFTLCD_HELPERS_H */
