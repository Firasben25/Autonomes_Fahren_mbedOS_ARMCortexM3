/** \file lcd_base.h
 *  \brief Base class for all LCD controller implementations.
 *  \copyright GNU Public License, v2. or later
 *
 * Generic object painting and screen control.
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
#ifndef TFTLCD_BASE_H
#define TFTLCD_BASE_H

#include "mbed.h"
#include "terminus.h"


#ifdef __cplusplus
extern "C" {
#endif

/** \def RGB(r,g,b)
 *  \brief Creates a RGB color from distinct bytes for the red, green and blue components.
 *
 * Displays which use 16 bits to assign colors to a specific pixel, use
 * 5 bits for the red component, 6 bits for the green component and 5
 * bits for the blue component.  Displays which have 18-bit color depth
 * use 6 bits for red, 6 bits for green and 6 bits for blue component.
 * This macro preserves the full 24-bit color depth, but it is the responsibility
 * of the respective driver to convert the color value to the correct format.
 */
#define RGB( r, g, b )          ( ( r ) << 16 ) | ( ( g ) << 8 ) | ( b )
/** \def COLOR_BLACK
 *  \brief Shorthand for RGB( 0, 0, 0 ).
 */
#define COLOR_BLACK             RGB( 0x00, 0x00, 0x00 )
/** \def COLOR_WHITE
 *  \brief Shorthand for RGB( 255, 255, 255 ).
 */
#define COLOR_WHITE             RGB( 0xFF, 0xFF, 0xFF )
/** \def COLOR_RED
 *  \brief Shorthand for RGB( 255, 0, 0 ).
 */
#define COLOR_RED               RGB( 0xFF, 0x00, 0x00 )
/** \def COLOR_GREEN
 *  \brief Shorthand for RGB( 0, 255, 0 ).
 */
#define COLOR_GREEN             RGB( 0x00, 0xFF, 0x00 )
/** \def COLOR_BLUE
 *  \brief Shorthand for RGB( 0, 0, 255 ).
 */
#define COLOR_BLUE              RGB( 0x00, 0x00, 0xFF )
/** \def COLOR_CYAN
 *  \brief Shorthand for RGB( 0, 255, 255 )
 */
#define COLOR_CYAN              RGB( 0x00, 0xFF, 0xFF )
/** \def COLOR_MAGENTA
 *  \brief Shorthand for RGB( 255, 0, 255 )
 */
#define COLOR_MAGENTA           RGB( 0xFF, 0x00, 0xFF )
/** \def COLOR_YELLOW
 *  \brief Shorthand for RGB( 255, 255, 0 )
 */
#define COLOR_YELLOW            RGB( 0xFF, 0xFF, 0x00 )


/** \enum Orientation_enum
 *  \brief Display orientation.
 */
enum Orientation_enum
{
    PORTRAIT = 0, /**< Top row of the screen is at 12 o'clock. */ 
    LANDSCAPE = 1, /**< Top row of the screen is at 9 o'clock. */
    PORTRAIT_REV = 2, /**< Top row of the screen is at 6 o'clock. */
    LANDSCAPE_REV = 3, /**< Top row of the screen is at 3 o'clock. */
};
/** \typedef orientation_t
 *  \brief Convenience shortcut for display orientation.
 */
typedef enum Orientation_enum orientation_t;

/** \enum ColorDepth_enum
 *  \brief Color depth
 */
enum ColorDepth_enum
{
    RGB16, /**< 16-bit colors, pixels can have 65K+ distinct color values */
    RGB18, /**< 18-bit colors, pixels can have 262K+ distinct color values */
    RGB24, /**< 24-bit colors, full 8 bits per component, 16M+ distinct color values */
};
/** \typedef colordepth_t
 *  \brief Convenience shortcut for display color depth.
 */
typedef enum ColorDepth_enum colordepth_t;

/** \enum Alignment_enum
 *  \brief Horizontal text alignment on the line.
 */
enum Alignment_enum
{
    LEFT = 0, /**< Left-oriented, naturally gravitate closer to the left edge of the screen. */
    CENTER = 9998, /**< Center-oriented, try to fit in the middle of the available space with equal free space to the left and right of the text. */
    RIGHT = 9999, /**< Right-oriented, naturally gravitate closer to the right edge of the screen, leaving any remaining free space to the left of the text. */
};
/** \typedef align_t
 *  \brief Convenience shortcut for text alignment.
 */
typedef enum Alignment_enum align_t;

