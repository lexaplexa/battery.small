/*************************************************************************
 * battery_management.cpp
 *
 * Created: 1.5.2019 13:31:31
 * Revised:
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include "battery_management.h"

using namespace Core::Drivers;
ADC_DRIVER cAdc(0);
uint16_t Core::Modules::BATTERY_MANAGEMENT::m_aVcell[];
float Core::Modules::BATTERY_MANAGEMENT::m_fRefVoltage;

void Core::Modules::BATTERY_MANAGEMENT::Init(ADC_PRESCALER_enum ePrescale, ADC_RESOLUTION_enum eResolution, ADC_REFERENCE_enum eReference, float fRefVoltage)
{
    cAdc.Init(ePrescale, eResolution, eReference, fRefVoltage);
    m_fRefVoltage = fRefVoltage;
}

void Core::Modules::BATTERY_MANAGEMENT::MeasCells()
{
    m_aVcell[1] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL1_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
    m_aVcell[2] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL2_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
    m_aVcell[3] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL3_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
    m_aVcell[4] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL4_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
    m_aVcell[5] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL5_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
    m_aVcell[6] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL6_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
    m_aVcell[7] = cAdc.AverageConvert((ADC_MUXPOS_enum)VCELL7_ADC_MUXPOS, ADC_AVERAGE_SAMPLE_8);
}

void Core::Modules::BATTERY_MANAGEMENT::CheckCells()
{
    /* Disable charge */
    for (uint8_t i=1; i<=7; i++)
    {
        if (GetCellVoltage(i) > 420) {vin_off(); break;}
    }
    
    /* Disable discharge */
    for (uint8_t i=1; i<=7; i++)
    {
        if (GetCellVoltage(i) < 300) {vout_off(); break;}
    }
    
    /* Enable charge */
    bool bEnableVin = true;
    for (uint8_t i=1; i<=7; i++)
    {
        if (GetCellVoltage(i) > 410) {bEnableVin = false; break;}
    }
    if (bEnableVin) {vin_on();}
    
    /* Enable discharge */
    bool bEnableVout = true;
    for (uint8_t i=1; i<=7; i++)
    {
        if (GetCellVoltage(i) < 320) {bEnableVout = false; break;}
    }
    if (bEnableVout) {vout_on();}
}

uint16_t Core::Modules::BATTERY_MANAGEMENT::GetCellVoltage(uint8_t unCellNum)
{
    if (unCellNum == 0 || unCellNum > 7) {return 0;}
    uint32_t unVcell = m_aVcell[unCellNum] - m_aVcell[unCellNum-1];
    return (unVcell*3100/4096);
}
