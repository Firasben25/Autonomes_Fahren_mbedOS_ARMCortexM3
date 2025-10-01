/******************************************************************************/
/*                                                                            */
/*              Ostfalia Hochschule für angewandte Wissenschaften             */
/*                        Fakultät für Fahrzeugtechnik                        */
/*           Institut für Fahrzeuginformatik und Fahrzeugelektronik           */
/*                             Fahrzeuginformatik                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* Paket:        ES_LAB                                                       */
/* Modul:        ES_LAB_3                                                     */
/* Projekt:      ES_LAB_3                                                     */
/* Beschreibung: Labor 3   "Grundsoftware für ein autonomes Modellfahrzeug"   */
/*----------------------------------------------------------------------------*/
/* Datei:        'Task.h'                                                     */
/*                                                                            */
/* Autoren:      Jan-Hendrik Aschen, M.Eng.                                   */
/*               Prof. Dr.-Ing. Volker von Holt                               */
/* Datum:        02.12.23                                                     */
/* Version:      0.3 / WS23                                                   */
/******************************************************************************/
/* Gruppe         :                                                           */
/* Name           : Ben Fraj, Firas                                           */
/* Matrikelnummer : 70475481        		                              */
/* Name           : Podorvanov, Dimitrij                                      */
/* Matrikelnummer : 70473730		                                      */
/******************************************************************************/


//------------------------------------------------------------------------------

#ifndef TASKS_H
#define TASKS_H

#include "mbed.h"

void Task_Sensor(void);
void Task_Aktor(void);
void Task_Display(void);
void Task_Menu(void);

void Sensor_Anzeige(void);
 
void initializeAll();

void mainMenu();
void anzeige();
void Display_querfuehrung();
void Display_Test();

void aktorenTest();
void querfuehrung();

uint16_t getMesswert_USR();
uint16_t getMesswert_USL();

#endif