/*****************************************************************************
 * mux.h
 *
 * Created: 26.1.2017 11:14:32
 * Revised: 4.4.2019
 * Author: uidm2956
 * BOARD:
 * ABOUT:
 *
 *****************************************************************************/


#ifndef MUX_H_
#define MUX_H_

#include "sam.h"

namespace Core
{
    namespace Drivers
    {
        /************************************************************************/
        /* PORT DEFINITIONS                                                     */
        /************************************************************************/
        #define PORTA       PORT->Group[0]
        #define PORTB       PORT->Group[1]
        
        
        /************************************************************************/
        /* MULTIPLEXER FUNCTIONS                                                */
        /************************************************************************/
        class MUX
        {
            private:

            public:
                /**
                 * \brief   Set multiplexer function for one pin
                 * 
                 * \param unPinNum          - pin number (0-64) 
                 * \param unMuxFunc         - multiplexer pin function
                 * 
                 * \return void
                 */
                static void SetPin(uint8_t unPinNum, uint8_t unMuxFunc);
        
        
                /**
                 * \brief   Set multiplexer function for one pin with pin configuration
                 * 
                 * \param unPinNum          - pin number (0-64)  
                 * \param unMuxFunc         - multiplexer pin function
                 * \param unPinCfg          - pin configuration
                 * 
                 * \return void
                 */
                static void SetPin(uint8_t unPinNum, uint8_t unMuxFunc, uint8_t unPinCfg);
        
        
                /**
                 * \brief   Set multiplexer function for group of pins
                 * 
                 * \param psPortGroup       - port group
                 * \param unPinMask         - mask of pins
                 * \param unMuxFunc         - multiplexer pin function
                 * 
                 * \return void
                 */
                static void SetPinGroup(PortGroup *psPortGroup, uint32_t unPinMask, uint8_t unMuxFunc);
        
        
                /**
                 * \brief   Set multiplexer function for group of pins with pin configuration
                 * 
                 * \param psPortGroup       - port group
                 * \param unPinMask         - mask of pins
                 * \param unMuxFunc         - multiplexer pin function
                 * \param unPinCfg          - pins configuration
                 * 
                 * \return void
                 */
                static void SetPinGroup(PortGroup *psPortGroup, uint32_t unPinMask, uint8_t unMuxFunc, uint8_t unPinCfg);   
        };
    }
}

#endif /* MUX_H_ */