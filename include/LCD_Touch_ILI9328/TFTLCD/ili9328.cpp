/*
 * Copyright (C)2010-2012 Henning Karlsen. All right reserved.
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
#include "ili9328.h"
#include "helpers.h"


ILI9328_LCD::ILI9328_LCD( PinName CS, PinName RS, PinName WR, PinName DIR, PinName EN, PinName LE, BusOut* DATA_PORT,  PinName RD , PinName BL, backlight_t blType, float defaultBackLightLevel )
    : LCD( 240, 320, CS, RS, DIR, EN, LE, BL, blType, defaultBackLightLevel ), _lcd_pin_wr( WR )
{
    _lcd_port = DATA_PORT;
    if ( RD != NC ) _lcd_pin_rd = new DigitalOut( RD );
    else _lcd_pin_rd = 0;
}

void ILI9328_LCD::Initialize( orientation_t orientation, colordepth_t colors )
{
    _orientation = orientation;
    _colorDepth = colors;
    
 /*   _lcd_pin_reset = HIGH;
    wait_ms( 50 );
    _lcd_pin_reset = LOW;
    wait_ms( 100 );
    _lcd_pin_reset = HIGH;
    wait_ms( 1000 ); */
 
    _lcd_pin_cs = HIGH;
    if ( _lcd_pin_bl != 0 )
        *_lcd_pin_bl = HIGH;
    else if ( _bl_pwm != 0 )
        *_bl_pwm = _bl_pwm_default;
    if ( _lcd_pin_rd != 0 )
        *_lcd_pin_rd = HIGH;
    _lcd_pin_wr = HIGH;
    wait_ms( 50 );
    
    Activate();
    
    short drivOut = 0;
    short entryMod = 0;
    short gateScan = 0x2700;
    switch ( _orientation )
    {
        case LANDSCAPE:
            drivOut = 0x0100;
            entryMod |= 0x0038;
            gateScan |= 0x0000;
            break;
            
        case LANDSCAPE_REV:
            drivOut = 0x0000;
            entryMod |= 0x0038;
            gateScan |= 0x8000;
            break;
            
        case PORTRAIT_REV:
            drivOut = 0x0000;
            entryMod |= 0x0030;
            gateScan |= 0x0000;
            break;
            
        case PORTRAIT:
        default:
            drivOut = 0x0100;
            entryMod |= 0x0030;
            gateScan |= 0x8000;
            break;
    }
    switch ( _colorDepth )
    {
        case RGB18:
            entryMod |= 0x9000;
            break;
            
        case RGB16:
        default:
            entryMod |= 0x1000;
            break;
    } 
    
//    WriteCmdData( 0xE5, 0x78F0 ); // set SRAM internal timing
    WriteCmdData( 0xE5, 0x8000 ); // set SRAM internal timing
    WriteCmdData( 0x00, 0x0000 ); // set Driver Output Control
    WriteCmdData( 0x01, drivOut ); // set Driver Output Control
//    WriteCmdData( 0x02, 0x0200 ); // set 1 line inversion
    WriteCmdData( 0x02, 0x0700 ); // set 1 line inversion
    WriteCmdData( 0x03, entryMod ); // set GRAM write direction and BGR=1.
    WriteCmdData( 0x04, 0x0000 ); // Resize register
//    WriteCmdData( 0x08, 0x0207 ); // set the back porch and front porch
    WriteCmdData( 0x08, 0x0202 ); // set the back porch and front porch
 
    WriteCmdData( 0x09, 0x0000 ); // set non-display area refresh cycle ISC[3:0]
    WriteCmdData( 0x0A, 0x0000 ); // FMARK function
    WriteCmdData( 0x0C, 0x0000 ); // RGB interface setting
    WriteCmdData( 0x0D, 0x0000 ); // Frame marker Position
    WriteCmdData( 0x0F, 0x0000 ); // RGB interface polarity
    // ----------- Power On sequence ----------- //
    WriteCmdData( 0x10, 0x0000 ); // SAP, BT[3:0], AP, DSTB, SLP, STB
//    WriteCmdData( 0x11, 0x0007 ); // DC1[2:0], DC0[2:0], VC[2:0]
    WriteCmdData( 0x11, 0x0000 ); // DC1[2:0], DC0[2:0], VC[2:0]
    WriteCmdData( 0x12, 0x0000 ); // VREG1OUT voltage
    WriteCmdData( 0x13, 0x0000 ); // VDV[4:0] for VCOM amplitude
//    WriteCmdData( 0x07, 0x0001 );
    wait_ms( 200 ); // Dis-charge capacitor power voltage
 
//    WriteCmdData( 0x10, 0x1690 ); // SAP, BT[3:0], AP, DSTB, SLP, STB
    WriteCmdData( 0x10, 0x17B0 ); // SAP, BT[3:0], AP, DSTB, SLP, STB
//    WriteCmdData( 0x11, 0x0227 ); // Set DC1[2:0], DC0[2:0], VC[2:0]
    WriteCmdData( 0x11, 0x0037 ); // Set DC1[2:0], DC0[2:0], VC[2:0]
    wait_ms( 50 ); // Delay 50ms
 
//    WriteCmdData( 0x12, 0x000D ); // 0012
    WriteCmdData( 0x12, 0x0138 ); // 0012
    wait_ms( 50 ); // Delay 50ms
 
//    WriteCmdData( 0x13, 0x1200 ); // VDV[4:0] for VCOM amplitude
    WriteCmdData( 0x13, 0x1700 ); // VDV[4:0] for VCOM amplitude
