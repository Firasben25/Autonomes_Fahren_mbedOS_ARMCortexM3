
#include "Tasks.h"
#include "mbed.h"
#include "LCD_Touch_SSD1289.h"
#include "SRF08.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Makros 
#define testen 0
#define quer 1
#define back 2
#define resetAktor 3
#define ANZAHL_BUTTONS 4

//PWM-Werten
PwmOut Motor(P1_21);
PwmOut Lenkung(P1_20);

//Kennlinie Infrarotsensoren 
static int GP2Y[17][2] = {{300, 3}, {272, 4}, {234, 5}, {201, 6}, {176, 7}, {155, 8}, {140, 9}, {126, 10}, {106, 12}, {93, 14}, {82, 16}, {75, 18}, {65, 20}, {52, 25}, {43, 30}, {37, 35}, {30, 40}};


//Festlegen der Infrarot-Sensoren-Pins 
AnalogIn rear_ir(P0_2);
AnalogIn front_ir(P1_31);
AnalogIn left_ir(P0_26);
AnalogIn right_ir(P1_30);

//Festlegen der Ultraschall-Sensoren-Einstellungen 
SRF08 US_R(P0_27, P0_28, 0xE2);
SRF08 US_L(P0_27, P0_28, 0xEA);


//LCD und Touchscreen Einstellungen 
BusOut dataBus(p26, p25, p24, p23, p22, p21, P2_6, P2_7);                // 8 pins
SSD1289_LCD lcd(P0_22, p15, P0_24, P0_21, P0_19, P0_20, &dataBus, P0_25);// control pins and data bus  CS, RS, WR, DIR, EN, LE, DATA_PORT, RD
TouchScreenADS7843 touch(p5, p6, p7, p8, P2_13, &lcd);                   // ADS7843 -> mosi, miso, sclk, cs, irq, SPI_TFT

//Initialisieren der Buttons 
Button button[ANZAHL_BUTTONS];

//Anlegen eines Array für die Zykluszeiten und die Counter
int Zyklus[3];
int counterDisplay;
int counterSensor;
int counterAktor;

//Anlegen einer Queue zur Steuerung des Querfuehrung Threads
Queue<int, 8> mq_quer;
Queue<int, 8> mq_exit;

//Anlegen einer Bool für Touch Button 
static bool sbquer = false;

//Defintion der kontinuierlich laufenden Threads 
Thread thread_Task_Sensor(osPriorityNormal, OS_IDLE_THREAD_STACK_SIZE, NULL, "Task_Sensor");     // Task Sensor mit normaler Priorität
Thread thread_Task_Aktor(osPriorityNormal, OS_IDLE_THREAD_STACK_SIZE, NULL, "Task_Aktor");       // Task Aktor mit normaler Priorität
Thread thread_Task_Display(osPriorityNormal, OS_STACK_SIZE, NULL, "Task_Display");               // Task Display mit normaler Priorität
Thread thread_Task_Querfuehrung(osPriorityAboveNormal, OS_STACK_SIZE, NULL, "Task_Querfuehrung");  // Task Folgefahrt mit hoher Priorität

//Definition der Timer
Timer t_sensor;
Timer t_aktor;
Timer t_display;
Timer t_sensorext;
Timer t_aktorext;
Timer t_displayext;

//Initialisierung der Mutex 
Mutex lcd_mutex;
Mutex sensor_mutex;
Mutex aktor_mutex;

//Deklaration der Sensor-Variablen 
uint16_t Messwert_Rear_IR;
uint16_t Messwert_Front_IR;
uint16_t Messwert_Left_IR;
uint16_t Messwert_Right_IR;
uint16_t Messwert_USR;
uint16_t Messwert_USL;

//Definition der Aktor-Variablen 
float MotorPWM = 0.0015;
float LenkPWM = 0.0015;

