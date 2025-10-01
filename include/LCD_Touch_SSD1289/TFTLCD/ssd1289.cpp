/*
 * Copyright (C)2010-2012 Henning Karlsen. All right reserved.
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
#include "helpers.h"
#include "ssd1289.h"
#include "mbed.h"


SSD1289_LCD::SSD1289_LCD( PinName CS, PinName RS, PinName WR, PinName DIR, PinName EN, PinName LE, BusOut* DATA_PORT,  PinName RD , PinName BL, backlight_t blType, float defaultBackLightLevel )
    : LCD( 240, 320,CS, RS, DIR, EN, LE, BL, blType, defaultBackLightLevel ) , _lcd_pin_wr( WR )
{
    _lcd_port = DATA_PORT;
    if ( RD != NC ) _lcd_pin_rd = new DigitalOut( RD );
    else _lcd_pin_rd = 0;
}


void SSD1289_LCD::Initialize( orientation_t orientation, colordepth_t colors )
{
    
     _orientation = orientation;
    _colorDepth = colors;
   
    _lcd_pin_cs = HIGH;

    if (_lcd_pin_bl != 0)
        *_lcd_pin_bl = HIGH;
    else if (_bl_pwm != 0)
        *_bl_pwm = _bl_pwm_default;

    if (_lcd_pin_rd != 0)
        *_lcd_pin_rd = HIGH;
    _lcd_pin_wr = HIGH;
    wait_ms(15);
    
    
    Activate();
    WriteCmdData( 0x00, 0x0001 ); // oscillator: 1 = on, 0 = off
    wait_ms( 1 );
    WriteCmdData( 0x03, 0xA8A4 ); // power control
    wait_ms( 1 );
    WriteCmdData( 0x0C, 0x0000 ); // power control 2
    wait_ms( 1 );
    WriteCmdData( 0x0D, 0x080C ); // power control 3
    wait_ms( 1 );
    WriteCmdData( 0x0E, 0x2B00 ); // power control 4
    wait_ms( 1 );
    WriteCmdData( 0x1E, 0x00B7 ); // power control 5
    wait_ms( 1 );
    WriteCmdData( 0x02, 0x0600 ); // driving waveform control
    wait_ms( 1 );
    WriteCmdData( 0x10, 0x0000 ); // sleep mode: 0 = exit, 1 = enter
    wait_ms( 1 );
    if ( _colorDepth == RGB16 )
    {
        switch ( _orientation )
        {
            case LANDSCAPE: // works
                WriteCmdData( 0x01, 0x293F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x6078 ); // entry mode
                break;
                
            case PORTRAIT_REV: // works
                WriteCmdData( 0x01, 0x693F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x6070 ); // entry mode
                break;
                
            case LANDSCAPE_REV: // works
                WriteCmdData( 0x01, 0x6B3F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x6078 ); // entry mode
                break;
                
            case PORTRAIT: // works
            default:
                WriteCmdData( 0x01, 0x2B3F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x6070 ); // entry mode
                break;
        }
    }
    else if ( _colorDepth == RGB18 )
    {
        switch ( _orientation )
        {
            case LANDSCAPE: // works
                WriteCmdData( 0x01, 0x293F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x4078 ); // entry mode
                break;
                
            case PORTRAIT_REV: // works
                WriteCmdData( 0x01, 0x693F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x4070 ); // entry mode
                break;
                
            case LANDSCAPE_REV: // works
                WriteCmdData( 0x01, 0x6B3F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x4078 ); // entry mode
                break;
                
            case PORTRAIT: // works
            default:
                WriteCmdData( 0x01, 0x2B3F ); // driver output control
                wait_ms( 1 );
                WriteCmdData( 0x11, 0x4070 ); // entry mode
                break;
        }
    }
    wait_ms( 1 );
    WriteCmdData( 0x05, 0x0000 ); // compare register
    wait_ms( 1 );
    WriteCmdData( 0x06, 0x0000 ); // compare register
    wait_ms( 1 );
    WriteCmdData( 0x16, 0xEF1C ); // horizontal porch
    wait_ms( 1 );
    WriteCmdData( 0x17, 0x0003 ); // vertical porch
    wait_ms( 1 );
    WriteCmdData( 0x07, 0x0233 ); // display control
    wait_ms( 1 );
    WriteCmdData( 0x0B, 0x0000 ); // frame cycle control
    wait_ms( 1 );
    WriteCmdData( 0x0F, 0x0000 ); // gate scan position
    wait_ms( 1 );
    WriteCmdData( 0x41, 0x0000 ); // vertical scroll control
    wait_ms( 1 );
    WriteCmdData( 0x42, 0x0000 ); // vertical scroll control
    wait_ms( 1 );
    WriteCmdData( 0x48, 0x0000 ); // 1st screen driving position
    wait_ms( 1 );
    WriteCmdData( 0x49, 0x013F ); // 1st screen driving position
    wait_ms( 1 );
    WriteCmdData( 0x4A, 0x0000 ); // 2nd screen driving position
    wait_ms( 1 );
    WriteCmdData( 0x4B, 0x0000 ); // 2nd screen driving position
    wait_ms( 1 );
    WriteCmdData( 0x44, 0xEF00 ); // horizontal ram address position
    wait_ms( 1 );
    WriteCmdData( 0x45, 0x0000 ); // vertical ram address position
    wait_ms( 1 );
    WriteCmdData( 0x46, 0x013F ); // vertical ram address position
    wait_ms( 1 );
    WriteCmdData( 0x30, 0x0707 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x31, 0x0204 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x32, 0x0204 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x33, 0x0502 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x34, 0x0507 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x35, 0x0204 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x36, 0x0204 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x37, 0x0502 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x3A, 0x0302 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x3B, 0x0302 ); // gamma control
    wait_ms( 1 );
    WriteCmdData( 0x23, 0x0000 ); // GRAM write mask for red and green pins
    wait_ms( 1 );
    WriteCmdData( 0x24, 0x0000 ); // GRAM write mask for blue pins
    wait_ms( 1 );
    WriteCmdData( 0x25, 0x8000 ); // frame frequency control
    wait_ms( 1 );
    WriteCmdData( 0x4e, 0x0000 ); // ram address set
    wait_ms( 1 );
    WriteCmdData( 0x4f, 0x0000 ); // ram address set
    wait_ms( 1 );
    WriteCmd( 0x22 ); // write GRAM
    Deactivate();
}

void SSD1289_LCD::Sleep( void )
{
    WriteCmdData( 0x10, 0x0001 ); // sleep mode: 0 = exit, 1 = enter
    LCD::Sleep();
}

void SSD1289_LCD::WakeUp( void )
{
    WriteCmdData( 0x10, 0x0000 ); // sleep mode: 0 = exit, 1 = enter
    LCD::WakeUp();
}
void SSD1289_LCD::WriteCmd( unsigned short cmd )
{
    _lcd_pin_cs = LOW;
    _lcd_pin_rs = LOW;           // Control Reg 
    _lcd_pin_dir = HIGH;
    _lcd_pin_en = LOW;
    _lcd_port->write(cmd & 0xFF);
    _lcd_pin_le = HIGH;
    _lcd_pin_le = LOW;
    _lcd_port->write((cmd >>8) & 0xFF );
    pulseLow( _lcd_pin_wr ); 
    _lcd_pin_cs = HIGH;
}


void SSD1289_LCD::WriteData( unsigned short data )
{
    _lcd_pin_cs = LOW;
    _lcd_pin_rs = HIGH;       // Select Data
    _lcd_pin_dir = HIGH;
    _lcd_pin_en = LOW;
    _lcd_port->write(data & 0xFF);
    _lcd_pin_le = HIGH;
    _lcd_pin_le = LOW;
    _lcd_port->write((data >> 8) & 0xFF);
    pulseLow( _lcd_pin_wr ); 
    _lcd_pin_cs = HIGH;
}

void SSD1289_LCD::SetXY( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2 )
{
    if ( _orientation == PORTRAIT || _orientation == PORTRAIT_REV )
    {
        WriteCmdData( 0x44, ( x2 << 8 ) + x1 );
        WriteCmdData( 0x45, y1 );
        WriteCmdData( 0x46, y2 );
        WriteCmdData( 0x4e, x1 );
        WriteCmdData( 0x4f, y1 );
    }
    else
    {
        WriteCmdData( 0x44, ( y2 << 8 ) + y1 );
        WriteCmdData( 0x45, x1 );
        WriteCmdData( 0x46, x2 );
        WriteCmdData( 0x4e, y1 );
        WriteCmdData( 0x4f, x1 );
    }
    WriteCmd( 0x22 );
}

void SSD1289_LCD::SetPixelColor( unsigned int color, colordepth_t mode )
{
    unsigned char r, g, b;
    unsigned short clr;
    if ( _colorDepth == RGB16 )
    {
        switch ( mode )
        {
            case RGB16:
                WriteData( color & 0xFFFF );
                break;
            case RGB18:
                r = ( color >> 10 ) & 0xF8;
                g = ( color >> 4 ) & 0xFC;
                b = ( color >> 1 ) & 0x1F;
                clr = ( ( r | ( g >> 5 ) ) << 8 ) | ( ( g << 3 ) | b );
                WriteData( clr );
                break;
            case RGB24:
                r = ( color >> 16 ) & 0xF8;
                g = ( color >> 8 ) & 0xFC;
                b = color & 0xF8;
                clr = ( ( r | ( g >> 5 ) ) << 8 ) | ( ( g << 3 ) | ( b >> 3 ) );
                WriteData( clr );
                break;
        }
    }
    else if ( _colorDepth == RGB18 )
    {
        switch ( mode )
        {
            case RGB16:
                r = ( ( color >> 8 ) & 0xF8 ) | ( ( color & 0x8000 ) >> 13 );
                g = ( color >> 3 ) & 0xFC;
                b = ( ( color << 3 ) & 0xFC ) | ( ( color >> 3 ) & 0x01 );
                WriteData( ( r << 8 ) | g );
                WriteData( b );
                break;
            case RGB18:
                b = ( color << 2 ) & 0xFC;
                g = ( color >> 4 ) & 0xFC;
                r = ( color >> 10 ) & 0xFC;
                WriteData( ( r << 8 ) | g );
                WriteData( b );
                break;
            case RGB24:
                r = ( color >> 16 ) & 0xFC;
                g = ( color >> 8 ) & 0xFC;
                b = color & 0xFC;
                WriteData( ( r << 8 ) | g );
                WriteData( b );
                break;
        }
    }
}
