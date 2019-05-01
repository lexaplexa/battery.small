/*************************************************************************
 * task_startup_core.cpp
 *
 * Created: 28.4.2019 10:20:59
 * Revised: 
 * Author: uidm2956
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <core/core.h>
#include <core/drivers/adc.h>

#define VOUT_EN_pin             PORT_PA14
#define VIN_EN_pin              PORT_PA11
#define LOW_POWER_pin           PORT_PA28
#define VCELL1_pin              PORT_PA04
#define VCELL2_pin              PORT_PA05

#define vout_on()               REG_PORT_OUTCLR0 = VOUT_EN_pin;
#define vout_off()              REG_PORT_OUTSET0 = VOUT_EN_pin;
#define vin_on()                REG_PORT_OUTCLR0 = VIN_EN_pin;
#define vin_off()               REG_PORT_OUTSET0 = VIN_EN_pin;
#define low_power_on()          REG_PORT_OUTCLR0 = LOW_POWER_pin;
#define low_power_off()         REG_PORT_OUTSET0 = LOW_POWER_pin;

#define OVERVOLTAGE_CUTOFF      4.2f
#define UNDERVOLTAGE_CUTOFF     3.0f

using namespace Core::Drivers;
ADC_DRIVER cAdc(0);
uint16_t res[7];
float resf[7];

void test()
{
    
    res[0] = cAdc.Convert(ADC_MUXPOS_AIN4);
    res[1] = cAdc.Convert(ADC_MUXPOS_AIN5);
    res[2] = cAdc.Convert(ADC_MUXPOS_AIN6);
    res[3] = cAdc.Convert(ADC_MUXPOS_AIN7);
    res[4] = cAdc.Convert(ADC_MUXPOS_AIN16);
    res[5] = cAdc.Convert(ADC_MUXPOS_AIN17);
    res[6] = cAdc.Convert(ADC_MUXPOS_AIN18);
    
    for (uint8_t i=0; i<7; i++)
    {
        resf[i] = float(res[i])*0.992/4096*31;
    }
    
    resf[6] -= resf[5];
    resf[5] -= resf[4];
    resf[4] -= resf[3];
    resf[3] -= resf[2];
    resf[2] -= resf[1];
    resf[1] -= resf[0];
    
    /* Over voltage detection */
    for (uint8_t i=0; i<7; i++)
    {
        if (resf[i] > OVERVOLTAGE_CUTOFF) 
        {
            vin_off();
            vout_on();
            break;
        }
    }
    
    /* Enable charging after over voltage cutoff */
    bool bVinOn = true;
    for (uint8_t i=0; i<7; i++)
    {
        if (resf[i] > 4.0f)
        {
            bVinOn = false;
            break;
        }
    }
    if (bVinOn) {vin_on();}
    
    /* Under voltage detection */
    for (uint8_t i=0; i<7; i++)
    {
        if (resf[i] < UNDERVOLTAGE_CUTOFF)
        {
            vout_off();
            low_power_on();
            break;
        }
    }
    
    /* Enable discharging after under voltage */
    bool bVoutOn = true;
    for (uint8_t i=0; i<7; i++)
    {
        if (resf[i] < 3.2f)
        {
            bVoutOn = false;
            break;
        }
    }
    if (bVoutOn) {vout_on(); low_power_off();}
}

void Core::Multitask::taskStartUpCore()
{    
    vout_off();
    vin_off();
    low_power_on();
    REG_PORT_DIR0 = VOUT_EN_pin|VIN_EN_pin|LOW_POWER_pin;
    
    Core::Multitask::MTASK::EnableDeepSleep();
    Core::Multitask::MTASK::Repeat(test,TASK_TOUT_MS(500));
    
    PORT->Group[0].PMUX[2].reg = MUX_PA04B_ADC_AIN4|MUX_PA05B_ADC_AIN5<<4;
    PORT->Group[0].PMUX[3].reg = MUX_PA06B_ADC_AIN6|MUX_PA07B_ADC_AIN7<<4;
    PORT->Group[0].PMUX[4].reg = MUX_PA08B_ADC_AIN16|MUX_PA09B_ADC_AIN17<<4;
    PORT->Group[0].PMUX[5].reg = MUX_PA10B_ADC_AIN18;
    
    cAdc.Init(ADC_PRESCALER_DIV64, ADC_RESOLUTION_12BIT, ADC_REFERENCE_INT1V, 1.01f);
}