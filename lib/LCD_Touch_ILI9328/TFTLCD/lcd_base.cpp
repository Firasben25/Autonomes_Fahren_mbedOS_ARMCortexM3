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
#include "lcd_base.h"
#include "helpers.h"



LCD::LCD( unsigned short width, unsigned short height, PinName CS, PinName RS,  PinName DIR, PinName EN, PinName LE, PinName BL, backlight_t blType, float defaultBacklight )
    : _disp_width( width ), _disp_height( height ), _lcd_pin_cs( CS ), _lcd_pin_rs( RS ),  _lcd_pin_dir(DIR), _lcd_pin_en(EN), _lcd_pin_le(LE), _bl_type( blType )
{
    SetForeground();
    SetBackground();
    _font = &TerminusFont;
    if ( defaultBacklight < 0 ) _bl_pwm_default = 0;
    else if ( defaultBacklight > 1.0 ) _bl_pwm_default = 1.0;
    else _bl_pwm_default = defaultBacklight;
    if ( BL != NC )
    {
        if ( blType == Constant )
        {
            _bl_pwm = 0;
            _lcd_pin_bl = new DigitalOut( BL );
        }
        else
        {
            _lcd_pin_bl = 0;
            _bl_pwm = new PwmOut( BL );
            _bl_pwm->period_ms( 8.33 ); // 120 Hz
            _bl_pwm_current = _bl_pwm_default;
            // initially off
            *_bl_pwm = 0;
        }
        
    }
    else
    {
        _lcd_pin_bl = 0;
        _bl_pwm = 0;
    }
}

void LCD::Sleep( void )
{
    if ( _lcd_pin_bl != 0 )
        *_lcd_pin_bl = LOW;
    else if ( _bl_pwm != 0 )
        *_bl_pwm = 0;
}

void LCD::WakeUp( void )
{
    if ( _lcd_pin_bl != 0 )
        *_lcd_pin_bl = HIGH;
    else if ( _bl_pwm != 0 )
        *_bl_pwm = _bl_pwm_current;
}

inline
void LCD::SetForeground( unsigned int color )
{
    _foreground = color;
}

inline
void LCD::SetBackground( unsigned int color )
{
    _background = color;
}

void LCD::SetFont( const font_t *font )
{
    _font = font;
}

inline
unsigned short LCD::GetWidth( void )
{
    if ( _orientation == LANDSCAPE || _orientation == LANDSCAPE_REV ) return _disp_height;
    return _disp_width;
}

inline
unsigned short LCD::GetHeight( void )
{
    if ( _orientation == LANDSCAPE || _orientation == LANDSCAPE_REV ) return _disp_width;
    return _disp_height;
}

inline
uint8_t LCD::GetFontWidth( void )
{
    if ( _font != 0 ) return _font->Width;
    return 0;
}

inline
uint8_t LCD::GetFontHeight( void )
{
    if ( _font != 0 ) return _font->Height;
    return 0;
}

void LCD::SetBacklightLevel( float level )
{
    switch ( _bl_type )
    {
        case Direct:
            if ( _bl_pwm != 0 )
            {
                *_bl_pwm = level;
                _bl_pwm_current = level;
            }
            break;
            
        case Indirect:
            break;
        case Constant:
        default:
            break;
    }
}

void LCD::FillScreen( int color )
{
    
    
    unsigned int rgb = color == -1 ? _background : color == -2 ? _foreground : ( unsigned int ) color;
    Activate();
    ClearXY();
    for ( int i = 0; i < ( ( _disp_width ) * ( _disp_height ) ); i++ )
        SetPixelColor( rgb );
    Deactivate();
      
}

inline
void LCD::ClearScreen( void )
{
    FillScreen( -1 );
}

void LCD::DrawPixel( unsigned short x, unsigned short y, int color )
{
    
    Activate();
    SetXY( x, y, x, y );
    SetPixelColor( color == -1 ? _background :
                    color == -2 ? _foreground : color );
    Deactivate();
    
}

