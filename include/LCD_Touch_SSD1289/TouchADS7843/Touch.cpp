/**************************************************************************************************
 *****                                                                                        *****
 *****  Name: Touch.cpp                                                                       *****
 *****  Ver.: 1.0                                                                             *****
 *****  Date: 04/01/2013                                                                      *****
 *****  Auth: Frank Vannieuwkerke                                                             *****
 *****        Erik Olieman                                                                    *****
 *****  Func: Touch driver for use with ADS7843                                               *****
 *****                                                                                        *****
 *****  Code based on Carlos E. Vidales tutorial :                                            *****
 *****  How To Calibrate Touch Screens                                                        *****
 *****  www.embedded.com/design/configurable-systems/4023968/How-To-Calibrate-Touch-Screens   *****
 *****                                                                                        *****
 **************************************************************************************************/

#include "Touch.h"
#include "mbed.h"
//#include "Arial12x12.h"

#define THRESHOLD 2

TouchScreenADS7843::TouchScreenADS7843(PinName tp_mosi, PinName tp_miso, PinName tp_sclk, PinName tp_cs, PinName tp_irq, /*ILI9328_LCD *_LCD*/SSD1289_LCD *_LCD )
        : LCD(_LCD), _tp_spi(tp_mosi, tp_miso, tp_sclk), _tp_cs(tp_cs), _tp_irq(tp_irq)
        {
            DisplaySample[0].x=45;
            DisplaySample[0].y=45;
            DisplaySample[1].x=45;
            DisplaySample[1].y=270;
            DisplaySample[2].x=190;
            DisplaySample[2].y=190;
            ScreenSample[0].x=45;
            ScreenSample[0].y=45;
            ScreenSample[1].x=45;
            ScreenSample[1].y=270;
            ScreenSample[2].x=190;
            ScreenSample[2].y=190;
            _tp_cs=1;
            _tp_spi.frequency(500000);
            _tp_spi.format(8,0);                    // 8 bit spi mode 0
        }

int TouchScreenADS7843::Read_XY(unsigned char XY)
{
    unsigned char msb, lsb;
    unsigned int Temp;
    
    Temp=0;
    _tp_cs=0;
    wait_us(SPI_RD_DELAY);
    _tp_spi.write(XY);
    wait_us(SPI_RD_DELAY);
    msb = _tp_spi.write(0x00);  // msb
    wait_us(SPI_RD_DELAY);
    lsb = _tp_spi.write(0x00);  // lsb
    _tp_cs=1;
    Temp = ((msb << 8 ) | lsb);
    Temp >>= 3;
    Temp &= 0xfff;
    Temp /= 4;                  // Scaling : return value range must be between 0 and 1024
    return(Temp);
}


void TouchScreenADS7843::TP_GetAdXY(int *x,int *y)
{
    int adx,ady;
    adx = Read_XY(CHX);
    wait_us(1);
    ady = Read_XY(CHY);
    *x = adx;
    *y = ady;
}
/*
void TouchScreenADS7843::TP_DrawPoint(unsigned int Xpos,unsigned int Ypos, unsigned int color)
{
    LCD->WriteCmdData(0x03, 0x1030);
//    LCD->WindowMax();
    LCD->DrawPixel(Xpos,Ypos,color);
    LCD->DrawPixel(Xpos+1,Ypos,color);
    LCD->DrawPixel(Xpos,Ypos+1,color);
    LCD->DrawPixel(Xpos+1,Ypos+1,color);
}
*/
void TouchScreenADS7843::DrawCross(unsigned int Xpos,unsigned int Ypos)
{
    LCD->DrawLine(Xpos-15,Ypos,Xpos-2,Ypos,COLOR_WHITE);
    LCD->DrawLine(Xpos+2,Ypos,Xpos+15,Ypos,COLOR_WHITE);
    LCD->DrawLine(Xpos,Ypos-15,Xpos,Ypos-2,COLOR_WHITE);
    LCD->DrawLine(Xpos,Ypos+2,Xpos,Ypos+15,COLOR_WHITE);

    LCD->DrawLine(Xpos-15,Ypos+15,Xpos-7,Ypos+15,COLOR_BLACK);
    LCD->DrawLine(Xpos-15,Ypos+7,Xpos-15,Ypos+15,COLOR_BLACK);

    LCD->DrawLine(Xpos-15,Ypos-15,Xpos-7,Ypos-15,COLOR_BLACK);
    LCD->DrawLine(Xpos-15,Ypos-7,Xpos-15,Ypos-15,COLOR_BLACK);

    LCD->DrawLine(Xpos+7,Ypos+15,Xpos+15,Ypos+15,COLOR_BLACK);
    LCD->DrawLine(Xpos+15,Ypos+7,Xpos+15,Ypos+15,COLOR_BLACK);

    LCD->DrawLine(Xpos+7,Ypos-15,Xpos+15,Ypos-15,COLOR_BLACK);
    LCD->DrawLine(Xpos+15,Ypos-15,Xpos+15,Ypos-7,COLOR_BLACK);
}