///** \struct Font_struct
// *  \brief Describes fonts and their properties.
// *  \sa Comments in fonts.h
// */
//struct Font_struct
//{
//    const    char* font; /**< A pointer to the first byte in the font. */
//    unsigned char  width; /**< The width of each character, in pixels. */
//    unsigned char  height; /**< Height of each character, in pixels. */
//    unsigned char  offset; /**< Offset of the first character in the font. */
//    unsigned char  numchars; /**< Count of the available characters in the font. */
//};
///** \typedef font_metrics_t
// *  \brief Convenience shortcut for fonts properties.
// */
//typedef struct Font_struct font_metrics_t;

/** \struct Bitmap_struct
 *  \brief Describes an image.
 */
struct Bitmap_struct
{
    colordepth_t Format; /**< Color depth of the image. */
    unsigned short Width; /**< Width of the image in pixels. */
    unsigned short Height; /**< Height of the image in pixels. */
    const void* PixelData; /**< Image pixel data. */
};
/** \typedef bitmap_t
 *  \brief Convenience shortcut bitmap type.
 */
typedef struct Bitmap_struct bitmap_t;

/** \struct BacklightPwmCtrl_enum
 *  \brief Type of backlight control for the LCD.
 *
 * When the selected type is \c Constant, the pin is simply on or off - there is no gradation in the intensity of the display.
 * In this case any free pin can be used to control the backlight.  On the other hand, when PWM is used to control brightness,
 * take care to use only PWM-able mbed pins (p21, p22, p23, p24, p25, and p26), any other pins won't work.  It is assumed that
 * you know what you are doing, so no check is done to prevent using a non-PWM pin as assigned control pin, when either \c Direct
 * or \c Indirect option is used.
 *
 * \version 0.1
 * \remark When choosing PWM to control the backlight, you have the option to choose the pin to either source (\c Direct) or sink
 *         (\c Indirect) the current for LCD brightness control.  Be aware that the mbed pins can source (and probably sink when
 *         configured as inputs) only 4 mA @+3V3 VDD.  So if you are intending to use a bigger LCD, whith more LEDs in its backlight
 *         implementation, you probably want to interface it through a small signal transistor or a small MOSFET, in order to be able
 *         to handle a higher current without damaging your mbed.
 * \remark As of version 0.1 (2013-01-25) the Indirect method of PWM has not been implemented yet.
 */
enum BacklightPwmCtrl_enum
{
    Constant, /**< When the pin is a simple on/off switch. */
    Direct, /**< Control the brightness with PWM, as the control pin is sourcing the current to drive the backlight LEDs. */
    Indirect, /**< Control the brightness with PWM, as the control pin is sinking the current which drives the backlight LEDs. */
};
/** \typedef backlight_t
 *  \brief Convenience shortcut for the backlight control type.
 */
typedef BacklightPwmCtrl_enum backlight_t;


/** Base class for LCD implementations.
 *
 * All separate LCD controller implementations have to subclass this one.
 *
 * \version 0.1
 * \author Todor Todorov
 */
class LCD
{
public:

    /** Initialize display.
     *
     * Wakes up the display from sleep, initializes power parameters.
     * This function must be called first, befor any painting on the
     * display is done, otherwise the positioning of graphical elements
     * will not work properly and any paynt operation will not be visible
     * or produce garbage.
     *
     * This function is controller-specific and needs to be implemented
     * separately for each available display.
     * \param oritentation The display orientation, landscape is default.
     * \param colors The correct color depth to use for the pixel data.
     */
    virtual void Initialize( orientation_t orientation, colordepth_t colors ) = 0;
    
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
     * \remarks This function is controller-specific and needs to be
     *          implemented separately for each available display.
     */
    virtual void Sleep( void );
    
    /** Wakes up the display from sleep mode.
     *
     * This function needs to be called before any other, when the
     * display has been put into sleep mode by a previois call to
     * #Sleep( void ).
     * \remarks This function is controller-specific and needs to be
     *          implemented separately for each available display.
     */
    virtual void WakeUp( void );
    
    /** Set the foreground color for painting.
     *
     * This is the default foreground color to be used in painting operations.
     * If a specific output function allows for a different color to be specified
     * in place, the new setting will be used for that single operation only and
     * will not change this value.
     *
     * \param color The color to be used (24-bit color depth).
     * \sa #RGB(r,g,b)
     */
    virtual void SetForeground( unsigned int color = COLOR_WHITE);
    