void LCD::DrawLine( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color )
{

    double delta, tx, ty;

    if ( ( ( x2 - x1 ) < 0 ) )
    {
        swap( ushort, x1, x2 )
        swap( ushort, y1, y2 )
    }
    if ( ( ( y2 - y1 ) < 0 ) )
    {
        swap( ushort, x1, x2 )
        swap( ushort, y1, y2 )
    }

    if ( y1 == y2 )
    {
        if ( x1 > x2 )
            swap( ushort, x1, x2 )
        DrawHLine( x1, y1, x2 - x1, color );
    }
    else if ( x1 == x2 )
    {
        if ( y1 > y2 )
            swap( ushort, y1, y2 )
        DrawVLine( x1, y1, y2 - y1, color );
    }
    else
    {
        unsigned int usedColor = color == -1 ? _background : color == -2 ? _foreground : ( unsigned int ) color;
        Activate();
        if ( abs( x2 - x1 ) > abs( y2 - y1 ) )
        {
            delta = ( double( y2 - y1 ) / double( x2 - x1 ) );
            ty = double( y1 );
            if ( x1 > x2 )
            {
                for ( int i = x1; i >= x2; i-- )
                {
                    SetXY( i, int( ty + 0.5 ), i, int( ty + 0.5 ) );
                    SetPixelColor( usedColor );
                    ty = ty - delta;
                }
            }
            else
            {
                for ( int i = x1; i <= x2; i++ )
                {
                    SetXY( i, int( ty + 0.5 ), i, int( ty + 0.5 ) );
                    SetPixelColor( usedColor );
                    ty = ty + delta;
                }
            }
        }
        else
        {
            delta = ( float( x2 - x1 ) / float( y2 - y1 ) );
            tx = float( x1 );
            if ( y1 > y2 )
            {
                for ( int i = y2 + 1; i > y1; i-- )
                {
                    SetXY( int( tx + 0.5 ), i, int( tx + 0.5 ), i );
                    SetPixelColor( usedColor );
                    tx = tx + delta;
                }
            }
            else
            {
                for ( int i = y1; i < y2 + 1; i++ )
                {
                    SetXY( int( tx + 0.5 ), i, int( tx + 0.5 ), i );
                    SetPixelColor( usedColor );
                    tx = tx + delta;
                }
            }
        }
        Deactivate();
    }
    
}

void LCD::DrawRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color )
{
    
    
    if ( x1 > x2 ) swap( ushort, x1, x2 )
    if ( y1 > y2 ) swap( ushort, y1, y2 )

    DrawHLine( x1, y1, x2 - x1, color );
    DrawHLine( x1, y2, x2 - x1, color );
    DrawVLine( x1, y1, y2 - y1, color );
    DrawVLine( x2, y1, y2 - y1, color );
    
    
}

void LCD::DrawRoundRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color )
{
    if ( x1 > x2 ) swap( ushort, x1, x2 )
    if ( y1 > y2 ) swap( ushort, y1, y2 )

    if ( ( x2 - x1 ) > 4 && ( y2 - y1 ) > 4 )
    {
        DrawPixel( x1 + 1, y1 + 1, color );
        DrawPixel( x2 - 1, y1 + 1, color );
        DrawPixel( x1 + 1, y2 - 1, color );
        DrawPixel( x2 - 1, y2 - 1, color );
        DrawHLine( x1 + 2, y1, x2 - x1 - 4, color );
        DrawHLine( x1 + 2, y2, x2 - x1 - 4, color );
        DrawVLine( x1, y1 + 2, y2 - y1 - 4, color );
        DrawVLine( x2, y1 + 2, y2 - y1 - 4, color );
    }
}

void LCD::FillRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color )
{
    
    
    if ( x1 > x2 ) swap( ushort, x1, x2 );
    if ( y1 > y2 ) swap( ushort, y1, y2 );

    for ( int i = 0; i < ( ( y2 - y1 ) / 2 ) + 1; i++ )
    {
        DrawHLine( x1, y1 + i, x2 - x1, color );
        DrawHLine( x1, y2 - i, x2 - x1, color );
    }
       
}

