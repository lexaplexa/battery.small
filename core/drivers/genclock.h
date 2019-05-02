/*****************************************************************************
 * genclock.h
 *
 * Created: 2.5.2019 11:21:04
 * Revised: 
 * Author: uidm2956
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/


#ifndef GENCLOCK_H_
#define GENCLOCK_H_

#include "sam.h"


/************************************************************************/
/* CLASS                                                                */
/************************************************************************/
namespace Core
{
	namespace Drivers
    {
        class GENCLOCK
        {
            private:
                uint32_t m_unFrequency;
                uint8_t m_unGenNum;
                    
            public:            
                /**
                 * \brief   Set generator clock using DPLL
                 * 
                 * \param unGenNum          - generator number (0-8)
                 * \param unGenFreq         - generator frequency
                 * \param unDiv             - divide generator frequency
                 * \param unRefClk          - DPLL reference clock (0 - XOSC32K, 1 - XOSC, 2 - GCLK)
                 * \param unMultiplication  - multiplication factor
                 * 
                 * \return 
                 */
                GENCLOCK(uint8_t unGenNum, uint32_t unGenFreq, uint8_t unDiv, uint8_t unRefClk, uint16_t unMultiplication)
                {
                    if (unGenNum > 8 || unRefClk > 2) {return;}
                    
                    m_unFrequency = unGenFreq;	
                    m_unGenNum = unGenNum;
                        
                    REG_NVMCTRL_CTRLB |= NVMCTRL_CTRLB_RWS(2);		/* Read wait state must be >1 for frequencies >24MHz */
                        
                    /* Enable RTC oscillator */
                    if (unRefClk == 0)
                    {
                        REG_SYSCTRL_XOSC32K = SYSCTRL_XOSC32K_XTALEN|SYSCTRL_XOSC32K_EN32K|SYSCTRL_XOSC32K_ENABLE;
                        while(!SYSCTRL->PCLKSR.bit.XOSC32KRDY);
                    }
                    /* Enable external oscillator */
                    else if (unRefClk == 1)
                    {
                        if      (unGenFreq <= 2000000)  {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(0)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else if (unGenFreq <= 4000000)  {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(1)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else if (unGenFreq <= 8000000)  {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(2)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else if (unGenFreq <= 16000000) {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(3)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else                            {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(4)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        while(!SYSCTRL->PCLKSR.bit.XOSCRDY);
                        unMultiplication *= 2;      /* Multiplication must be 2x higher because XOSC is divided by 2 (DPLLCTRLB.DIV = 0) */
                    }
                        
                    /* Enable DPLL */
                    REG_SYSCTRL_DPLLCTRLB = SYSCTRL_DPLLCTRLB_REFCLK(unRefClk);
                    REG_SYSCTRL_DPLLRATIO = SYSCTRL_DPLLRATIO_LDR(unMultiplication-1);
                    REG_SYSCTRL_DPLLCTRLA = SYSCTRL_DPLLCTRLA_ENABLE;
                    while(!SYSCTRL->DPLLSTATUS.bit.ENABLE);
                        
                    /* Set Genclock */
                    REG_GCLK_CTRL = GCLK_GENCTRL_ID(unGenNum)|GCLK_GENCTRL_SRC_DPLL96M|GCLK_GENCTRL_GENEN;
                    while(REG_GCLK_STATUS & GCLK_STATUS_SYNCBUSY);
                    REG_GCLK_GENDIV = GCLK_GENDIV_ID(unGenNum)|GCLK_GENDIV_DIV(unDiv);
                    while(REG_GCLK_STATUS & GCLK_STATUS_SYNCBUSY);
                }
                    
                    
                /**
                 * \brief   Set generator clock
                 * 
                 * \param unGenNum          - generator number (0-8)
                 * \param unGenFreq         - generator frequency
                 * \param unDiv             - divide generator frequency
                 * \param unSourceClk       - generator source clock (0->XOSC, 1->GCLKIN, 2->GCLKGEN1, 3->OSCULP32K, 4->OSC32K, 5->XOSC32K, 6->OSC48M, 7->DPPL96M)
                 * 
                 * \return 
                 */
                GENCLOCK(uint8_t unGenNum, uint32_t unGenFreq, uint8_t unDiv, uint8_t unSourceClk)
                {
                    if (unGenNum > 8 || unSourceClk > 8) {return;}
                        
                    m_unFrequency = unGenFreq;
                    m_unGenNum = unGenNum;
                        
                    REG_NVMCTRL_CTRLB |= NVMCTRL_CTRLB_RWS(2);		/* Read wait state must be >1 for frequencies >24MHz */
                        
                    if (unSourceClk == GCLK_SOURCE_XOSC)
                    {
                        if      (unGenFreq <= 2000000)  {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(0)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else if (unGenFreq <= 4000000)  {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(1)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else if (unGenFreq <= 8000000)  {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(2)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else if (unGenFreq <= 16000000) {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(3)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        else                            {REG_SYSCTRL_XOSC = SYSCTRL_XOSC_GAIN(4)|SYSCTRL_XOSC_XTALEN|SYSCTRL_XOSC_ENABLE;}
                        while(!SYSCTRL->PCLKSR.bit.XOSCRDY);
                    }
                    else if (unSourceClk == GCLK_SOURCE_XOSC32K)
                    {
                        REG_SYSCTRL_XOSC32K = SYSCTRL_XOSC32K_XTALEN|SYSCTRL_XOSC32K_EN32K|SYSCTRL_XOSC32K_ENABLE;
                        while(!SYSCTRL->PCLKSR.bit.XOSC32KRDY);
                    }
                    else if (unSourceClk == GCLK_SOURCE_OSC8M)
                    {
                        SYSCTRL->OSC8M.bit.PRESC = 0;
                    }
                        
                    /* Set Genclock */
                    REG_GCLK_CTRL = GCLK_GENCTRL_ID(unGenNum)|GCLK_GENCTRL_SRC(unSourceClk)|GCLK_GENCTRL_GENEN;
                    while(REG_GCLK_STATUS & GCLK_STATUS_SYNCBUSY);
                    REG_GCLK_GENDIV = GCLK_GENDIV_ID(unGenNum)|GCLK_GENDIV_DIV(unDiv);
                    while(REG_GCLK_STATUS & GCLK_STATUS_SYNCBUSY);
                }
                    
                    
                /**
                 * \brief   Get generator number
                 * 
                 * 
                 * \return uint8_t
                 */
                uint8_t GenNum()
                {
                    return m_unGenNum;
                }
                    
                    
                /**
                 * \brief   Get actual frequency
                 * 
                 * 
                 * \return uint32_t
                 */
                uint32_t GenFreq()
                {
                    GCLK->GENDIV.bit.ID = m_unGenNum;
                    return (m_unFrequency/GCLK->GENDIV.bit.DIV);
                }
        };
    }
}
#endif /* GENCLOCK_H_ */
