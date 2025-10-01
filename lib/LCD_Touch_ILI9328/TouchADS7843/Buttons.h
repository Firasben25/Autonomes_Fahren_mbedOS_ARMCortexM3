/******************************************************************
 *****                                                        *****
 *****  Name: Buttons.h                                       *****
 *****  Ver.: 1.0                                             *****
 *****  Date: 03/04/2015                                      *****
 *****  Auth: Gregor Mraczny                                  *****
 *****  Func: Touch Buttons                                   *****
 *****                                                        *****
 ******************************************************************/
 
#ifndef BUTTONS_H
#define BUTTONS_H 
 
#include "mbed.h"
#include "Touch.h"
#include "ili9328.h"


class Button{
public:
    /*
    * Create a Button object with Touch display interactions 
    * 
    * @param name displays on the button
    * @param x1, y1, x2, y2 positon parameters on the display in Pixel
    * @param pointer to the TFT constructer 
    * @param poniter to the TOUCH constructer
    *
    */
    //Button( const char* name, unsigned short x1, unsigned short y1, ILI9328_LCD *lcd, TouchScreenADS7843 *touch, unsigned short x2, unsigned short y2);
    
    Button();
    /*
    * Draw the button with the given Positions values on the display 
    * default Color are blue and font color white
    * 
    * @param font Color
    * @param rectangle Color of the Button 
    *
    */
    void Draw(int fgColor = COLOR_WHITE, int bgColor = COLOR_BLUE );
    
    /*
    * Draw the button red to show that the button is touched 
    * on the display
    *
    */   
    void DrawTouched(int fgColor = COLOR_WHITE, int bgColor = COLOR_RED, bool shortly = false);
    
    /*
    * Draw only the rectangle of the button on the display 
    *
    */
    void DrawRect();
    
    /*
    * Draw only the name of the button on the display 
    *
    */
    void Print( const char* t_name);
    
    /*
    * Set the parameters of a new button object
    *
    */    
    void Create( const char* name, unsigned short x1, unsigned short y1, ILI9328_LCD *lcd, TouchScreenADS7843 *touch, unsigned short x2 = 0, unsigned short y2 = 0);
    
    /*
    * Checks if the button is touched 
    * default Color are blue and font color white
    * 
    * @returns
    *   0 no touchsignal on the display or the button is not touched
    *   1 the button is touched
    */
    int GetButton_touched();
    
    /*
    * Reset the buttons to standard color if the button was 
    *touched and set button_touched to false
    */
    void ResetButtonTouched();
    
    /*
    * Reset only the button_touched Status
    `*/
    void ResetButtonTouchedonlyStatus();
    
    ILI9328_LCD    *LCD;
    TouchScreenADS7843 *TOUCH;
    unsigned short t_x1;
    unsigned short t_x2;
    unsigned short t_y1;
    unsigned short t_y2;
    const char * t_name;
    bool button_touched;
    bool touchshort;

protected:        
};    

    /*
    * Checks which Button is touched,
    * the givinig buttons numbers must be in order
    * and the created button object is an array 
    *
    * @param first button to be checked
    * @param last button to be checked 
    *
    * @returns
    *   0 no touchsignal on the display or a button from the giving param is not touched
    *   1 a button from the giving param is touched
    */
    int GetSelectedButton(Button *button, TouchScreenADS7843 *touch, int button_first, int button_last);

    /*
    * If return from Submenu to Menu draw the Buttons from the Menu,
    * the givinig buttons numbers must be in order
    * and the created button object is an array 
    *
    * @param first button to be checked
    * @param last button to be checked 
    *
    *
    * @returns
    *   0 no given button was touched
    *   1 a given button was touched
    */
    int DrawPreviousMenu(Button *button, int button_first, int button_last, ILI9328_LCD *LCD);
    
    /*
    * Until there is no more touch signal on the display
    * do nothing
    */
    void WaitUntilNoTouch(TouchScreenADS7843 *TOUCH);
    
    /*
    * Reset the buttons to standard color if the button was 
    *touched and set button_touched to false
    */
    void ResetButtonsTouched(Button *button, int button_first, int button_last, ILI9328_LCD *LCD, int skip = -1);
    
    /*
    * If switching between menus, draw last status of the buttons
    */
    void DrawLastButtonsStatus(Button *button, int button_first, int button_last, ILI9328_LCD *LCD, int skip = -1);

#endif
