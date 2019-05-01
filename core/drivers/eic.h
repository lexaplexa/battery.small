/*****************************************************************************
 * eic.h
 *
 * Created: 9.12.2015 16:16:14
 * Revised: 4.4.2019
 * Author: uidm2956
 * BOARD: 
 * ABOUT: External Interrupt Controller
 * 
 *****************************************************************************/

#ifndef EIC_H_
#define EIC_H_

#include <sam.h>

namespace Core
{
	namespace Drivers
    {
        /************************************************************************/
        /* ENUMS                                                                */
        /************************************************************************/
        enum EIC_SENSE_enum {
            EIC_SENSE_None,
            EIC_SENSE_Rise,
            EIC_SENSE_Fall,
            EIC_SENSE_Both,
            EIC_SENSE_High,
            EIC_SENSE_Low
        };
            
        /************************************************************************/
        /* CLASS                                                                */
        /************************************************************************/
        class EXTINT
        {
            private:
                
            public:
                static void Enable();

                /**
                    * \brief   Enable external interrupt controller
                    * 
                    * \param unGenNum  - clock generator number
                    * 
                    * \return void
                    */
                static void Enable(uint8_t unGenNum);
            
                /**
                    * \brief   Disable external interrupt controller
                    * 
                    * 
                    * \return void
                    */
                static void Disable();
            
                /**
                    * \brief   Enable external interrupt
                    * 
                    * \param unIntNum      - interrupt number
                    * \param eSense        - input sense
                    * 
                    * \return void
                    */
                static void SetExtInt(uint8_t unIntNum, EIC_SENSE_enum eSense);
            
                /**
                    * \brief   Disable external interrupt
                    * 
                    * \param unIntNum  - interrupt number
                    * 
                    * \return void
                    */
                static void ClearExtInt(uint8_t unIntNum);
        };
    }
}

#endif /* EIC_H_ */