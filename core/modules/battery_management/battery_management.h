/*************************************************************************
 * battery_management.h
 *
 * Created: 1.5.2019 12:11:07
 * Revised: 
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <core/drivers/adc.h>
#include <conf/conf_board.h>

using namespace Core::Drivers;

#ifndef BATTERY_MANAGEMENT_H_
#define BATTERY_MANAGEMENT_H_

namespace Core
{
    namespace Modules
    {
        class BATTERY_MANAGEMENT
        {
            private:
                static uint16_t m_aVcell[8];
                static float m_fRefVoltage;
                
            public:
                static void Init(ADC_PRESCALER_enum ePrescale, ADC_RESOLUTION_enum eResolution, ADC_REFERENCE_enum eReference, float fRefVoltage);
                
                static void MeasCells();
                
                static void CheckCells();
                
                static uint16_t GetCellVoltage(uint8_t unCellNum);
            
        };
    }
}




#endif /* BATTERY_MANAGEMENT_H_ */