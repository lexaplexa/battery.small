/*****************************************************************************
 * eic.cpp
 *
 * Created: 19.7.2017 12:48:38
 * Revised: 1.5.2019
 * Author: uidm2956
 * BOARD: 
 * ABOUT: External Interrupt Controller
 *
 *****************************************************************************/

#include "eic.h"

namespace Core::Drivers
{
    
    void EXTINT::Enable()
    {
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(0)|EIC_GCLK_ID;
        EIC->CTRL.bit.ENABLE = true;
        while(EIC->STATUS.bit.SYNCBUSY);
    }

    void EXTINT::Enable(uint8_t unGenNum)
    {
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGenNum)|EIC_GCLK_ID;
        EIC->CTRL.bit.ENABLE = true;
        while(EIC->STATUS.bit.SYNCBUSY);
    }

    void EXTINT::Disable()
    {
        EIC->CTRL.bit.ENABLE = false;
        while(EIC->STATUS.bit.SYNCBUSY);
    }

    void EXTINT::SetExtInt(uint8_t unIntNum, EIC_SENSE_enum eSense)
    {
        EIC->CTRL.bit.ENABLE = false;
        while(EIC->STATUS.bit.SYNCBUSY);
        EIC->INTENSET.reg = 1<<unIntNum;
        EIC->CONFIG[unIntNum/8].reg &= ~(EIC_CONFIG_SENSE0_Msk<<(unIntNum%8*4));
        EIC->CONFIG[unIntNum/8].reg |= eSense<<(unIntNum%8*4);
        EIC->CTRL.bit.ENABLE = true;
        while(EIC->STATUS.bit.SYNCBUSY);
    }

    void EXTINT::ClearExtInt(uint8_t unIntNum)
    {
        EIC->CTRL.bit.ENABLE = false;
        while(EIC->STATUS.bit.SYNCBUSY);
        EIC->INTENCLR.reg = 1<<unIntNum;
        EIC->CONFIG[unIntNum/8].reg &= ~(EIC_CONFIG_SENSE0_Msk<<(unIntNum%8*4));
        EIC->CTRL.bit.ENABLE = true;
        while(EIC->STATUS.bit.SYNCBUSY);
    }
}