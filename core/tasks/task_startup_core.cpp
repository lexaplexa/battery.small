/*************************************************************************
 * task_startup_core.cpp
 *
 * Created: 28.4.2019 10:20:59
 * Revised: 5.5.2019
 * Author: uidm2956
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <core/core.h>


using namespace Core::Drivers;
using namespace Core::Extensions;
using namespace Core::Modules;
using namespace Core::Multitask;
GENCLOCK cClockMain(0, CPU_FREQ, 1, GCLK_SOURCE_OSC8M);
void taskCellTest();
uint16_t res;

void Core::Multitask::taskStartUpCore()
{    
    vout_off();
    vin_off();
    low_power_on();
    REG_PORT_DIRSET0 = VOUT_EN_msk|VIN_EN_msk|LOW_POWER_msk;
    
    Core::Multitask::MTASK::EnableDeepSleep();
    
    /* ADC channels initialization */
    MUX::SetPinGroup(&PORTA, VCELL1_msk|VCELL2_msk|VCELL3_msk|VCELL4_msk|VCELL5_msk|VCELL6_msk|VCELL7_msk, VCELL1_pinmux);
    BATTERY_MANAGEMENT::Init(ADC_PRESCALER_DIV64, ADC_RESOLUTION_12BIT, ADC_REFERENCE_INT1V, 1.01f);
    
    /* Buttons initialization */
    REG_PORT_OUTSET0 = BUTTON_LEFT_msk|BUTTON_RIGHT_msk;        /* Pull-up */
    MUX::SetPinGroup(&PORTA, BUTTON_LEFT_msk|BUTTON_RIGHT_msk, BUTTON_LEFT_pinmux, PORT_PINCFG_PMUXEN|PORT_PINCFG_PULLEN|PORT_PINCFG_INEN);    
    EXTINT::Enable(0);
    EXTINT::SetExtInt(BUTTON_LEFT_extint, EIC_SENSE_Fall);
    EXTINT::SetExtInt(BUTTON_RIGHT_extint, EIC_SENSE_Fall);
    NVIC_EnableIRQ(EIC_IRQn);
    
    /* Display initialization */
    MUX::SetPinGroup(&PORTA, DISP_SCL_msk|DISP_SDA_msk, DISP_SCL_pinmux);
    
    /* Run application */
    MTASK::Run(taskStartUpApp);
    
    /* Tests */
    MTASK::Repeat(taskCellTest, TASK_TOUT_MS(100));
    SSD1306::Init();
    SSD1306::On();    
    SSD1306::Println((uint8_t*)"!\"#$%&'()*+,-./0123456789:;", 0, 0);
    SSD1306::Println((uint8_t*)"<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ", 1, 0);
    SSD1306::Println((uint8_t*)"[\\]^_`abcdefghijklmnopqrstuvwxyz", 3, 0);
    SSD1306::WriteCmd(SSD1306_INVERTDISPLAY);
}

void taskCellTest()
{
    BATTERY_MANAGEMENT::MeasCells();
    //BATTERY_MANAGEMENT::CheckCells();
    res = BATTERY_MANAGEMENT::GetCellVoltage(1);
    res = BATTERY_MANAGEMENT::GetCellVoltage(2);
    res = BATTERY_MANAGEMENT::GetCellVoltage(3);
    res = BATTERY_MANAGEMENT::GetCellVoltage(4);
    res = BATTERY_MANAGEMENT::GetCellVoltage(5);
    res = BATTERY_MANAGEMENT::GetCellVoltage(6);
    res = BATTERY_MANAGEMENT::GetCellVoltage(7);
}


/************************************************************************/
/* INTERRUPT HANDLERS                                                   */
/************************************************************************/
void EIC_Handler()
{
    if (REG_EIC_INTFLAG & (1<<BUTTON_LEFT_extint))
    {
        if(BUTTONS::GetEvent(BUTTONS_EVENT_PushEnter) != 0) {MTASK::Run((FuncPtr_t)BUTTONS::GetEvent(BUTTONS_EVENT_PushEnter));}
    }
    if (REG_EIC_INTFLAG & (1<<BUTTON_RIGHT_extint))
    {
        if(BUTTONS::GetEvent(BUTTONS_EVENT_PushNext) != 0) {MTASK::Run((FuncPtr_t)BUTTONS::GetEvent(BUTTONS_EVENT_PushNext));}
    }
    
    EIC->INTFLAG.reg = 0xFFFFFFFF;      /* Clear flags */
}