//    WriteCmdData( 0x29, 0x000A ); // 04  VCM[5:0] for VCOMH
    WriteCmdData( 0x29, 0x000D ); // 04  VCM[5:0] for VCOMH
    WriteCmdData( 0x2B, 0x000D ); // Set Frame Rate
    wait_ms( 50 ); // Delay 50ms
 
    WriteCmdData( 0x20, 0x0000 ); // GRAM horizontal Address
    WriteCmdData( 0x21, 0x0000 ); // GRAM Vertical Address
 
    // ----------- Adjust the Gamma Curve ----------//
//    WriteCmdData( 0x30, 0x0000 );
    WriteCmdData( 0x30, 0x0001 );
//    WriteCmdData( 0x31, 0x0404 );
    WriteCmdData( 0x31, 0x0606 );
//    WriteCmdData( 0x32, 0x0003 );
    WriteCmdData( 0x32, 0x0304 );
//new line
    WriteCmdData( 0x33, 0x0202 );
    WriteCmdData( 0x34, 0x0202 );
//    WriteCmdData( 0x35, 0x0405 );
    WriteCmdData( 0x35, 0x0103 );
 
//    WriteCmdData( 0x36, 0x0808 );
    WriteCmdData( 0x36, 0x011D );
//    WriteCmdData( 0x37, 0x0407 );
    WriteCmdData( 0x37, 0x0404 );
//    WriteCmdData( 0x38, 0x0303 );
    WriteCmdData( 0x38, 0x0404 );
//    WriteCmdData( 0x39, 0x0707 );
    WriteCmdData( 0x39, 0x0404 );
//    WriteCmdData( 0x3C, 0x0504 );
    WriteCmdData( 0x3C, 0x0700 );
//    WriteCmdData( 0x3D, 0x0808 );
    WriteCmdData( 0x3D, 0x0A1F );
 
    //------------------ Set GRAM area ---------------//
    WriteCmdData( 0x50, 0x0000 ); // Horizontal GRAM Start Address
    WriteCmdData( 0x51, 0x00EF ); // Horizontal GRAM End Address
    WriteCmdData( 0x52, 0x0000 ); // Vertical GRAM Start Address
    WriteCmdData( 0x53, 0x013F ); // Vertical GRAM Start Address
    WriteCmdData( 0x60, gateScan ); // Gate Scan Line (0xA700)
//    WriteCmdData( 0x61, 0x0000 ); // NDL,VLE, REV
    WriteCmdData( 0x61, 0x0001 ); // NDL,VLE, REV
    WriteCmdData( 0x6A, 0x0000 ); // set scrolling line
    //-------------- Partial Display Control ---------//
    WriteCmdData( 0x80, 0x0000 );
    WriteCmdData( 0x81, 0x0000 );
    WriteCmdData( 0x82, 0x0000 );
    WriteCmdData( 0x83, 0x0000 );
    WriteCmdData( 0x84, 0x0000 );
    WriteCmdData( 0x85, 0x0000 );
    //-------------- Panel Control -------------------//
    WriteCmdData( 0x90, 0x0010 ); 
    WriteCmdData( 0x92, 0x0000 );
//new lines
    WriteCmdData( 0x93, 0x0003 );
    WriteCmdData( 0x95, 0x0101 );
    WriteCmdData( 0x97, 0x0000 );
    WriteCmdData( 0x98, 0x0000 );
 
//    WriteCmdData( 0x07, 0x0133 ); // 262K color and display ON
    WriteCmdData( 0x07, 0x0021 ); // 262K color and display ON
    WriteCmdData( 0x07, 0x0031 ); // 262K color and display ON
    WriteCmdData( 0x07, 0x0173 ); // 262K color and display ON
//new line       
     
    wait_ms( 50 ); // Delay 50ms    
    Deactivate();
}

void ILI9328_LCD::Sleep( void )
{
    Activate();
    WriteCmdData( 0x10, 0x1692 ); // enter sleep mode
    wait_ms( 200 );
    LCD::Sleep();
    Deactivate();
}

void ILI9328_LCD::WakeUp( void )
{
    Activate();
    WriteCmdData( 0x10, 0x1690 ); // exit sleep mode
    wait_ms( 200 );
    LCD::WakeUp();
    Deactivate();
}

void ILI9328_LCD::WriteCmd( unsigned short cmd )
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

void ILI9328_LCD::WriteData( unsigned short data )
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

void ILI9328_LCD::SetXY( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2 )
{
    switch ( _orientation )
    {
        case LANDSCAPE:
        case LANDSCAPE_REV:
            WriteCmdData( 0x20, y1 );
            WriteCmdData( 0x21, x1 );
            WriteCmdData( 0x50, y1 );
            WriteCmdData( 0x52, x1 );
            WriteCmdData( 0x51, y2 );
            WriteCmdData( 0x53, x2 );
            break;
            
        case PORTRAIT_REV:
        case PORTRAIT:
        default:
            WriteCmdData( 0x20, x1 );
            WriteCmdData( 0x21, y1 );
            WriteCmdData( 0x50, x1 );
            WriteCmdData( 0x52, y1 );
            WriteCmdData( 0x51, x2 );
            WriteCmdData( 0x53, y2 );
            break;
    }
    WriteCmd( 0x22 );
}

void ILI9328_LCD::SetPixelColor( unsigned int color, colordepth_t mode )
{
    unsigned char r, g, b;
    unsigned short clr;
    r = g = b = 0;
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
                break;
            case RGB18:
                b = ( color << 2 ) & 0xFC;
                g = ( color >> 4 ) & 0xFC;
                r = ( color >> 10 ) & 0xFC;
                break;
            case RGB24:
                r = ( color >> 16 ) & 0xFC;
                g = ( color >> 8 ) & 0xFC;
                b = color & 0xFC;
                break;
        }
        clr = ( r << 8 ) | ( g << 2 ) | ( b >> 4 );
        WriteData( clr );
        WriteData( b << 4 );
    }
}
