/*************************************************************************
 * task_startup_app.cpp
 *
 * Created: 28.4.2019 10:24:54
 * Revised: 2.6.2019
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <app/app.h>

using namespace Core::Extensions;
using namespace Core::Multitask;
using namespace Core::Modules;

uint16_t res;
uint8_t aData[20];

void taskCellMeas();
void taskSleep();
void taskWakeUp();

void taskStartUpApp()
{
    SSD1306::Init();
    SSD1306::On();
    MTASK::EnableDeepSleep();
    BUTTONS::SetEvent(BUTTONS_EVENT_PushNext, (void*)taskWakeUp);
    SSD1306::Println((uint8_t*)"Hello", 2,40,SSD1306_FONT_Large);
    MTASK::Delay(taskSleep, 10000);
    MTASK::Repeat(taskCellMeas, 1000);
}

void taskCellMeas()
{
    BATTERY_MANAGEMENT::MeasCells();
    res = BATTERY_MANAGEMENT::GetCellVoltage(1);
    sprintf((char*)aData, "Cell1 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,0,10,SSD1306_FONT_Small);
    res = BATTERY_MANAGEMENT::GetCellVoltage(2);
    sprintf((char*)aData, "Cell2 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,1,10,SSD1306_FONT_Small);
    res = BATTERY_MANAGEMENT::GetCellVoltage(3);
    sprintf((char*)aData, "Cell3 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,2,10,SSD1306_FONT_Small);
    res = BATTERY_MANAGEMENT::GetCellVoltage(4);
    sprintf((char*)aData, "Cell4 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,3,10,SSD1306_FONT_Small);
    res = BATTERY_MANAGEMENT::GetCellVoltage(5);
    sprintf((char*)aData, "Cell5 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,4,10,SSD1306_FONT_Small);
    res = BATTERY_MANAGEMENT::GetCellVoltage(6);
    sprintf((char*)aData, "Cell6 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,5,10,SSD1306_FONT_Small);
    res = BATTERY_MANAGEMENT::GetCellVoltage(7);
    sprintf((char*)aData, "Cell7 - %*d.%02dV   ", 3, res/100, res%100);
    SSD1306::Println(aData,6,10,SSD1306_FONT_Small);
}

void taskSleep()
{
    SSD1306::Off();
    MTASK::Suspend(taskCellMeas);
}

void taskWakeUp()
{
    SSD1306::On();
    MTASK::Delay(taskSleep, 10000);
    MTASK::Resume(taskCellMeas);
}