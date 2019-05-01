/*****************************************************************************
 * sercom.h
 *
 * Created: 23.9.2015 17:16:21
 * Revised: 30.4.2019
 * Author: uidm2956
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/


#ifndef SERCOM_H_
#define SERCOM_H_

#include <sam.h>

namespace Core
{
    namespace Drivers
    {
        /************************************************************************/
        /* SERCOM                                                               */
        /************************************************************************/
        class SERCOM
        {
            protected:
                ::Sercom* m_pSercom;
                
                /**
                 * \brief   General SERCOM constructor
                 * 
                 * \param pSercom           - pointer to SERCOM
                 * \param unGeneratorNum    - generator clock number
                 * 
                 * \return 
                 */
                SERCOM(::Sercom* pSercom, uint8_t unGeneratorNum);
                
                /**
                 * \brief   Virtual method for Sercom initialization
                 * 
                 * \param unPadIn           - PAD input value
                 * \param unPadOut          - PAD output value
                 * \param unFgen            - clock generator frequency
                 * \param unBaud            - baud rate
                 * 
                 * \return void
                 */
                virtual void Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud) = 0;
                
                /**
                 * \brief   Virtual method for sending byte
                 * 
                 * \param unByte            - data byte
                 * 
                 * \return void
                 */
                virtual void Send(uint8_t unByte) = 0;
                
                /**
                 * \brief   Virtual method to read data from DATA register
                 * 
                 * 
                 * \return uint8_t          - data byte
                 */
                virtual uint8_t Read() = 0;
        };            
        
        /************************************************************************/
        /* USART                                                                */
        /************************************************************************/
        class USART: public SERCOM
        {
            private:
            
            public:
                /**
                 * \brief   SERCOM UART constructor
                 * 
                 * \param pSercom           - pointer to SERCOM
                 * \param unGeneratorNum    - generator clock number
                 * 
                 * \return 
                 */
                USART(::Sercom* pSercom, uint8_t unGeneratorNum): SERCOM(pSercom, unGeneratorNum) {};                    
                
                /**
                 * \brief   Initialize USART
                 * 
                 * \param unPadIn           - PAD input value
                 * \param unPadOut          - PAD output value
                 * \param unFgen            - clock generator frequency
                 * \param unBaud            - baud rate
                 * 
                 * \return void
                 */
                void Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud);
                
                /**
                 * \brief   Send byte
                 * 
                 * \param unByte        - data byte
                 * 
                 * \return void
                 */
                void Send(uint8_t unByte) {m_pSercom->USART.DATA.reg = unByte;}
                
                /**
                 * \brief   Send string
                 * 
                 * \param pString       - pointer to string
                 * 
                 * \return void
                 */
                void Send(char* pString);
                
                /**
                 * \brief   Send data
                 * 
                 * \param pData         - pointer to data
                 * \param unLength      - number of data
                 * 
                 * \return void
                 */
                void Send(uint8_t* pData, uint16_t unLength);
                
                /**
                 * \brief   Read byte from DATA register
                 * 
                 * 
                 * \return uint8_t      - byte from register DATA
                 */
                uint8_t Read() {return m_pSercom->USART.DATA.reg;}
        };
        
        /************************************************************************/
        /* SPI                                                                  */
        /************************************************************************/
        class SPI: public SERCOM
        {
            private:
                
            public:
                /**
                 * \brief   SERCOM SPI constructor
                 * 
                 * \param pSercom           - pointer to SERCOM 
                 * \param unGeneratorNum    - generator clock number
                 * 
                 * \return 
                 */
                SPI(::Sercom *pSercom, uint8_t unGeneratorNum): SERCOM(pSercom, unGeneratorNum) {};
        
                /**
                 * \brief   Initialize SPI slave
                 * 
                 * \param unPadIn           - PAD input value
                 * \param unPadOut          - PAD output value
                 * 
                 * \return void
                 */
                void Init(uint8_t unPadIn, uint8_t unPadOut);
        
                /**
                 * \brief   Initialize SPI master
                 * 
                 * \param unPadIn           - PAD input value
                 * \param unPadOut          - PAD output value
                 * \param unFgen            - clock generator frequency
                 * \param unBaud            - baud rate
                 * 
                 * \return void
                 */
                void Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud);
        
                /**
                 * \brief   Read data from DATA register
                 * 
                 * 
                 * \return uint8_t          - data from DATA register
                 */
                inline uint8_t Read() 
                {
                    m_pSercom->SPI.DATA.reg = 0xFF;
                    while (!m_pSercom->SPI.INTFLAG.bit.TXC);
                    m_pSercom->SPI.INTFLAG.bit.TXC = true;
                    return m_pSercom->SPI.DATA.reg;
                }

                /**
                 * \brief   Read data
                 * 
                 * \param pData             - pointer do data
                 * \param unLength          - number of data
                 * 
                 * \return void
                 */
                void Read(uint8_t* pData, uint16_t unLength);
        
                /**
                 * \brief   Write data to DATA register
                 * 
                 * \param unByte            - byte to write
                 * 
                 * \return void
                 */
                inline void Send(uint8_t unByte) 
                {   
                    m_pSercom->SPI.DATA.reg = unByte;
                    while (!m_pSercom->SPI.INTFLAG.bit.TXC);
                    m_pSercom->SPI.INTFLAG.bit.TXC = true;
                    uint8_t temp = m_pSercom->SPI.DATA.reg;     /* clear RXC flag */
                };

                /**
                 * \brief  Send data
                 * 
                 * \param pData             - pointer to data
                 * \param unLength          - number of data
                 * 
                 * \return void
                 */
                void Send(uint8_t* pData, uint16_t unLength);

                /**
                 * \brief   Send data and get answer
                 * 
                 * \param unByte            - byte to write
                 * 
                 * \return uint8_t          - answer byte
                 */
                inline uint8_t Transmit(uint8_t unByte)
                {
                    m_pSercom->SPI.DATA.reg = unByte;
                    while (!m_pSercom->SPI.INTFLAG.bit.TXC);
                    m_pSercom->SPI.INTFLAG.bit.TXC = true;
                    return m_pSercom->SPI.DATA.reg;
                }
        };
        
        /************************************************************************/
        /* I2C                                                                  */
        /************************************************************************/
        enum I2C_CMD_enum {
            I2C_CMD_NoAction,
            I2C_CMD_RepeatedStart,
            I2C_CMD_Acknowledge,
            I2C_CMD_Stop,
        };

        enum I2C_BUSSTATE_enum {
            I2C_BUSSTATE_Unknown,
            I2C_BUSSTATE_Idle,
            I2C_BUSSTATE_Owner,
            I2C_BUSSTATE_Busy,
        };

        class I2C: public SERCOM
        {
            private:
                uint8_t m_unAddress = 0;
            
            public:
                /**
                 * \brief   SERCOM I2C constructor
                 * 
                 * \param pSercom           - pointer to SERCOM 
                 * \param unGeneratorNum    - generator clock number
                 * 
                 * \return 
                 */
                I2C(::Sercom *pSercom, uint8_t unGeneratorNum): SERCOM(pSercom, unGeneratorNum) {};
                
                /**
                 * \brief   Initialize I2C master
                 * 
                 * \param unPadIn           - not relevant for I2C
                 * \param unPadOut          - not relevant for I2C
                 * \param unFgen            - clock generator frequency
                 * \param unBaud            - baud rate
                 * 
                 * \return void
                 */
                void Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud);
                
                /**
                 * \brief   Send byte to slave
                 * 
                 * \param unByte            - byte to send
                 * 
                 * \return void
                 */
                void Send(uint8_t unByte);
                
                /**
                 * \brief   Read byte from slave
                 * 
                 * 
                 * \return uint8_t          - byte to read
                 */
                uint8_t Read();

                /**
                 * \brief   Set slave address
                 * 
                 * \param unAddress         - slave address
                 * 
                 * \return void
                 */
                void SetAddress(uint8_t unAddress) {m_unAddress = unAddress;}

                /**
                 * \brief   Send data to slave
                 * 
                 * \param aData             - pointer to data
                 * \param unLength          - data length
                 * 
                 * \return void
                 */
                void Send(uint8_t* aData, uint16_t unLength);

                /**
                 * \brief   Read data from slave
                 * 
                 * \param aData             - pointer to data
                 * \param unLength          - data length
                 * 
                 * \return void
                 */
                void Read(uint8_t* aData, uint16_t unLength);

        };
        
        /************************************************************************/
        /* LIN                                                                  */
        /************************************************************************/
        class LIN: public SERCOM
        {
            private:
            
            public:
                /**
                 * \brief   SERCOM LIN constructor
                 * 
                 * \param pSercom           - pointer to SERCOM 
                 * \param unGeneratorNum    - generator clock number
                 * 
                 * \return 
                 */
                LIN(::Sercom *pSercom, uint8_t unGeneratorNum): SERCOM(pSercom, unGeneratorNum) {};
                
                /**
                 * \brief   Initialize LIN master
                 * 
                 * \param unPadIn           - PAD input value
                 * \param unPadOut          - PAD output value
                 * \param unFgen            - clock generator frequency
                 * \param unBaud            - baud rate
                 * 
                 * \return void
                 */
                void Init(uint8_t unPadIn, uint8_t unPadOut, uint32_t unFgen, uint32_t unBaud);
                
                void Send(uint8_t unByte) {};
                
                uint8_t Read() {};
        };
    }
}

#endif /* SERCOM_H_ */