    /** Set the background color for painting.
     *
     * This is the default color to be used for "empty" pixels while painting.
     * If a particular function allows for a different value to be specified
     * when the function is called, the new value will be used only for this
     * single call and will not change this setting.
     *
     * \param color The background color (24-bit color depth).
     * \sa #RGB(r,g,b)
     */
    virtual void SetBackground( unsigned int color = COLOR_BLACK );
    
    /** Sets the font to be used for painting of text on the screen.
     * \param font A pointer to the font data.
     * \sa Comments in file fonts.h
     */
    virtual void SetFont( const font_t* font );
    
    /** Gets the display width.
     *  \return Display width in pixels.
     */
    unsigned short GetWidth( void );
    
    /** Gets the display height.
     *  \return Display height in pixels.
     */
    unsigned short GetHeight( void );
    
    /** Gets the font width.
     *  \return The current font width.
     */
    uint8_t GetFontWidth( void );
    
    /** Gets the font height.
     *  \return The current font height.
     */
    uint8_t GetFontHeight( void );
    
    /** Fills the whole screen with a single color.
     * \param color The color to be used. The value must be in RGB-565 format.
     * \remarks The special values -1 and -2 signify the preset background and foreground colors, respectively.
     *          The backround color is the default.
     */
    virtual void FillScreen( int color = -1 );
    
    /** Sets the backlight intensity in percent as a float value in the range [0.0,1.0].
     *  \param level The backligh intensity in percent, where 0.0 is off and 1.0 is full brightness.
     */
     
    virtual void SetBacklightLevel( float level );
    
    /** Clears the screen.
     *
     * This is the same as calling #FillScreen() or #FillScreen( -1 ) to use the background color.
     */
    virtual void ClearScreen( void );
    
    /** Draws a pixel at the specified location.
     *
     * By default the function will use the preset foreground color, but the background
     * or a custom color could be used as well.
     *
     * \param x The horizontal offset of the pixel from the upper left corner of the screen.
     * \param y The vertical offset of the pixel from the upper left corner of the screen.
     * \param color The color to be used. Use a custom color, or -1 for background and -2 for foreground color.
     */
    virtual void DrawPixel( unsigned short x, unsigned short y, int color = -2 );
    
    /** Draws a line.
     *
     * \param x1 Horizontal offset of the beginning point of the line.
     * \param y1 Vertical offset of the beginning point of the line.
     * \param x2 Horizontal offset of the end point of the line.
     * \param y2 Verical offset of the end point of the line.
     * \param color The color to use for painting, or -1 for background, or -2 for foreground.
     */
    virtual void DrawLine( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color = -2 );
    
