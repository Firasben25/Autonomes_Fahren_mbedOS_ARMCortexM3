/** \file terminus.h
 *  \brief Terminus fontrs for mbed TFTLCDD library
 *  \version 0.1
 *  \copyright GNU Public License, v2.
 *
 * This header provides 2 fixed-width fonts, both based on the
 * free Terminus font.  The smaller font is 8x12 pixels, and
 * the big one is 
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
 
 #ifndef TFTLCD_TERMINUS_H
 #define TFTLCD_TERMINUS_H
 
 
#ifdef __cplusplus
extern "C" {
#endif

typedef struct FontMetrics_struct
{
    uint8_t Offset;
    uint8_t Width;
    uint8_t Height;
    const int16_t* Position;
    const char* Data;
} font_t;

extern const font_t TerminusFont;

extern const font_t TerminusBigFont;

#ifdef __cplusplus
}
#endif

#endif /* TFTLCD_TERMINUS_H */
