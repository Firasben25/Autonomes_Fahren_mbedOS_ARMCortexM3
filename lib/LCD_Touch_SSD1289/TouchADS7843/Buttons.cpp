#include "Buttons.h"
#include "mbed.h"

Button::Button()
{
}   

void Button::Create(const char* name, unsigned short x1, unsigned short y1,SSD1289_LCD *lcd, TouchScreenADS7843 *touch, unsigned short x2, unsigned short y2)
{

    int i = 0;
    int n = 0;
    int extra = 0;
    
    while(name[i]!='\0')
    {
        n++;
        i++;
    }    
    
    if(i<=8)
        extra = 10;

    t_name = name;
    t_x1 = x1;
    t_y1 = y1;
    
    if(x2 == 0)
        t_x2 = x1 + n*9 + extra;    // 9 Pixel pro Zeichen 
    else
        t_x2 = x2;
    
    if(y2 == 0)
        t_y2 = y1 + 30;
    else        
    t_y2 = y2;
    
    LCD = lcd; 
    TOUCH = touch;
    button_touched = false;
    touchshort = false;
}


void Button::Draw(int fgColor , int bgColor )
{  
    int i = 0;
    int n = 0;
    
    while(t_name[i]!='\0')
    {
        n++;
        i++;
    }    
    

    LCD->FillRect(t_x1, t_y1, t_x2, t_y2, bgColor);
    LCD->Print(t_name, t_x1+((t_x2-t_x1)-8*n)/2, t_y1+((t_y2-t_y1)/2)-5, fgColor, bgColor);
}

void Button::DrawTouched(int fgColor, int bgColor, bool shortly)
{
    Draw(fgColor , bgColor);
    
    if(shortly)
    {   
        wait(0.3);
        Draw();
        wait(0.1);
    }
}    

void Button::DrawRect()
{
    LCD->FillRect(t_x1, t_y1, t_x2, t_y2, COLOR_BLUE);
}    

void Button::Print(const char* t_name)
{
    LCD->Print(t_name, t_x1+10, t_y1+((t_y2-t_y1)/2)-5, COLOR_WHITE, COLOR_BLUE);
}
     
int Button::GetButton_touched()
{
    if(TOUCH->Read_Ads7843())
        TOUCH->getDisplayPoint();
    
    else
        return 0;    
        
    if((TOUCH->display.x > t_x1 && TOUCH->display.x < t_x2) && (TOUCH->display.y > t_y1 && TOUCH->display.y < t_y2))
    {
        TOUCH->display.x = 0;
        TOUCH->display.y = 0;    
        
        if(button_touched)    
            button_touched = false;
        else
            button_touched = true;
        
        return 1;   
    }  
    
    return 0;
}    
  
void Button::ResetButtonTouched()
{
    if(button_touched)
    {
        Draw();
        button_touched = false;
    }    
}    

void Button::ResetButtonTouchedonlyStatus()
{
    if(button_touched)
        button_touched = false; 
}    

int GetSelectedButton(Button *button, TouchScreenADS7843 *TOUCH, int button_first, int button_last )
{
    int i;
    
    if(TOUCH->Read_Ads7843())
        TOUCH->getDisplayPoint();
        
    else
        return -1;
        
     
    for(i=button_first; i<=button_last;i++)   
    {
        if((TOUCH->display.x > button[i].t_x1 && TOUCH->display.x < button[i].t_x2) && (TOUCH->display.y > button[i].t_y1 && TOUCH->display.y < button[i].t_y2))
        {
            TOUCH->display.x = 0;
            TOUCH->display.y = 0;
            
            if( button[i].button_touched)    
                 button[i].button_touched = false;
                
            else
                 button[i].button_touched = true;
                
            return i;    
        }    
    }   
    return -1;
} 


int DrawPreviousMenu(Button *button, int button_first, int button_last, SSD1289_LCD *LCD)
{
    int i, j, t = 0;
    
    for(i=button_first;i<=button_last;i++)
    {
        if(button[i].button_touched)
        {
            t = 1;
            
            LCD->FillScreen(COLOR_WHITE);
            
            for(j=button_first;j<=button_last;j++)
            {
                button[j].Draw();
                button[j].button_touched = false;
            }   
#ifdef  Tests            
            if(button_first != 0)
               button[ZURUECK].Draw();
#endif
        }  
    }      
      
   return t;       
}    

void WaitUntilNoTouch(TouchScreenADS7843 *TOUCH)
{
    while(!TOUCH->_tp_irq);
}  


void ResetButtonsTouched(Button *button, int button_first, int button_last, SSD1289_LCD *LCD , int skip)
{
    int i;
    
    for(i=button_first;i<=button_last;i++)
    {
        if(i != skip)
        {
            if(button[i].button_touched)
            {
                button[i].Draw();
                button[i].button_touched = false;
            }
        }
    }
}

void DrawLastButtonsStatus(Button *button, int button_first, int button_last, SSD1289_LCD *LCD, int skip)
{
    int i;
    
    for(i=button_first;i<=button_last;i++)
    {
        if(i != skip)
        {
            if(button[i].button_touched && !button[i].touchshort)
                button[i].DrawTouched();

            
            else if(button[i].touchshort)
                button[i].DrawTouched(COLOR_WHITE, COLOR_CYAN);
                
            else if(!button[i].button_touched)
                button[i].Draw();
        }
    }
}