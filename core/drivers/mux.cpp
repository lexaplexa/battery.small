/*****************************************************************************
 * mux.cpp
 *
 * Created: 19.7.2017 12:57:02
 * Revised: 4.4.2019
 * Author: uidm2956
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/

#include "mux.h"

namespace Core::Drivers
{
    void MUX::SetPin(uint8_t unPinNum, uint8_t unMuxFunc)
    {
         PortGroup *sPortGroup;
         
         if (unPinNum >= 32) {unPinNum %= 32; sPortGroup = &PORT->Group[1];}
         else {sPortGroup = &PORT->Group[0];}
         
         /* Lower 16 pins */
         if (unPinNum < 16) {sPortGroup->WRCONFIG.reg = PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PINMASK(1<<unPinNum);}
         /* Higher 16 pins */
         else {sPortGroup->WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PINMASK(1<<(unPinNum-16));}
    }

    void MUX::SetPin(uint8_t unPinNum, uint8_t unMuxFunc, uint8_t unPinCfg)
    {
        PortGroup *sPortGroup;
        
        if (unPinNum >= 32) {unPinNum %= 32; sPortGroup = &PORT->Group[1];}
        else {sPortGroup = &PORT->Group[0];}
        
        /* Lower 16 pins */
        if (unPinNum < 16) {sPortGroup->WRCONFIG.reg = PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|unPinCfg<<PORT_WRCONFIG_PMUXEN_Pos|PORT_WRCONFIG_PINMASK(1<<unPinNum);}
        /* Higher 16 pins */
        else {sPortGroup->WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|unPinCfg<<PORT_WRCONFIG_PMUXEN_Pos|PORT_WRCONFIG_PINMASK(1<<(unPinNum-16));}
    }

    void MUX::SetPinGroup(PortGroup *psPortGroup, uint32_t unPinMask, uint8_t unMuxFunc)
    {
        /* Lower 16 pins */
        psPortGroup->WRCONFIG.reg = PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PINMASK(unPinMask);
        /* Higher 16 pins */
        psPortGroup->WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|PORT_WRCONFIG_PMUXEN|PORT_WRCONFIG_PINMASK(unPinMask>>16);
    }

    void MUX::SetPinGroup(PortGroup *psPortGroup, uint32_t unPinMask, uint8_t unMuxFunc, uint8_t unPinCfg)
    {
         /* Lower 16 pins */
         psPortGroup->WRCONFIG.reg = PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|unPinCfg<<PORT_WRCONFIG_PMUXEN_Pos|PORT_WRCONFIG_PINMASK(unPinMask);
         /* Higher 16 pins */
         psPortGroup->WRCONFIG.reg = PORT_WRCONFIG_HWSEL|PORT_WRCONFIG_WRPMUX|PORT_WRCONFIG_WRPINCFG|PORT_WRCONFIG_PMUX(unMuxFunc)|unPinCfg<<PORT_WRCONFIG_PMUXEN_Pos|PORT_WRCONFIG_PINMASK(unPinMask>>16);
    }
}