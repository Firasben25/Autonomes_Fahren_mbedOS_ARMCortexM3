/** \file ili9328.h
 *  \brief mbed LCD driver for displays with the ILI9328 controller.
 *  \copyright GNU Public License, v2. or later
 *
 * This library is based on the Arduino/chipKIT UTFT library by Henning
 * Karlsen, http://henningkarlsen.com/electronics/library.php?id=52
 *
 * Copyright (C)2010-2012 Henning Karlsen. All right reserved.
 *
 * Copyright (C)2012-2013 Todor Todorov.
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
#ifndef TFTLCD_ILI9328_H
#define TFTLCD_ILI9328_H

#include "lcd_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Represents a LCD instance.
 *
 * This is the utility class, through which the display can be manipulated
 * and graphics objects can be shown to the user.  A known display, which
 * works with this library is the INANBO-T24-ILI9328-V11 - a RGB TFT
 * with 240x320 pixels resolution and 65K/262K colors, using 8/16-bit interface.
 *
 * The display works with a supply voltage of 2.8-3.3 volts for both logic and
 * backlight.  It can be driven in 8bit or 16bit interface mode. (Current
 * version of the driver works only in 16bit mode for now.)
 *
 * How to use:
 * \code
 * // include the library, this will also pull in the header for the provided fonts
 * #include "ili9328.h"
 * 
 * // prepare the data bus for writing commands and pixel data
 * BusOut dataBus( p30, p29, p28, p27, p26, p25, p24, p23, p22, p21, p20, p19, p18, p17, p16, p15 ); // 16 pins
 * // create the lcd instance
 * ILI9328_LCD lcd( p14, p13, p12, p11, &dataBus ); // control pins and data bus
 *
 * int main()
 * {
 *     // initialize display - place it in standard portrait mode and set background to black and
 *     //                      foreground to white color.
 *     lcd.Initialize();
 *     // set current font to the smallest 8x12 pixels font.
 *     lcd.SetFont( Font8x12 );
 *     // print something on the screen
 *     lcd.Print( "Hello, World!", CENTER, 25 ); // align text to center horizontally and use starndard colors
 *
 *     while ( 1 ) { }
 * }
 *
 * \endcode
 * \version 0.1
 * \author Todor Todorov
 */
class ILI9328_LCD : public LCD
{
public:
    /** Creates a new instance of the class.
     *
     * \param CS Pin for the ChipSelect signal.
     * \param RESET Pin for the RESET line.
     * \param RS Pin for the RS signal.
     * \param WR Pin for the WR signal.
     * \param DATA_PORT Address of the data bus for transfer of commands and pixel data.
     * \param BL Pin for controlling the backlight. By default not used.
     * \param RD Pin for the RD signal. This line is not needed by the driver, so if you would like to
     *       use the pin on the mbed for something else, just pull-up the respective pin on the LCD high,
     *       and do not assign a value to this parameter when creating the controller instance.
     * \param blType The backlight type, the default is to utilize the pin - if supplied - as a simple on/off switch
     * \param defaultBacklightLevel If using PWM to control backlight, this would be the default brightness in percent after LCD initialization.
     */
    ILI9328_LCD( PinName CS, PinName RS, PinName WR, PinName DIR, PinName EN, PinName LE, BusOut* DATA_PORT,  PinName RD , PinName BL = NC, backlight_t blType = Constant, float defaultBackLightLevel = 1.0 );
    
    /** Initialize display.
     *
     * Wakes up the display from sleep, initializes power parameters.
     * This function must be called first, befor any painting on the
     * display is done, otherwise the positioning of graphical elements
     * will not work properly and any paynt operation will not be visible
     * or produce garbage.
     *
     * \param oritentation The display orientation, landscape is default.
     * \param colors The correct color depth to use for the pixel data. Value is disregarded.
     */
    virtual void Initialize( orientation_t orientation = LANDSCAPE, colordepth_t colors = RGB16 );
    
    /** Puts the display to sleep.
     *
     * When the display is in sleep mode, its power consumption is
     * minimized.  Before new pixel data can be written to the display
     * memory, the controller needs to be brought out of sleep mode.
     * \sa #WakeUp( void );
     * \remarks The result of this operation might not be exactly as
     *          expected. Putting the display to sleep will cause the
     *          controller to switch to the standard color of the LCD,
     *          so depending on whether the display is normally white,
     *          or normally dark, the screen might or might not go
     *          dark.  Additional power saving can be achieved, if
     *          the backlight of the used display is not hardwired on
     *          the PCB and can be controlled via the BL pin.
     */
    virtual void Sleep( void );
    
    /** Wakes up the display from sleep mode.
     *
     * This function needs to be called before any other, when the
     * display has been put into sleep mode by a previois call to
     * #Sleep( void ).
     */
    virtual void WakeUp( void );

protected:
    /** Sends a command to the display.
     *
     * \param cmd The display command.
     * \remarks Commands are controller-specific and this function needs to
     *          be implemented separately for each available controller.
     */
    virtual void WriteCmd( unsigned short cmd );
    
    /** Sends pixel data to the display.
     *
     * \param data The display data.
     * \remarks Sending data is controller-specific and this function needs to
     *          be implemented separately for each available controller.
     */
    virtual void WriteData( unsigned short data );
    
    /** Assigns a chunk of the display memory to receive data.
     *
     * When data is sent to the display after this function completes, the opertion will
     * start from the begining of the assigned address (pixel position) and the pointer
     * will be automatically incremented so that the next data write operation will continue
     * with the next pixel from the memory block.  If more data is written than available
     * pixels, at the end of the block the pointer will jump back to its beginning and
     * commence again, until the next address change command is sent to the display.
     *
     * \param x1 The X coordinate of the pixel at the beginning of the block.
     * \param y1 The Y coordinate of the pixel at the beginning of the block.
     * \param x2 The X coordinate of the pixel at the end of the block.
     * \param y2 The Y coordinate of the pixel at the end of the block.
     * \remarks Addressing commands are controller-specific and this function needs to be
     *          implemented separately for each available controller.
     */
    virtual void SetXY( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2 );
    
    /** Sets the color of the pixel at the address pointer of the controller.
     *
     * This function is to be provided by each implementation separately in
     * order to account for different color depth used by the controller.
     * \param color The color of the pixel.
     * \param mode The depth (palette) of the color.
     */
    virtual void SetPixelColor( unsigned int color, colordepth_t mode = RGB24 );

private:
    DigitalOut  _lcd_pin_wr;
    BusOut*     _lcd_port;
    DigitalOut* _lcd_pin_bl;
    DigitalOut* _lcd_pin_rd;
};

#ifdef __cplusplus
}
#endif

#endif /* TFTLCD_ILI9328_H */