void LCD::FillRoundRect( unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, int color )
{
    if ( x1 > x2 ) swap( ushort, x1, x2 )
    if ( y1 > y2 ) swap( ushort, y1, y2 )

    if ( ( x2 - x1 ) > 4 && ( y2 - y1 ) > 4 )
    {
        for ( int i = 0; i < ( ( y2 - y1 ) / 2 ) + 1; i++ )
        {
            switch ( i )
            {
                case 0:
                    DrawHLine( x1 + 2, y1 + i, x2 - x1 - 4, color );
                    DrawHLine( x1 + 2, y2 - i, x2 - x1 - 4, color );
                    break;

                case 1:
                    DrawHLine( x1 + 1, y1 + i, x2 - x1 - 2, color );
                    DrawHLine( x1 + 1, y2 - i, x2 - x1 - 2, color );
                    break;

                default:
                    DrawHLine( x1, y1 + i, x2 - x1, color );
                    DrawHLine( x1, y2 - i, x2 - x1, color );
                    break;
            }
        }
    }
}

void LCD::DrawCircle( unsigned short x, unsigned short y, unsigned short radius, int color )
{
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x1 = 0;
    int y1 = radius;
    unsigned int usedColor = color == -1 ? _background : color == -2 ? _foreground : ( unsigned int ) color;

    Activate();
    SetXY( x, y + radius, x, y + radius );
    SetPixelColor( usedColor );
    SetXY( x, y - radius, x, y - radius );
    SetPixelColor( usedColor );
    SetXY( x + radius, y, x + radius, y );
    SetPixelColor( usedColor );
    SetXY( x - radius, y, x - radius, y );
    SetPixelColor( usedColor );

    while ( x1 < y1 )
    {
        if ( f >= 0 )
        {
            y1--;
            ddF_y += 2;
            f += ddF_y;
        }
        x1++;
        ddF_x += 2;
        f += ddF_x;
        SetXY( x + x1, y + y1, x + x1, y + y1 );
        SetPixelColor( usedColor );
        SetXY( x - x1, y + y1, x - x1, y + y1 );
        SetPixelColor( usedColor );
        SetXY( x + x1, y - y1, x + x1, y - y1 );
        SetPixelColor( usedColor );
        SetXY( x - x1, y - y1, x - x1, y - y1 );
        SetPixelColor( usedColor );
        SetXY( x + y1, y + x1, x + y1, y + x1 );
        SetPixelColor( usedColor );
        SetXY( x - y1, y + x1, x - y1, y + x1 );
        SetPixelColor( usedColor );
        SetXY( x + y1, y - x1, x + y1, y - x1 );
        SetPixelColor( usedColor );
        SetXY( x - y1, y - x1, x - y1, y - x1 );
        SetPixelColor( usedColor );
    }
    Deactivate();
}

void LCD::FillCircle( unsigned short x, unsigned short y, unsigned short radius, int color )
{
    unsigned int usedColor = color == -1 ? _background : color == -2 ? _foreground : ( unsigned int ) color;
    Activate();
    for ( int y1 = -radius; y1 <= radius; y1++ )
        for ( int x1 = -radius; x1 <= radius; x1++ )
            if ( x1 * x1 + y1 * y1 <= radius * radius )
            {
                SetXY( x + x1, y + y1, x + x1, y + y1 );
                SetPixelColor( usedColor );
            }
    Deactivate();
}

void LCD::Print( const char *str, unsigned short x, unsigned short y, int fgColor, int bgColor, unsigned short deg )
{
    
    
    int stl, i;

    stl = strlen( str );

    if ( x == RIGHT )
        x = GetWidth() - ( stl * _font->Width );
    if ( x == CENTER )
        x = ( GetWidth() - ( stl * _font->Width ) ) / 2;

    for ( i = 0; i < stl; i++ )
        if ( deg == 0 )
            PrintChar( *str++, x + ( i * ( _font->Width ) ), y, fgColor, bgColor );
        else
            RotateChar( *str++, x, y, i, fgColor, bgColor, deg );
              
}

