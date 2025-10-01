/******************************************************************
 *****                                                        *****
 *****  Name: LCD_Touch_SDD1289.h                             *****
 *****  Ver.: 0.2                                             *****
 *****  Date: 10/10/2016                                      *****
 *****  Auth: Jan-Hendrik Aschen  B.Eng.                      *****
 *****                                                        *****
 *****  Func: Touch/LCD driver for use with ADS7843/SSD1289   *****
 *****                                                        *****
 ******************************************************************/
#ifndef LCD_Touch_SSD1289_H
#define LCD_Touch_SSD1289_H

#include "mbed.h"
#include "ssd1289.h"
#include "Touch.h"
#include "Buttons.h"

// Beispiel Deklaration LCD/Touch


/*
#include "mbed.h"
#include "LCD_Touch_SSD1289.h"
*/

/*****Pin Connections to**************/
/*****Data Bus, LCD, Touch**/
/*
 BusOut dataBus( p26, p25, p24, p23, p22, p21, P2_6, P2_7); // 8 pins
 SSD1289_LCD lcd( P0_22, p15, P0_24, P0_21, P0_19, P0_20, &dataBus, P0_25 );// control pins and data bus  CS, RS, WR, DIR, EN, LE, DATA_PORT, RD
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