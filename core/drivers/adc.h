/*************************************************************************
 * adc.h
 *
 * Created: 28.4.2019 15:51:28
 * Revised: 
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#ifndef ADC_H_
#define ADC_H_

#include <sam.h>

namespace Core
{
    namespace Drivers
    {
        enum ADC_REFERENCE_enum
        {
            ADC_REFERENCE_INT1V,
            ADC_REFERENCE_INTVCC0,
            ADC_REFERENCE_INTVCC1,
            ADC_REFERENCE_VREFA,
            ADC_REFERENCE_VREFB,
        };

        enum ADC_PRESCALER_enum
        {
            ADC_PRESCALER_DIV4,
            ADC_PRESCALER_DIV8,
            ADC_PRESCALER_DIV16,
            ADC_PRESCALER_DIV32,
            ADC_PRESCALER_DIV64,
            ADC_PRESCALER_DIV128,
            ADC_PRESCALER_DIV256,
            ADC_PRESCALER_DIV512,
        };

        enum ADC_RESOLUTION_enum
        {
            ADC_RESOLUTION_12BIT,
            ADC_RESOLUTION_16BIT,
            ADC_RESOLUTION_10BIT,
            ADC_RESOLUTION_8BIT,
        };

        enum ADC_MUXPOS_enum
        {
            ADC_MUXPOS_AIN0,
            ADC_MUXPOS_AIN1,
            ADC_MUXPOS_AIN2,
            ADC_MUXPOS_AIN3,
            ADC_MUXPOS_AIN4,
            ADC_MUXPOS_AIN5,
            ADC_MUXPOS_AIN6,
            ADC_MUXPOS_AIN7,
            ADC_MUXPOS_AIN8,
            ADC_MUXPOS_AIN9,
            ADC_MUXPOS_AIN10,
            ADC_MUXPOS_AIN11,
            ADC_MUXPOS_AIN12,
            ADC_MUXPOS_AIN13,
            ADC_MUXPOS_AIN14,
            ADC_MUXPOS_AIN15,
            ADC_MUXPOS_AIN16,
            ADC_MUXPOS_AIN17,
            ADC_MUXPOS_AIN18,
            ADC_MUXPOS_AIN19,
            ADC_MUXPOS_BANDGAP = 0x19,
            ADC_MUXPOS_SCALEDCOREVCC,
            ADC_MUXPOS_SCALEDIOVCC,
            ADC_MUXPOS_DAC,
        };

        enum ADC_MUXNEG_enum
        {
            ADC_MUXNEG_AIN0,
            ADC_MUXNEG_AIN1,
            ADC_MUXNEG_AIN2,
            ADC_MUXNEG_AIN3,
            ADC_MUXNEG_AIN4,
            ADC_MUXNEG_AIN5,
            ADC_MUXNEG_AIN6,
            ADC_MUXNEG_AIN7,
            ADC_MUXNEG_GND = 0x18,  /* Internal ground */
            ADC_MUXNEG_IOGND,       /* I/O ground */
        };

        class ADC_DRIVER
        {
            protected:
                ::Adc* m_pAdc;
                float m_fRefVoltage;
                    
            public:
                /**
                    * \brief   Analog to digital converter constructor
                    * 
                    * \param unGeneratorNum
                    * 
                    * \return 
                    */
                ADC_DRIVER(uint8_t unGeneratorNum);
                    
                /**
                    * \brief   Initialize ADC
                    * 
                    * \param ePrescale     - prescaler
                    * \param eResolution   - ADC resolution
                    * \param eReference    - reference type
                    * \param fRefVoltage   - reference voltage
                    * 
                    * \return void
                    */
                virtual void Init(ADC_PRESCALER_enum ePrescale, ADC_RESOLUTION_enum eResolution, ADC_REFERENCE_enum eReference, float fRefVoltage);
                    
                /**
                    * \brief   Convert voltage in positive pin
                    * 
                    * \param ePos          - Positive position
                    * 
                    * \return uint16_t     - ADC result
                    */
                virtual uint16_t Convert(ADC_MUXPOS_enum ePos);
                
                /**
                 * \brief   Convert differential voltage between positive and negative pin
                 * 
                 * \param ePos              - positive position
                 * \param eNeg              - negative position
                 * 
                 * \return uint16_t         - ADC result
                 */
                virtual uint16_t Convert(ADC_MUXPOS_enum ePos, ADC_MUXNEG_enum eNeg);
        };
    }
}
#endif /* ADC_H_ */