void LCD::DrawBitmap( unsigned short x, unsigned short y, const bitmap_t* img, unsigned char scale )
{
    
    
    int tx, ty, tc, tsx, tsy;

    Activate();
    if ( scale == 1 )
    {
        SetXY( x, y, x + img->Width - 1, y + img->Height - 1 );

        if ( img->Format == RGB16 )
        {
            const unsigned short *pixel = ( const unsigned short* ) img->PixelData;
            for ( tc = 0; tc < ( img->Width * img->Height ); tc++ )
                SetPixelColor( *pixel++, img->Format );
        }
        else if ( img->Format == RGB18 )
        {
            const unsigned int *pixel = ( const unsigned int* ) img->PixelData;
            for ( tc = 0; tc < ( img->Width * img->Height ); tc++ )
                SetPixelColor( *pixel++, img->Format );
        }
    }
    else
    {
        if ( img->Format == RGB16 )
        {
            const unsigned short *pixel = ( const unsigned short* ) img->PixelData;
            
            for ( ty = 0; ty < img->Height; ty++ )
            {
                SetXY( x, y + ( ty * scale ), x + ( ( img->Width * scale ) - 1 ), y + ( ty * scale ) + scale );
                for ( tsy = 0; tsy < scale; tsy++ )
                {
                    for ( tx = 0; tx < img->Width; tx++ )
                    {
                        for ( tsx = 0; tsx < scale; tsx++ )
                            SetPixelColor( pixel[ ( ty * img->Width ) + tx ], img->Format );
                    }
                }
            }
        }
        else if ( img->Format == RGB18 )
        {
            const unsigned int *pixel = ( const unsigned int* ) img->PixelData;
            
            for ( ty = 0; ty < img->Height; ty++ )
            {
                SetXY( x, y + ( ty * scale ), x + ( ( img->Width * scale ) - 1 ), y + ( ty * scale ) + scale );
                for ( tsy = 0; tsy < scale; tsy++ )
                {
                    for ( tx = 0; tx < img->Width; tx++ )
                    {
                        for ( tsx = 0; tsx < scale; tsx++ )
                            SetPixelColor( pixel[ ( ty * img->Width ) + tx ], img->Format );
                    }
                }
            }
        }
    }
    Deactivate();
        
}

void LCD::DrawBitmap( unsigned short x, unsigned short y, const bitmap_t* img, unsigned short deg, unsigned short rox, unsigned short roy )
{
  
    
    int tx, ty, newx, newy;
    double radian;
    radian = deg * 0.0175;

    if ( deg == 0 )
        DrawBitmap( x, y, img );
    else
    {
        Activate();
        
        if ( img->Format == RGB16 )
        {
            const unsigned short *pixel = ( const unsigned short* ) img->PixelData;
            
            for ( ty = 0; ty < img->Height; ty++ )
                for ( tx = 0; tx < img->Width; tx++ )
                {
                    newx = x + rox + ( ( ( tx - rox ) * cos( radian ) ) - ( ( ty - roy ) * sin( radian ) ) );
                    newy = y + roy + ( ( ( ty - roy ) * cos( radian ) ) + ( ( tx - rox ) * sin( radian ) ) );
    
                    SetXY( newx, newy, newx, newy );
                    SetPixelColor( pixel[ ( ty * img->Width ) + tx ], img->Format );
                }
        }
        else if ( img->Format == RGB18 )
        {
            const unsigned int *pixel = ( const unsigned int* ) img->PixelData;
            
            for ( ty = 0; ty < img->Height; ty++ )
                for ( tx = 0; tx < img->Width; tx++ )
                {
                    newx = x + rox + ( ( ( tx - rox ) * cos( radian ) ) - ( ( ty - roy ) * sin( radian ) ) );
                    newy = y + roy + ( ( ( ty - roy ) * cos( radian ) ) + ( ( tx - rox ) * sin( radian ) ) );
    
                    SetXY( newx, newy, newx, newy );
                    SetPixelColor( pixel[ ( ty * img->Width ) + tx ], img->Format );
                }
        }
        Deactivate();
    }
   
}