unsigned char TouchScreenADS7843::Read_Ads7843(Coordinate * screenPtr)
{
    int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
    uint8_t count=0;
    int buffer[2][9]={{0},{0}};
    if (screenPtr == NULL) screenPtr = &screen;
    do
    {
        TP_GetAdXY(TP_X,TP_Y);
        buffer[0][count]=TP_X[0];
        buffer[1][count]=TP_Y[0];
        count++;
    }
    while(!_tp_irq && (count < 9));
    if(count==9)
    {
        temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
        temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
        temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        if( (m0>THRESHOLD)  &&  (m1>THRESHOLD)  &&  (m2>THRESHOLD) ) return 0;
        if(m0<m1)
        {
            if(m2<m0)
                screenPtr->x=(temp[0]+temp[2])/2;
            else
                screenPtr->x=(temp[0]+temp[1])/2;
        }
        else if(m2<m1)
            screenPtr->x=(temp[0]+temp[2])/2;
        else
            screenPtr->x=(temp[1]+temp[2])/2;

        temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
        temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
        temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        if( (m0>THRESHOLD)  &&  (m1>THRESHOLD)  &&  (m2>THRESHOLD) ) return 0;

        if(m0<m1)
        {
            if(m2<m0)
                screenPtr->y=(temp[0]+temp[2])/2;
            else
                screenPtr->y=(temp[0]+temp[1])/2;
        }
        else if(m2<m1)
            screenPtr->y=(temp[0]+temp[2])/2;
        else
            screenPtr->y=(temp[1]+temp[2])/2;
        return 1;
    }
    return 0;
}

uint8_t TouchScreenADS7843::setCalibrationMatrix( Coordinate * displayPtr,
        Coordinate * screenPtr,
        Matrix * matrixPtr)
{
    uint8_t retTHRESHOLD = 0 ;
    // K = (Xs0 - Xs2)*(Ys1 - Ys2) - (Xs1 - Xs2)*(Ys0 - Ys2)
    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                         ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
    if( matrixPtr->Divider == 0 )
    {
        retTHRESHOLD = 1;
    }
    else
    {
        //                 (Xd0 - Xd2)*(Ys1 - Ys2) - (Xd1 - Xd2)*(Ys0 - Ys2)
        //            A = ---------------------------------------------------
        //                                   K
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                          ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
        //                 (Xs0 - Xs2)*(Xd1 - Xd2) - (Xd0 - Xd2)*(Xs1 - Xs2)
        //            B = ---------------------------------------------------
        //                                   K
        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                        ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
        //                 Ys0*(Xs2*Xd1 - Xs1*Xd2) + Ys1*(Xs0*Xd2 - Xs2*Xd0) + Ys2*(Xs1*Xd0 - Xs0*Xd1)
        //            C = ----------------------------------------------------------------------------
        //                                   K
        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
        //                 (Yd0 - Yd2)*(Ys1 - Ys2) - (Yd1 - Yd2)*(Ys0 - Ys2)
        //            D = ---------------------------------------------------
        //                                   K
        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                        ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
        //                 (Xs0 - Xs2)*(Yd1 - Yd2) - (Yd0 - Yd2)*(Xs1 - Xs2)
        //            E = ---------------------------------------------------
        //                                   K
        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                        ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
        //                 Ys0*(Xs2*Yd1 - Xs1*Yd2) + Ys1*(Xs0*Yd2 - Xs2*Yd0) + Ys2*(Xs1*Yd0 - Xs0*Yd1)
        //            F = ----------------------------------------------------------------------------
        //                                   K
        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
    }
    return( retTHRESHOLD ) ;
}