//Interpolation für die Infrarotsensoren 
uint16_t interpolation(float dspannung)    // Interpolation der Infrarotsensoren
{
    int spannung = (int)(dspannung * 330); // ADC vergleich 3,3V in % als float

    if (spannung > GP2Y[0][0])
    {
        return (uint16_t)GP2Y[0][1];       // Distanz <= 30mm
    }

    if (spannung < GP2Y[16][0])            // Entfernung zu groß, dann maximalen Wert zurückgeben
    {
        return (uint16_t)GP2Y[16][1];      // Distanz >= 400mm
    }

    for (uint8_t i = 1; i <= 16; i++)      // For-Schleife abgleich des Arrays
    {
        if ((spannung <= GP2Y[i - 1][0]) && (spannung >= GP2Y[i][0])) // liegt der gemessene Wert zwischen zwei Werten dann interpoliere
        {
            return (uint16_t)(GP2Y[i - 1][1] + ((GP2Y[i][1] - GP2Y[i - 1][1]) * (spannung - GP2Y[i - 1][0])) / (GP2Y[i][0] - GP2Y[i - 1][0])); // Berechnung der Entfernung
        }
    }
    return 0;
}

//***Main-Thread 

//Main-Methode 
int main()
{
    
    osThreadSetPriority(osThreadGetId(), osPriorityIdle);      // Priorität des Main-Threads auf niedrigste Stufe stellen
    initializeAll();                                           // Aufruf der Intitalisierungs-Methode

    // Starten der kontinuierlich laufenden Threads
    thread_Task_Sensor.start(callback(Task_Sensor));
    thread_Task_Aktor.start(callback(Task_Aktor));
    thread_Task_Display.start(callback(Task_Display));
    thread_Task_Querfuehrung.start(callback(querfuehrung));

    while (1)
    {
        
        ThisThread::sleep_for(1000);     // Begrenzen des Main-Threads auf 1000ms
    }
}

//Initialisierungs-Mehode 
void initializeAll()
{
    // Initialisierung des LCD-Display und des Touchscreens
    lcd_mutex.lock();

    lcd.Initialize(PORTRAIT_REV);
    lcd.FillScreen(COLOR_WHITE);
    lcd.SetFont(&TerminusFont);
    touch.TouchPanel_Calibrate();

    lcd_mutex.unlock();

    // Initialisierung der Ultraschallsensoren
    sensor_mutex.lock();

    // Setzen des Reichweitenregisters
    US_R.setRangeRegister(0x8c);
    US_L.setRangeRegister(0x8c);

    // Setzen des Verstärkungs Register
    US_R.setMaxGainRegister(0x01);
    US_L.setMaxGainRegister(0x01);

    sensor_mutex.unlock();

    // Initialisierung der Aktoren
    aktor_mutex.lock();

    Motor.period(0.022);
    Motor.pulsewidth(MotorPWM);

    Lenkung.period(0.022);
    Lenkung.pulsewidth(LenkPWM);

    aktor_mutex.unlock();
}




//***Sensor-Thread
//Sensor-Task
void Task_Sensor(void)
{
    while (1) // kontinuierliche Erfassung der Sensorwerte
    {

        sensor_mutex.lock();

        Messwert_Rear_IR = interpolation(rear_ir.read());
        Messwert_Front_IR = interpolation(front_ir.read());
        Messwert_Left_IR = interpolation(left_ir.read());
        Messwert_Right_IR = interpolation(right_ir.read());

        sensor_mutex.unlock();

        // Ultraschallsensoren beginnen zu senden
        US_R.startRanging();
        US_L.startRanging();

        // Stoppen des Timers zur Laufzeitbestimmung
        t_sensor.stop();
        float sleeptime = 100 - t_sensor.read_ms();
        // Thread pausieren vor Einlesen der Ultraschallsensoren
        if (sleeptime > 0)
        {
            ThisThread::sleep_for(sleeptime);
        }
        else
        {
            counterSensor++;
        }
        t_sensorext.stop();
        Zyklus[2] = t_sensorext.read_ms();
        t_sensorext.reset();
        // Reseten und Starten des Timers zur Laufzeitmessung
        t_sensor.reset();
        t_sensor.start();
        t_sensorext.start();

        // Einlesen der Ultraschallsensoren; geschützt durch Mutex
        sensor_mutex.lock();

        Messwert_USL = US_L.getRange();
        Messwert_USR = US_R.getRange();

        sensor_mutex.unlock();
    }
}