inline
void LCD::Activate( void )
{
    _lcd_pin_cs = LOW;
}

inline
void LCD::Deactivate( void )
{
    _lcd_pin_cs = HIGH;
}

inline
void LCD::WriteCmdData( unsigned short cmd, unsigned short data )
{
    WriteCmd( cmd );
    WriteData( data );
}

inline
void LCD::ClearXY( void )
{
    SetXY( 0, 0, GetWidth() - 1, GetHeight() - 1 );
}

void LCD::DrawHLine( unsigned short x, unsigned short y, unsigned short len, int color )
{
    unsigned int usedColor = color == -1 ? _background : color == -2 ? _foreground : ( unsigned int ) color;
    
    Activate();
    SetXY( x, y, x + len, y );
    for ( int i = 0; i < len + 1; i++ )
        SetPixelColor( usedColor );
    Deactivate();
}

void LCD::DrawVLine( unsigned short x, unsigned short y, unsigned short len, int color )
{
    unsigned int usedColor = color == -1 ? _background : color == -2 ? _foreground : ( unsigned int ) color;
    
    Activate();
    SetXY( x, y, x, y + len );
    for ( int i = 0; i < len; i++ )
        SetPixelColor( usedColor );
    Deactivate();
}

void LCD::PrintChar( char c, unsigned short x, unsigned short y, int fgColor, int bgColor )
{
    uint8_t i, ch;
    uint16_t j;
    unsigned int usedColorFG = fgColor == -1 ? _background : fgColor == -2 ? _foreground : ( unsigned int ) fgColor;
    unsigned int usedColorBG = bgColor == -1 ? _background : bgColor == -2 ? _foreground : ( unsigned int ) bgColor;
    
    uint16_t totalCharBytes = ( _font->Width * _font->Height ) / 8;
    int16_t position = _font->Position[ c - _font->Offset ];
    if ( position == -1 ) position = 0; // will print space character
    
    Activate();

    SetXY( x, y, x + _font->Width - 1, y + _font->Height - 1 );
    
    for ( j = 0; j < totalCharBytes; j++ )
    {
        ch = _font->Data[ position ];
        for ( i = 0; i < 8; i++ )
        {
            if ( ( ch & ( 1 << ( 7 - i ) ) ) != 0 ) SetPixelColor( usedColorFG );
            else SetPixelColor( usedColorBG );
        }
        position++;
    }
    Deactivate();
}

void LCD::RotateChar( char c, unsigned short x, unsigned short y, int pos, int fgColor, int bgColor, unsigned short deg )
{
    uint8_t i, j, ch;
    int newx, newy;
    double radian;
    radian = deg * 0.0175;
    
    unsigned int usedColorFG = fgColor == -1 ? _background : fgColor == -2 ? _foreground : ( unsigned int ) fgColor;
    unsigned int usedColorBG = bgColor == -1 ? _background : bgColor == -2 ? _foreground : ( unsigned int ) bgColor;

    int16_t position = _font->Position[ c - _font->Offset ];
    if ( position == -1 ) position = 0; // will print space character
    
    Activate();
    
    for ( j = 0; j < _font->Height; j++ )
    {
        for ( uint16_t zz = 0; zz < ( ( double ) _font->Width / 8 ); zz++ )
        {
            ch = _font->Data[ position + zz ];
            for ( i = 0; i < 8; i++ )
            {
                newx = x + ( ( ( i + ( zz * 8 ) + ( pos * _font->Width ) ) * cos( radian ) ) - ( ( j ) * sin( radian ) ) );
                newy = y + ( ( ( j ) * cos( radian ) ) + ( ( i + ( zz * 8 ) + ( pos * _font->Width ) ) * sin( radian ) ) );

                SetXY( newx, newy, newx + 1, newy + 1 );

                if ( ( ch & ( 1 << ( 7 - i ) ) ) != 0 ) SetPixelColor( usedColorFG );
                else SetPixelColor( usedColorBG );
            }
        }
        position += ( _font->Width / 8 );
    }

    Deactivate();
}