    /** Paints a rectangle.
     *
     * \param x1 The horizontal offset of the beginning point of one of the rectangle's diagonals.
     * \param y1 The vertical offset of the beginning point of one of the rectangle's diagonals.
     * \param x2 The horizontal offset of the end point of the same of the rectangle's diagonals.
     * \param y2 The vertical offset of the end point of the same of the rectangle's diagonals.
     * \param color The color to use for painting. -1 indicated background, -2 foreground, or custom color.
     */
    virtual void DrawRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color = -2 );
    
    /** Paints a rectangle and fills it with the paint color.
     *
     * \param x1 The horizontal offset of the beginning point of one of the rectangle's diagonals.
     * \param y1 The vertical offset of the beginning point of one of the rectangle's diagonals.
     * \param x2 The horizontal offset of the end point of the same of the rectangle's diagonals.
     * \param y2 The vertical offset of the end point of the same of the rectangle's diagonals.
     * \param color The color to use for painting. -1 indicated background, -2 foreground, or custom color.
     */
    virtual void DrawRoundRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color = -2 );
    
    /** Paints a rectangle with rounded corners.
     *
     * \param x1 The horizontal offset of the beginning point of one of the rectangle's diagonals.
     * \param y1 The vertical offset of the beginning point of one of the rectangle's diagonals.
     * \param x2 The horizontal offset of the end point of the same of the rectangle's diagonals.
     * \param y2 The vertical offset of the end point of the same of the rectangle's diagonals.
     * \param color The color to use for painting. -1 indicated background, -2 foreground, or custom color.
     */
    virtual void FillRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color = -2 );
    
    /** Paints a rectangle with rounded corners and fills it with the paint color.
     *
     * \param x1 The horizontal offset of the beginning point of one of the rectangle's diagonals.
     * \param y1 The vertical offset of the beginning point of one of the rectangle's diagonals.
     * \param x2 The horizontal offset of the end point of the same of the rectangle's diagonals.
     * \param y2 The vertical offset of the end point of the same of the rectangle's diagonals.
     * \param color The color to use for painting. -1 indicated background, -2 foreground, or custom color.
     */
    virtual void FillRoundRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color = -2 );
    
    /** Paints a circle.
     *
     * \param x The offset of the circle's center from the left edge of the screen.
     * \param y The offset of the circle's center from the top edge of the screen.
     * \param radius The circle's radius.
     * \param color The color to use for painting. -1 indicated background, -2 foreground, or custom color.
     */
    virtual void DrawCircle( unsigned short x, unsigned short y, unsigned short radius, int color = -2 );
    
    /** Paints a circle and fills it with the paint color.
     *
     * \param x The offset of the circle's center from the left edge of the screen.
     * \param y The offset of the circle's center from the top edge of the screen.
     * \param radius The circle's radius.
     * \param color The color to use for painting. -1 indicated background, -2 foreground, or custom color.
     */
    virtual void FillCircle( unsigned short x, unsigned short y, unsigned short radius, int color = -2 );
    
    /** Print a text on the screen.
     *
     * \param str The text.
     * \param x The horizontal offset form the left edge of the screen. The special values LEFT, CENTER,
     *          or RIGHT can be used instead of pixel offset to indicate the text's horizontal alignment.
     * \param y The vertical offset of the text from the top of the screen.
     * \param fgColor The foreground to use for painting the text; -1 indicates background color, -2 the foreground setting, or custom color.
     * \param bgColor The color to use for painting the empty pixels; -1 indicates the background color, -2 the foreground setting, or custom color.
     * \param deg If different than 0, the text will be rotated at an angle this many degrees around its starting point. Default is not to ratate.
     */
    virtual void Print( const char *str, unsigned short x, unsigned short y, int fgColor = -2, int bgColor = -1, unsigned short deg = 0 );
    
    /** Draw an image on the screen.
     *
     * The pixels of the picture must be in the RGB-565 format.  The data can be provided
     * as an array in a source or a header file.  To convert an image file to the appropriate
     * format, a special utility must be utilized.  One such tool is provided by Henning Karlsen,
     * the author of the UTFT display liberary and can be downloaded for free from his web site:
     * http://henningkarlsen.com/electronics/library.php?id=52
     *
     * \param x Horizontal offset of the first pixel of the image.
     * \param y Vertical offset of the first pixel of the image.
     * \param img Image data pointer.
     * \param scale A value of 1 will produce an image with its original size, while a different value will scale the image.
     */
    virtual void DrawBitmap( unsigned short x, unsigned short y, const bitmap_t* img, unsigned char scale = 1 );
    
    /** Draw an image on the screen.
     *
     * The pixels of the picture must be in the RGB-565 format.  The data can be provided
     * as an array in a source or a header file.  To convert an image file to the appropriate
     * format, a special utility must be utilized.  One such tool is provided by Henning Karlsen,
     * the author of the UTFT display liberary and can be downloaded for free from his web site:
     * http://henningkarlsen.com/electronics/library.php?id=52
     *
     * \param x Horizontal offset of the first pixel of the image.
     * \param y Vertical offset of the first pixel of the image.
     * \param img Image data pointer.
     * \param deg Angle to rotate the image before painting on screen, in degrees.
     * \param rox
     * \param roy
     */
    virtual void DrawBitmap( unsigned short x, unsigned short y, const bitmap_t* img, unsigned short deg, unsigned short rox, unsigned short roy );
        