//***Aktor-Thread 
//Aktor-Task 
void Task_Aktor(void)
{
    while (1)
    {
        // Starten des Timers zur Laufzeitbestimmung
        t_aktor.start();
        t_aktorext.start();

        // Ansteuern der Aktoren; geschützt durch Mutex
        aktor_mutex.lock();

        Motor.pulsewidth(MotorPWM);
        Lenkung.pulsewidth(LenkPWM);

        aktor_mutex.unlock();

        // Stoppen des Timers zur Laufzeitbestimmung
        t_aktor.stop();

        float sleeptime = 20 - t_aktor.read_ms();

        if (sleeptime > 0)
        {
            ThisThread::sleep_for(sleeptime);
        }
        else
        {
            counterAktor++;
        }

        t_aktorext.stop();
        Zyklus[1] = t_aktorext.read_ms(); // Der Timer wird in das Array Zyklus gespeichert und danach zurückgesetzt
        t_aktorext.reset();
        // Reseten des Timers
        t_aktor.reset();
    }
}



//***Display-Thread 
//Display-Task 
void Task_Display(void)
{
    mainMenu();
}


//Anzeige-Task 
void anzeige()
{
    // Variable zur Ausgabe anlegen
    char Print[50];

    // Anzahl der Threads bestimmen und benötigten Speicher allokieren
    int cnt = osThreadGetCount();
    mbed_stats_thread_t *stats = (mbed_stats_thread_t *)malloc(cnt * sizeof(mbed_stats_thread_t));

    // Timer starten
    t_display.start();
    t_displayext.start();

    // Speicher ausgeben
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    float current = heap_stats.current_size;
    float reserved = heap_stats.reserved_size;
    sprintf(Print, " %.2f%% ", (current / reserved) * 100);
    lcd_mutex.lock();
    lcd.Print(Print, 160, 200, -1, -2, 0);

    // aktive Threads anzeigen
    cnt = mbed_stats_thread_get_each(stats, cnt);
    for (int i = 0; i < cnt; i++)
    {
        if (stats[i].name == "Task_Display")
        {
            sprintf(Print, "%s %d ms cnt:", stats[i].name, Zyklus[0]);
            lcd.Print(Print, 5, 230, -1, -2, 0);
            sprintf(Print, "%d", counterDisplay);
            lcd.Print(Print, 200, 230, -1, -2, 0);
        }
        else if (stats[i].name == "Task_Aktor")
        {
            sprintf(Print, "%s %d ms cnt:", stats[i].name, Zyklus[1]);
            lcd.Print(Print, 5, 245, -1, -2, 0);
            sprintf(Print, "%d", counterAktor);
            lcd.Print(Print, 180, 245, -1, -2, 0);
        }
        else if (stats[i].name == "Task_Sensor")
        {
            sprintf(Print, "%s %d ms cnt:", stats[i].name, Zyklus[2]);
            lcd.Print(Print, 5, 260, -1, -2, 0);
            sprintf(Print, "%d", counterSensor);
            lcd.Print(Print, 190, 260, -1, -2, 0);
        }
        else if (stats[i].name == "Task_Querfuehrung")
        {
            sprintf(Print, "%s", stats[i].name);
            lcd.Print(Print, 5, 275, -1, -2, 0);
        }
    }
    lcd_mutex.unlock();

    // Buttons einlesen
    if (button[testen].GetButton_touched())
    {
        if (sbquer)
        {
            mq_exit.put((int *)1);
            sbquer = false;
            ThisThread::sleep_for(150);
        }
        button[testen].DrawTouched(COLOR_WHITE, COLOR_RED, true);
        Display_Test();
    }
    else if (button[quer].GetButton_touched())
    {
        if (!sbquer)
        {
            sbquer = true;
            button[quer].DrawTouched(COLOR_WHITE, COLOR_RED, true);
            mq_quer.put((int *)1);
            button[quer].ResetButtonTouched();
            Display_querfuehrung();
        }
    }
    else if (button[back].GetButton_touched())
    {
        if (sbquer)
        {
            mq_exit.put((int *)1);
            sbquer = false;
            ThisThread::sleep_for(150);
        }
        button[back].DrawTouched(COLOR_WHITE, COLOR_RED, true);
        mainMenu();
    }

    // Timer stoppen und sleeptime ermitteln
    t_display.stop();
    float sleeptime = 250 - t_display.read_ms();
    if (sleeptime > 0)
    {
        ThisThread::sleep_for(sleeptime);
    }
    else
    {
        counterDisplay++;
    }
    t_displayext.stop();
    Zyklus[0] = t_displayext.read_ms();
    t_display.reset();
    t_displayext.reset();

    // Speicher freigeben
    free(stats);
}


