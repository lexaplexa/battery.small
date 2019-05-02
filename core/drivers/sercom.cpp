/*****************************************************************************
 * sercom.cpp
 *
 * Created: 23.9.2015 17:18:53
 * Revised: 2.5.2019
 * Author: uidm2956
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/

#include "sercom.h"

namespace Core::Drivers
{
    /************************************************************************/
    /* SERCOM                                                               */
    /************************************************************************/
    SERCOM::SERCOM(::Sercom* pSercom, uint8_t unGeneratorNum)
    {
        m_pSercom = pSercom;
        
        if (m_pSercom == SERCOM0)
        {
            PM->APBCMASK.bit.SERCOM0_ = true;
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGeneratorNum)|SERCOM0_GCLK_ID_CORE;
        }
        else if (m_pSercom == SERCOM1)
        {
            PM->APBCMASK.bit.SERCOM1_ = true;
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGeneratorNum)|SERCOM1_GCLK_ID_CORE;            
        }
        else if (m_pSercom == SERCOM2)
        {
            PM->APBCMASK.bit.SERCOM2_ = true;
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGeneratorNum)|SERCOM2_GCLK_ID_CORE;
        }
        else if (m_pSercom == SERCOM3)
        {
            PM->APBCMASK.bit.SERCOM3_ = true;
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGeneratorNum)|SERCOM3_GCLK_ID_CORE;
        }
        #ifdef SERCOM4
        else if (m_pSercom == SERCOM4)
        {
            PM->APBCMASK.bit.SERCOM4_ = true;
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGeneratorNum)|SERCOM4_GCLK_ID_CORE;
        }
        #endif
        #ifdef SERCOM5
        else if (m_pSercom == SERCOM5)
        {
            PM->APBCMASK.bit.SERCOM5_ = true;
            GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN|GCLK_CLKCTRL_GEN(unGeneratorNum)|SERCOM5_GCLK_ID_CORE;
        }
        #endif
    }
    
    /************************************************************************/
    /* USART                                                                */
    /************************************************************************/
    void USART::Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud)
    {
        m_pSercom->USART.CTRLA.bit.SWRST = true;
        while (m_pSercom->USART.SYNCBUSY.bit.SWRST);
        
        m_pSercom->USART.CTRLA.bit.MODE = 1;
        m_pSercom->USART.CTRLA.bit.FORM = 0;
        m_pSercom->USART.CTRLA.bit.DORD = 1;
        m_pSercom->USART.CTRLA.bit.RXPO = unPadIn;
        m_pSercom->USART.CTRLA.bit.TXPO = unPadOut;
        m_pSercom->USART.BAUD.reg = 65536 - (65536*16*(unBaud/1000)/(unFgen/1000));
        m_pSercom->USART.CTRLB.bit.RXEN = true;
        m_pSercom->USART.CTRLB.bit.TXEN = true;
        m_pSercom->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC;
        
        m_pSercom->USART.CTRLA.bit.ENABLE = true;
        while (m_pSercom->USART.SYNCBUSY.bit.ENABLE);
    }

    void USART::Send(char* pString)
    {
        for (pString; *pString != '\0'; pString++)
        {
            m_pSercom->USART.DATA.reg = *pString;
            while (m_pSercom->USART.INTFLAG.bit.TXC);
            m_pSercom->USART.INTFLAG.bit.TXC = 1;       /* Clear flag */
        }
    }

    void USART::Send(uint8_t* pData, uint16_t unLength)
    {
        for (uint16_t i=0; i<unLength; i++)
        {
            m_pSercom->USART.DATA.reg = *pData++;
            while (m_pSercom->USART.INTFLAG.bit.TXC);
            m_pSercom->USART.INTFLAG.bit.TXC = 1;       /* Clear flag */
        }
    }
    
    /************************************************************************/
    /* SPI                                                                  */
    /************************************************************************/
    void SPI::Init(uint8_t unPadIn, uint8_t unPadOut)
    {
        m_pSercom->SPI.CTRLA.bit.SWRST = true;
        while (m_pSercom->SPI.SYNCBUSY.bit.SWRST);
        
        m_pSercom->SPI.CTRLA.bit.MODE = 2;
        m_pSercom->SPI.CTRLA.bit.DOPO = unPadOut;
        m_pSercom->SPI.CTRLA.bit.DIPO = unPadIn;
        m_pSercom->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_SSL|SERCOM_SPI_INTENSET_RXC|SERCOM_SPI_INTENSET_TXC;
        m_pSercom->SPI.CTRLB.bit.RXEN = true;
        m_pSercom->SPI.CTRLB.bit.SSDE = true;
        m_pSercom->SPI.INTFLAG.reg = 0xFF;
        m_pSercom->SPI.CTRLB.bit.PLOADEN = true;
        
        m_pSercom->SPI.CTRLA.bit.ENABLE = true;
        while (m_pSercom->SPI.SYNCBUSY.bit.ENABLE);
    }

    void SPI::Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud)
    {
        m_pSercom->SPI.CTRLA.bit.SWRST = true;
        while (m_pSercom->SPI.SYNCBUSY.bit.SWRST);
        
        m_pSercom->SPI.CTRLA.bit.MODE = 3;
        m_pSercom->SPI.CTRLA.bit.DOPO = unPadOut;
        m_pSercom->SPI.CTRLA.bit.DIPO = unPadIn;
        m_pSercom->SPI.CTRLB.bit.RXEN = true;
        m_pSercom->SPI.BAUD.reg = (unFgen/2/unBaud)-1;
        m_pSercom->SPI.INTFLAG.reg = 0xFF;
        
        m_pSercom->SPI.CTRLA.bit.ENABLE = true;
        while (m_pSercom->SPI.SYNCBUSY.bit.ENABLE);
    }

    void SPI::Read(uint8_t* pData, uint16_t unLength)
    {
        for (uint16_t i=0; i<unLength; i++) {*pData++ = Read();}
    }

    void SPI::Send(uint8_t* pData, uint16_t unLength)
    {
        for (uint16_t i=0; i<unLength; i++) {Send(*pData++);}
    }

    /************************************************************************/
    /* I2C                                                                  */
    /************************************************************************/
    void I2C::Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud)
    {
        m_pSercom->I2CM.CTRLA.bit.SWRST = true;
        while (m_pSercom->I2CM.SYNCBUSY.bit.SWRST);

        m_pSercom->I2CM.CTRLA.bit.MODE = 0x05;              /* I2C master operation */
        m_pSercom->I2CM.CTRLA.bit.INACTOUT = 0x03;
        m_pSercom->I2CM.BAUD.bit.BAUD = unFgen/(2*unBaud)-1;

        m_pSercom->I2CM.CTRLA.bit.ENABLE = true;
        while (m_pSercom->I2CM.SYNCBUSY.bit.ENABLE);
        m_pSercom->I2CM.STATUS.bit.BUSSTATE = I2C_BUSSTATE_Idle;
    }

    void I2C::Send(uint8_t unByte)
    {
        /* Send Address */
        m_pSercom->I2CM.ADDR.bit.ADDR = m_unAddress;
        while(!m_pSercom->I2CM.INTFLAG.bit.MB);
        m_pSercom->I2CM.INTFLAG.bit.MB = 1;                 /* Clear interrupt flag */
        if (m_pSercom->I2CM.STATUS.bit.RXNACK) {return;}    /* No acknowledge from slave */

        /* Send byte */
        m_pSercom->I2CM.DATA.reg = unByte;
        while(!m_pSercom->I2CM.INTFLAG.bit.MB);
        m_pSercom->I2CM.INTFLAG.bit.MB = 1;
        
        /* Stop communication */
        m_pSercom->I2CM.CTRLB.bit.CMD = I2C_CMD_Stop;
    }
    
    uint8_t I2C::Read()
    {
        /* Send Address */
        m_pSercom->I2CM.ADDR.bit.ADDR = m_unAddress|0x01;
        while(!m_pSercom->I2CM.INTFLAG.bit.SB);
        m_pSercom->I2CM.INTFLAG.bit.SB = 1;                 /* Clear interrupt flag */

        /* Stop communication */
        m_pSercom->I2CM.CTRLB.bit.CMD = I2C_CMD_Stop;
        return m_pSercom->I2CM.DATA.reg;
    }

    void I2C::Send(uint8_t* aData, uint16_t unLength)
    {
        /* Send Address */
        m_pSercom->I2CM.ADDR.bit.ADDR = m_unAddress;
        while(!m_pSercom->I2CM.INTFLAG.bit.MB);
        m_pSercom->I2CM.INTFLAG.bit.MB = 1;                     /* Clear interrupt flag */
        if (m_pSercom->I2CM.STATUS.bit.RXNACK) {return;}        /* No acknowledge from slave */

        /* Send Data */
        for (uint16_t i=0; i<unLength; i++)
        {
            m_pSercom->I2CM.DATA.reg = aData[i];
            while(!m_pSercom->I2CM.INTFLAG.bit.MB);
            m_pSercom->I2CM.INTFLAG.bit.MB = 1;                 /* Clear interrupt flag */
            if (m_pSercom->I2CM.STATUS.bit.RXNACK) {return;}    /* No acknowledge from slave */
        }

        /* Stop communication */
        m_pSercom->I2CM.CTRLB.bit.CMD = I2C_CMD_Stop;
    }
    
    void I2C::Read(uint8_t* aData, uint16_t unLength)
    {
        /* Send Address */
        m_pSercom->I2CM.ADDR.bit.ADDR = m_unAddress|0x01;

        /* Read data */
        for (uint16_t i=0; i<unLength; i++)
        {
            while(!m_pSercom->I2CM.INTFLAG.bit.SB);
            m_pSercom->I2CM.INTFLAG.bit.SB = 1;                 /* Clear interrupt flag */
            if (m_pSercom->I2CM.STATUS.bit.RXNACK) {return;}    /* No acknowledge from slave */
            else {aData[i] = m_pSercom->I2CM.DATA.reg;}
            m_pSercom->I2CM.CTRLB.bit.CMD = I2C_CMD_Acknowledge;
        }

        /* Stop communication */
        m_pSercom->I2CM.CTRLB.bit.CMD = I2C_CMD_Stop;
    }
}