protected:
    /** Creates an instance of the class.
     *
     * \param width Width of the display in pixels.
     * \param height Height of the display in pixels.
     * \param CS Pin connected to the CS input of the display.
     * \param RS Pin connected to the RS input of the display.
     * \param RESET Pin connected to the RESET input of the display.
     * \param BL Pin connected to the circuit controlling the LCD's backlight.
     * \param blType The type of backlight to be used.
     * \param defaultBacklight The standard backlight intensity (if using PWM control), expressed in percent as float value from 0.0 to 1.0
     */
    LCD( unsigned short width, unsigned short height, PinName CS, PinName RS,  PinName DIR, PinName EN, PinName LE, PinName BL, backlight_t blType, float defaultBacklight );

    /** Activates the display for command/data transfer.
     *
     * Usually achieved by pulling the CS pin of the display low.
     */
    virtual void Activate( void );
    
    /** Deactivates the display after data has been transmitted.
     *
     * Usually achieved by pulling the CS pin of the display high.
     */
    virtual void Deactivate( void );
    
    /** Sends a command to the display.
     *
     * \param cmd The display command.
     * \remarks Commands are controller-specific and this function needs to
     *          be implemented separately for each available controller.
     */
    virtual void WriteCmd( unsigned short cmd ) = 0;
    
    /** Sends pixel data to the display.
     *
     * \param data The display data.
     * \remarks Sendin data is controller-specific and this function needs to
     *          be implemented separately for each available controller.
     */
    virtual void WriteData( unsigned short data ) = 0;
    

    
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
    virtual void SetXY( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2 ) = 0;
    
    /** Resets the memory address for the next display write operation to the screen origins (0,0).
     */
    virtual void ClearXY( void );
    
    /** Sets the color of the pixel at the address pointer of the controller.
     *
     * This function is to be provided by each implementation separately in
     * order to account for different color depths used by the controller.
     * \param color The color of the pixel.
     * \param mode The depth (palette) of the color.
     */
    virtual void SetPixelColor( unsigned int color, colordepth_t mode = RGB24 ) = 0;
    
    /** Draws a horizontal line.
     *
     * This is a utility function to draw horizontal-only lines
     * for reduced code complexity and faster execution.
     *
     * \param x X coordinate of the starting point of the line.
     * \param y Y coordinate of the starting point of the line.
     * \param len Length of the line.
     * \param color The color to use to draw the line. By default the global foreground color is used ( -2 ),
     *              -1 switches to the default background color, or any custom color can be used.
     */
    virtual void DrawHLine( unsigned short x, unsigned short y, unsigned short len, int color = -2 );
    
    /** Draws a vertical line.
     *
     * This is a utility function to draw vertical-only lines
     * for reduced code complexity and faster execution.
     *
     * \param x X coordinate of the starting point of the line.
     * \param y Y coordinate of the starting point of the line.
     * \param len Height of the line.
     * \param color The color to use to draw the line. By default the global foreground color is used ( -2 ),
     *              -1 switches to the default background color, or any custom color can be used.
     */
    virtual void DrawVLine( unsigned short x, unsigned short y, unsigned short len, int color = -2 );
    
    /** Prints a character at the given position and using the given color.
     *
     * \param c The character.
     * \param x X coordinate of the character position.
     * \param y Y coordinate of the character position.
     * \param fgColor Foreground color for drawing. By default the global foreground color is used ( -2 ),
     *                -1 switches to the default background color, or any custom color can be used.
     * \param bgColor Background color for drawing. By default the global background color is used ( -1 ),
     *                -2 switches to the default foreground color, or any custom color can be used.
     */
    virtual void PrintChar( char c, unsigned short x, unsigned short y, int fgColor = -2, int bgColor = -1 );
    
    /** Prints a character at the given position and using the given color and with the given rotation.
     *
     * \param c The character.
     * \param x X coordinate of the character position.
     * \param y Y coordinate of the character position.
     * \param pos Position of the character in the string from which it originates (used to rotate a whole string).
     * \param fgColor Foreground color for drawing. By default the global foreground color is used ( -2 ),
     *                -1 switches to the default background color, or any custom color can be used.
     * \param bgColor Background color for drawing. By default the global background color is used ( -1 ),
     *                -2 switches to the default foreground color, or any custom color can be used.
     * \param deg The angle at which to rotate. 
     */
    virtual void RotateChar( char c, unsigned short x, unsigned short y, int pos, int fgColor = -2, int bgColor = -1, unsigned short deg = 0 );
    
    /** Sends both command and data to the display controller.
     *
     * This is a helper utility function which combines the 2 functions above
     * into one single convenience step.
     *
     * \param cmd The display command.
     * \param data The display pixel data.
     */
    virtual void WriteCmdData( unsigned short cmd, unsigned short data );  

protected:
    unsigned short  _disp_width, _disp_height;
    DigitalOut      _lcd_pin_cs, _lcd_pin_rs,  _lcd_pin_dir, _lcd_pin_en, _lcd_pin_le;
    orientation_t   _orientation;
    colordepth_t    _colorDepth;
    unsigned int    _foreground, _background;
    const font_t*   _font;
    DigitalOut*     _lcd_pin_bl;
    PwmOut*         _bl_pwm;
    backlight_t     _bl_type;
    float           _bl_pwm_default, _bl_pwm_current;
};

#ifdef __cplusplus
}
#endif

#endif /* TFTLCD_BASE_H */