//Hauptmenü-Task 
void mainMenu()
{
    // Setzen der Aktor-PWMs; geschützt durch Mutex
    aktor_mutex.lock();

    MotorPWM = 0.0015;
    LenkPWM = 0.0015;

    aktor_mutex.unlock();

    // Erstellen der Buttons
    button[testen].Create("Test", 5, 5, &lcd, &touch, 70, 55);
    button[quer].Create("Quer", 75, 5, &lcd, &touch, 140, 55);
    button[back].Create("STOP", 145, 5, &lcd, &touch, 210, 55);

    lcd_mutex.lock();

    // Buttons und festen Bildschirm zeichnen
    lcd.FillScreen(COLOR_WHITE);
    button[testen].Draw();
    button[quer].Draw();
    button[back].Draw();
    lcd.Print("Reserve", 5, 200, -1, -2, 0);
    lcd.Print("Aktive Threads:", 5, 220, -1, -2, 0);

    lcd_mutex.unlock();

    // Anzeige Methode aufrufen
    while (1)
    {
        anzeige();
    }
}


//Testmenü-Task
void Display_Test()
{
    // Zeichnen von Text und Knöpfen; geschützt durch Mutex
    lcd_mutex.lock();

    lcd.Print("Rear IR  :      ", 5, 60, -1, -2, 0);
    lcd.Print("Front IR :      ", 5, 70, -1, -2, 0);
    lcd.Print("Left IR  :      ", 5, 80, -1, -2, 0);
    lcd.Print("Right IR :      ", 5, 90, -1, -2, 0);
    lcd.Print("US R     :      ", 5, 100, -1, -2, 0);
    lcd.Print("US L     :      ", 5, 110, -1, -2, 0);

    lcd_mutex.unlock();

    // Aufrufen der Funktion AktorTest wegen zeitlichen Problem der Auslagerung, beispiel wird mitgebracht
    aktorenTest();

    while (1)
    {
        // Ausgabe der Sensor-Werte und der Hauptanzeige
        Sensor_Anzeige();
        anzeige();
        ThisThread::sleep_for(100);
    }
}


//Anzeige der Sensorwerte
void Sensor_Anzeige()
{
    char Ausgabe[20]; // Speichert die Sensorwerte bevor sie auf dem Display ausgegeben werden
    lcd_mutex.lock();
    sensor_mutex.lock();

    sprintf(Ausgabe, "%3d    ", Messwert_Rear_IR);
    lcd.Print(Ausgabe, 110, 60, -1, -2, 0);

    sprintf(Ausgabe, "%3d    ", Messwert_Front_IR);
    lcd.Print(Ausgabe, 110, 70, -1, -2, 0);

    sprintf(Ausgabe, "%3d    ", Messwert_Left_IR);
    lcd.Print(Ausgabe, 110, 80, -1, -2, 0);

    sprintf(Ausgabe, "%3d    ", Messwert_Right_IR);
    lcd.Print(Ausgabe, 110, 90, -1, -2, 0);

    sprintf(Ausgabe, "%3d    ", Messwert_USR);
    lcd.Print(Ausgabe, 110, 100, -1, -2, 0);

    sprintf(Ausgabe, "%3d    ", Messwert_USL);
    lcd.Print(Ausgabe, 110, 110, -1, -2, 0);

    sensor_mutex.unlock();
    lcd_mutex.unlock();
}


//Anzeige während Querfahrt 
void Display_querfuehrung()
{
    // In dem mitgebrachten Beispiel hätte die Funktion Ausgelagert werden können
    lcd_mutex.lock();

    lcd.Print("Rear IR  :      ", 5, 60, 1, -2, 0);
    lcd.Print("Front IR :      ", 5, 70, -1, -2, 0);
    lcd.Print("Left IR  :      ", 5, 80, -1, -2, 0);
    lcd.Print("Right IR :      ", 5, 90, -1, -2, 0);
    lcd.Print("US R     :      ", 5, 100, -1, -2, 0);
    lcd.Print("US L     :      ", 5, 110, -1, -2, 0);

    lcd_mutex.unlock();

    while (1)
    {
        Sensor_Anzeige();
        anzeige();
        ThisThread::sleep_for(100);
    }
}



//***weitere Methoden 

