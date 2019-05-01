/*************************************************************************
 * adc.cpp
 *
 * Created: 28.04.2019 15:50:42
 * Revised: 
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 * Analog digital converter
 *************************************************************************/

#include "adc.h"

namespace Core::Drivers
{
    ADC_DRIVER::ADC_DRIVER(uint8_t unGeneratorNum)
    {
        GCLK->CLKCTRL.bit.ID = ADC_GCLK_ID;
        GCLK->CLKCTRL.bit.GEN = unGeneratorNum;
        GCLK->CLKCTRL.bit.CLKEN = true;
    }

    void ADC_DRIVER::Init(ADC_PRESCALER_enum ePrescale, ADC_RESOLUTION_enum eResolution, ADC_REFERENCE_enum eReference, float fRefVoltage)
    {
        m_fRefVoltage = fRefVoltage;

        ADC->CTRLA.bit.SWRST = true;
        while(m_pAdc->STATUS.bit.SYNCBUSY);
        
        ADC->CTRLB.bit.PRESCALER = ePrescale;
        ADC->CTRLB.bit.RESSEL = eResolution;
        ADC->REFCTRL.bit.REFSEL = eReference;        

        ADC->CTRLA.bit.ENABLE = true;
        while(m_pAdc->STATUS.bit.SYNCBUSY);
    }

    uint16_t ADC_DRIVER::Convert(ADC_MUXPOS_enum ePos)
    {
        ADC->INTFLAG.bit.RESRDY = 1;     /* Clear Flag before conversion */
        ADC->CTRLB.bit.DIFFMODE = false;
        ADC->INPUTCTRL.bit.MUXPOS = ePos;
        ADC->INPUTCTRL.bit.MUXNEG = ADC_MUXNEG_IOGND;
        ADC->SWTRIG.bit.START = true;
        while(!ADC->INTFLAG.bit.RESRDY);
        return ADC->RESULT.bit.RESULT;
    }

    uint16_t ADC_DRIVER::Convert(ADC_MUXPOS_enum ePos, ADC_MUXNEG_enum eNeg)
    {
        ADC->INTFLAG.bit.RESRDY = 1;     /* Clear Flag before conversion */
        ADC->CTRLB.bit.DIFFMODE = true;
        ADC->INPUTCTRL.bit.MUXPOS = ePos;
        ADC->INPUTCTRL.bit.MUXNEG = eNeg;
        ADC->SWTRIG.bit.START = true;
        while(!ADC->INTFLAG.bit.RESRDY);
        return ADC->RESULT.bit.RESULT;
    }

    uint16_t ADC_DRIVER::AverageConvert(ADC_MUXPOS_enum ePos, ADC_AVERAGE_SAMPLE_enum eSamples)
    {
        ADC->INTFLAG.bit.RESRDY = 1;     /* Clear Flag before conversion */
        ADC->CTRLB.bit.DIFFMODE = false;
        ADC->INPUTCTRL.bit.MUXPOS = ePos;
        ADC->INPUTCTRL.bit.MUXNEG = ADC_MUXNEG_IOGND;
        ADC->AVGCTRL.bit.SAMPLENUM = eSamples;
        if (eSamples == ADC_AVERAGE_SAMPLE_1) {ADC->AVGCTRL.bit.ADJRES = 0;}
        else if (eSamples == ADC_AVERAGE_SAMPLE_2) {ADC->AVGCTRL.bit.ADJRES = 1;}
        else if (eSamples == ADC_AVERAGE_SAMPLE_4) {ADC->AVGCTRL.bit.ADJRES = 2;}
        else if (eSamples == ADC_AVERAGE_SAMPLE_8) {ADC->AVGCTRL.bit.ADJRES = 3;}
        else{ADC->AVGCTRL.bit.ADJRES = 4;}
        ADC->SWTRIG.bit.START = true;
        while(!ADC->INTFLAG.bit.RESRDY);
        return ADC->RESULT.bit.RESULT;
    }

}
