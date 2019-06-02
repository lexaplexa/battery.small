/*************************************************************************
 * task_startup_core.cpp
 *
 * Created: 28.4.2019 10:20:59
 * Revised: 2.6.2019
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


void Core::Multitask::taskStartUpCore()
{    
    vout_off();
    vin_off();
    low_power_on();
    REG_PORT_DIRSET0 = VOUT_EN_msk|VIN_EN_msk|LOW_POWER_msk;
    
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
}