//Ansteuern der Aktoren
void aktorenTest()
{
    // Motor

    // Motor rückwärts drehen
    aktor_mutex.lock();
    MotorPWM = 0.001;
    aktor_mutex.unlock();

    ThisThread::sleep_for(2000);

    // Motor neutral
    aktor_mutex.lock();
    MotorPWM = 0.0015;
    aktor_mutex.unlock();

    ThisThread::sleep_for(2000);

    // Motor vorwärts drehen
    aktor_mutex.lock();
    MotorPWM = 0.002;
    aktor_mutex.unlock();

    ThisThread::sleep_for(2000);

    // Motor neutral
    aktor_mutex.lock();
    MotorPWM = 0.0015;
    aktor_mutex.unlock();

    // Lenkung
    // nach links lenken
    aktor_mutex.lock();
    LenkPWM = 0.001;
    aktor_mutex.unlock();

    ThisThread::sleep_for(2000);

    // Lenkung neutral
    aktor_mutex.lock();
    LenkPWM = 0.0015;
    aktor_mutex.unlock();

    ThisThread::sleep_for(2000);

    // nach rechts lenken
    aktor_mutex.lock();
    LenkPWM = 0.002;
    aktor_mutex.unlock();

    ThisThread::sleep_for(2000);

    // Lenkung neutral
    aktor_mutex.lock();
    LenkPWM = 0.0015;
    aktor_mutex.unlock();
}


//Ablauf der Querführung
void querfuehrung()
{
    while (1)
    {

        // hier message queue abfragen
        osEvent evt = mq_quer.get();

        if (evt.status == osEventMessage)
        {
            // Während des Ansteuerungsmechanismus, werden die globalen Sensorwerte
            // vom Mutex geschützt
            while (1)
            {
                sensor_mutex.lock();

                // sollte ein Sensor ein Objekt näher als 20cm erkennen, muss das
                // Fahrzeugvstehen bleiben
                if (Messwert_USR <= 20 || Messwert_USL <= 20)
                {
                    aktor_mutex.lock();
                    MotorPWM = 0.0015;
                    aktor_mutex.unlock();
                }
                // Schrittgeschwindigkeit
                else
                {
                    aktor_mutex.lock();
                    MotorPWM = 0.00167;
                    aktor_mutex.unlock();
                }
                // Misst der sensor Links zwischen 30 cm und 1 cm Werte so soll er nach rechts lenken
                // Mittels linearer regretion wird die formel gebildet
                if (Messwert_Left_IR < 30 && Messwert_Left_IR > 3)
                {
                    aktor_mutex.lock();
                    LenkPWM = (2000 - 16.5 * Messwert_Left_IR) / 1000000;
                    aktor_mutex.unlock();
                }

                // Misst der sensor rechts zwischen 30 cm und 1 cm Werte so soll er nach links lenken
                // Mittels linearer regretion wird die formel gebildet
                else if (Messwert_Right_IR < 30 && Messwert_Right_IR > 3)
                {
                    aktor_mutex.lock();
                    LenkPWM = (1000 + 16.5 * Messwert_Right_IR) / 1000000;
                    aktor_mutex.unlock();
                }

                // Werden rechts und links vom Infrarot-Sensor keine Werte gemessen so bleibt die Lenkung neutral
                else
                {
                    aktor_mutex.lock();
                    LenkPWM = 0.0015;
                    aktor_mutex.unlock();
                }

                sensor_mutex.unlock();
                ThisThread::sleep_for(10);

                // Wird eine Message Exit empfangen so wird die Funktion beendet
                osEvent evt1 = mq_exit.get(10); // messageque exit abfrage
                if (evt1.status == osEventMessage)
                {
                    break; // bei eintretenden ereignis break
                }
            }
        }
        else
        {
            ThisThread::sleep_for(1200);
        }
    }
}


//getter-Methode Ultraschall-Sensor rechts//
//Funktion wird zur Verwendung in while-Schleifen-Bedingung aufgerufen,um Mutex nutzen zu können//
uint16_t getMesswert_USR()
{
    uint16_t value;

    // Auslesen des rechten Ultraschall-Sensors; geschützt durch Mutex
    sensor_mutex.lock();
    value = Messwert_USR;
    sensor_mutex.unlock();

    return value;
}


//getter-Methode Ultraschall-Sensor links
// Funktion wird zur Verwendung in while-Schleifen-Bedingung aufgerufen,um Mutex nutzen zu können//
uint16_t getMesswert_USL()
{
    uint16_t value;

    // Auslesen des linken Ultraschall-Sensors; geschützt durch Mutex
    sensor_mutex.lock();
    value = Messwert_USR;
    sensor_mutex.unlock();

    return value;
}