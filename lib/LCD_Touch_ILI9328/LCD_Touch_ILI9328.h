/******************************************************************
 *****                                                        *****
 *****  Name: LCD_Touch_ILI9328.h                             *****
 *****  Ver.: 0.1                                             *****
 *****  Date: 04/01/2013                                      *****
 *****  Auth: Jan-Hendrik Aschen  B.Eng.                      *****
 *****                                                        *****
 *****  Func: Touch/LCD driver for use with ADS7843/SSD1289   *****
 *****                                                        *****
 ******************************************************************/
#ifndef LCD_Touch_ILI9328_H
#define LCD_Touch_ILI9328_H

#include "mbed.h"
#include "Touch.h"
#include "Buttons.h"
#include "ili9328.h"

// Beispiel Deklaration LCD/Touch


/*
#include "mbed.h"
#include "LCD_Touch_ILI9328.h"
*/
/*****Pin Connections to**************/
/*****Data Bus, LCD, Touch**/
/*
BusOut dataBus( p26, p25, p24, p23, p22, p21, P2_6, P2_7); // 8 pins
ILI9328_LCD lcd( P0_22, p15, P0_24, P0_21, P0_19, P0_20, &dataBus, P0_25 );// control pins and data bus  CS, RS, WR, DIR, EN, LE, DATA_PORT, RD
TouchScreenADS7843 touch(p5, p6, p7, p8, P2_13, &lcd);     // ADS7843 -> mosi, miso, sclk, cs, irq, SPI_TFT


int main()
{

    lcd.Initialize(PORTRAIT_REV);

    lcd.FillScreen(COLOR_WHITE);
    lcd.SetFont(&TerminusFont);
    //touch.setCalibrationwithoutUser();
    touch.TouchPanel_Calibrate();
}
*/



#endif