uint8_t TouchScreenADS7843::getDisplayPoint(void)
{
    uint8_t retTHRESHOLD = 0 ;

    if( matrix.Divider != 0 )
    {
        // XD = AX+BY+C
        display.x = ( (matrix.An * screen.x) +
                          (matrix.Bn * screen.y) +
                           matrix.Cn
                         ) / matrix.Divider ;
        // YD = DX+EY+F
        display.y = ( (matrix.Dn * screen.x) +
                          (matrix.En * screen.y) +
                           matrix.Fn
                         ) / matrix.Divider ;
    }
    else
    {
        retTHRESHOLD = 1;
    }
    return(retTHRESHOLD);
}

void TouchScreenADS7843::TouchPanel_Calibrate(void)
{
    char buff[10];    //new
    uint8_t i;
    Coordinate screen_cal;
    setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix) ;
    LCD->SetFont(&TerminusFont);
    for(i=0;i<3;i++)
    {
        LCD->ClearScreen();
//        LCD->locate(10,10);
        LCD->Print("Touch crosshair to calibrate", 10, 10, COLOR_BLACK, COLOR_BLUE );
        wait_ms(500);
        DrawCross(DisplaySample[i].x,DisplaySample[i].y);
        do {} while (!Read_Ads7843(&screen_cal));
        ScreenSample[i].x= screen_cal.x;ScreenSample[i].y= screen_cal.y;
//////////////////////////////////////////new
        sprintf (buff, "%d", screen_cal.x); 
        LCD->Print("Touch x-Koordinate", 10, 40, COLOR_WHITE, COLOR_BLUE);
        LCD->Print(buff, 10, 50, COLOR_WHITE, COLOR_BLUE);

        sprintf (buff, "%d", screen_cal.y); 
        LCD->Print("Touch y-Koordinate", 10, 90, COLOR_WHITE, COLOR_BLUE);
        LCD->Print(buff, 10, 100, COLOR_WHITE, COLOR_BLUE);
        
        wait(0.5);        
///////////////////////////////////////////////////////////
    }
    setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix) ;
    LCD->ClearScreen();
}

void TouchScreenADS7843::GetCalibration(Matrix * matrixPtr, Coordinate * screenPtr)
{
    uint8_t i;
    Matrix * mp1;
    mp1 = &matrix;
    *matrixPtr = *mp1;
    for(i=0;i<3;i++)
    {
        screenPtr[i].x = ScreenSample[i].x;
        screenPtr[i].y = ScreenSample[i].y;
    }
}

void TouchScreenADS7843::SetCalibration(Matrix * matrixPtr, Coordinate * screenPtr)
{
    uint8_t i;
    Matrix * mp1;
    mp1 = &matrix;
    *mp1 = *matrixPtr;
    for(i=0;i<3;i++)
    {
        ScreenSample[i].x = screenPtr[i].x;
        ScreenSample[i].y = screenPtr[i].y;
    }
    setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix) ;
}

///////////////////////////////////////////////////////////////////////////

void TouchScreenADS7843::setCalibrationwithoutUser()
{
    ScreenSample[0].x = 225;
    ScreenSample[1].x = 269;
    ScreenSample[2].x = 815;
    
    ScreenSample[0].y = 839;
    ScreenSample[1].y = 227;
    ScreenSample[2].y = 446;
    
    setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix) ;        
}

