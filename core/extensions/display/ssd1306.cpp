/*****************************************************************************
* ssd1306.cpp
*
* Created: 5.5.2019 7:22:31
* Revised: 2.6.2019
* Author: uidm2956
* BOARD:
* ABOUT:
*
*****************************************************************************/

#include "ssd1306.h"

using namespace Core::Drivers;

uint8_t Core::Extensions::SSD1306::m_GDDRAM[SSD1306_LCDHEIGHT/8][SSD1306_LCDWIDTH];



I2C cI2c(DISP_SERCOM, 0);

void Core::Extensions::SSD1306::Init()
{
    cI2c.Init(0,0, CPU_FREQ, 400000);
    cI2c.SetAddress(SSD1306_ADDR);
    
    WriteCmd(SSD1306_DISPLAYOFF);
    WriteCmd(SSD1306_SETDISPLAYCLOCKDIV);
    WriteCmd(0x80);

    WriteCmd(SSD1306_SETMULTIPLEX);
    WriteCmd(SSD1306_LCDHEIGHT - 1);

    WriteCmd(SSD1306_SETDISPLAYOFFSET);
    WriteCmd(0x00);
    WriteCmd(SSD1306_SETSTARTLINE | 0x00);
    WriteCmd(SSD1306_CHARGEPUMP);
    WriteCmd(0x14);
    WriteCmd(SSD1306_MEMORYMODE);
    WriteCmd(0x00);
    WriteCmd(SSD1306_SEGREMAP | 0x01);
    WriteCmd(SSD1306_COMSCANDEC);

    #ifdef SSD1306_128_32
    WriteCmd(SSD1306_SETCOMPINS);
    WriteCmd(0x02);
    WriteCmd(SSD1306_SETCONTRAST);
    WriteCmd(0x8F);
    #endif
    #ifdef SSD1306_128_64
    WriteCmd(SSD1306_SETCOMPINS);
    WriteCmd(0x12);
    WriteCmd(SSD1306_SETCONTRAST);
    WriteCmd(0xAF);
    #endif
    #ifdef SSD1306_96_16
    WriteCmd(SSD1306_SETCOMPINS);
    WriteCmd(0x02);
    WriteCmd(SSD1306_SETCONTRAST);
    WriteCmd(0xAF);
    #endif

    WriteCmd(SSD1306_SETPRECHARGE);
    WriteCmd(0xF1);
    WriteCmd(SSD1306_SETVCOMDETECT);
    WriteCmd(0x40);
    WriteCmd(SSD1306_DISPLAYALLON_RESUME);
    WriteCmd(SSD1306_NORMALDISPLAY);

    WriteCmd(SSD1306_DEACTIVATE_SCROLL);
    ClearDisplay();
}

void Core::Extensions::SSD1306::WriteCmd(uint8_t command)
{
    uint8_t aData[2];
    aData[0] = 0x00;
    aData[1] = command;
    cI2c.Send(aData, 2);
}

void Core::Extensions::SSD1306::RefreshDisplay()
{
    WriteCmd(SSD1306_COLUMNADDR);
    WriteCmd(0);                    /* Column start address (0 = reset) */
    WriteCmd(SSD1306_LCDWIDTH-1);   /* Column end address (127 = reset) */
    
    WriteCmd(SSD1306_PAGEADDR);
    WriteCmd(0);                    /* Page start address (0 = reset) */
    #if SSD1306_LCDHEIGHT == 64
    WriteCmd(7);                /* Page end address */
    #endif
    #if SSD1306_LCDHEIGHT == 32
    WriteCmd(3);                /* Page end address */
    #endif
    #if SSD1306_LCDHEIGHT == 16
    WriteCmd(1);                /* Page end address */
    #endif
    
    /* Send Address */
    DISP_SERCOM->I2CM.ADDR.bit.ADDR = SSD1306_ADDR;
    while(!DISP_SERCOM->I2CM.INTFLAG.bit.MB);
    DISP_SERCOM->I2CM.INTFLAG.bit.MB = 1;                       /* Clear interrupt flag */
    if (DISP_SERCOM->I2CM.STATUS.bit.RXNACK) {return;}          /* No acknowledge from slave */
    
    /* Write GDDRAM data */
    DISP_SERCOM->I2CM.DATA.reg = 0x40;                          /* RAM data */
    while(!DISP_SERCOM->I2CM.INTFLAG.bit.MB);
    DISP_SERCOM->I2CM.INTFLAG.bit.MB = 1;                       /* Clear interrupt flag */
    if (DISP_SERCOM->I2CM.STATUS.bit.RXNACK) {return;}          /* No acknowledge from slave */

    /* Send Data */
    for (uint16_t i=0; i<SSD1306_GDDRAM_BYTE_SIZE; i++)
    {
        DISP_SERCOM->I2CM.DATA.reg = *((uint8_t*)m_GDDRAM+i);
        while(!DISP_SERCOM->I2CM.INTFLAG.bit.MB);
        DISP_SERCOM->I2CM.INTFLAG.bit.MB = 1;                   /* Clear interrupt flag */
        if (DISP_SERCOM->I2CM.STATUS.bit.RXNACK) {return;}      /* No acknowledge from slave */
    }

    /* Stop communication */
    DISP_SERCOM->I2CM.CTRLB.bit.CMD = I2C_CMD_Stop;
}

void Core::Extensions::SSD1306::ClearDisplay()
{
    for (uint16_t i=0; i<SSD1306_GDDRAM_BYTE_SIZE; i++) {*((uint8_t*)m_GDDRAM+i) = 0;}
    RefreshDisplay();
}

void Core::Extensions::SSD1306::Println(uint8_t* pText, uint8_t unRow, uint8_t unCol, SSD1306_FONT_enum eFont)
{
    SSD1306_PICTURE_struct *psChar;
    uint8_t unColStart = unCol;
    
    for (pText; *pText != '\0'; pText++)
    {
        if (eFont == SSD1306_FONT_Small)        {psChar = (SSD1306_PICTURE_struct*)m_font_small[*pText - ' '];}
        else if (eFont == SSD1306_FONT_Large)   {psChar = (SSD1306_PICTURE_struct*)m_font_large[*pText - ' '];}
        
        if (*pText == '\n')
        {
            unRow++;
            if (eFont == SSD1306_FONT_Large) {unRow++;}
            unCol = unColStart;
            if (unRow >= SSD1306_LCDHEIGHT/8) {return;}         /* Outside display area */
        }
        else if (*pText < ' ' || *pText > '~') {continue;}
        else
        {
            if ((unCol + psChar->unColumns) >= SSD1306_LCDWIDTH-1)
            {
                unRow++;
                if (eFont == SSD1306_FONT_Large) {unRow++;}
                unCol = unColStart;
                if (unRow >= SSD1306_LCDHEIGHT/8) {return;}     /* Outside display area */
            }
            
            if (eFont == SSD1306_FONT_Small)
            {
                for (uint8_t i=0; i<psChar->unByteSize; i++) {m_GDDRAM[unRow][unCol + i] = psChar->aData[i];}
            }
            else if (eFont == SSD1306_FONT_Large)
            {
                for (uint8_t i=0; i<psChar->unByteSize; i+=2) {m_GDDRAM[unRow][unCol + i/2] = psChar->aData[i]; m_GDDRAM[unRow+1][unCol + i/2] = psChar->aData[i+1];}
            }
            
            unCol += psChar->unColumns;
            if (eFont == SSD1306_FONT_Small) {m_GDDRAM[unRow][unCol++] = 0x00;}         /* Space 1px between characters */
            else if (eFont == SSD1306_FONT_Large)                                       /* Space 2px between characters */
            {
                m_GDDRAM[unRow][unCol] = 0x00; m_GDDRAM[unRow+1][unCol++] = 0x00;
                m_GDDRAM[unRow][unCol] = 0x00; m_GDDRAM[unRow+1][unCol++] = 0x00;
            }
        }
    }
    RefreshDisplay();
}

/************************************************************************/
/* SMALL FONT CHARACTERS                                                */
/************************************************************************/
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_20 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x00,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_21 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_22 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x03,0x00,0x03,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_23 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x28,0x7F,0x14,0x7F,0x0A,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_24 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x24,0x2A,0x6B,0x12}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_25 = {.unByteSize=6, .unRows=8, .unColumns=6, .aData={0x20,0x13,0x0B,0x68,0x64,0x02,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_26 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x36,0x49,0x56,0x20,0x50,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_27 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x03,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_28 = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x3E,0x41,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_29 = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x41,0x3E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_2A = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x05,0x02,0x05,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_2B = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x1C,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_2C = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x40,0x20,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_2D = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x08,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_2E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x40,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_2F = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x40,0x30,0x0C,0x03}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_30 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x3E,0x41,0x41,0x3E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_31 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x04,0x42,0x7F,0x40}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_32 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x62,0x51,0x49,0x46}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_33 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x22,0x41,0x49,0x36}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_34 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x18,0x14,0x12,0x7F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_35 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x27,0x45,0x45,0x39}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_36 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x3E,0x49,0x49,0x32}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_37 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x01,0x61,0x19,0x07}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_38 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x36,0x49,0x49,0x36}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_39 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x26,0x49,0x49,0x3E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_3A = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x28,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_3B = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x40,0x28,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_3C = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x14,0x22,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_3D = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x14,0x14,0x14,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_3E = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x22,0x14,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_3F = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x06,0x01,0x59,0x06}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_40 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x3C,0x42,0x59,0x55,0x4E,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_41 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7E,0x11,0x11,0x7E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_42 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x49,0x49,0x36}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_43 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x3E,0x41,0x41,0x22}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_44 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x41,0x41,0x3E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_45 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x49,0x49,0x41}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_46 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x09,0x09,0x01}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_47 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x3E,0x41,0x49,0x32}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_48 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x08,0x08,0x7F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_49 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x41,0x7F,0x41,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_4A = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x21,0x41,0x41,0x3F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_4B = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x08,0x14,0x63}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_4C = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x40,0x40,0x40}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_4D = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x7F,0x02,0x04,0x02,0x7F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_4E = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x7F,0x06,0x08,0x30,0x7F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_4F = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x3E,0x41,0x41,0x41,0x3E,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_50 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x09,0x09,0x06}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_51 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x3E,0x41,0x51,0x21,0x5E,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_52 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x19,0x29,0x46}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_53 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x26,0x49,0x49,0x32}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_54 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x01,0x01,0x7F,0x01,0x01,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_55 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x3F,0x40,0x40,0x40,0x3F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_56 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x0F,0x30,0x40,0x30,0x0F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_57 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x3F,0x40,0x3C,0x40,0x3F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_58 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x63,0x1C,0x1C,0x63}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_59 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x07,0x08,0x70,0x08,0x07,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_5A = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x61,0x51,0x49,0x45,0x43,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_5B = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x7F,0x41,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_5C = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x03,0x0C,0x30,0x40}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_5D = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x41,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_5E = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x04,0x02,0x01,0x02,0x04,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_5F = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x08,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_60 = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x01,0x02,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_61 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x20,0x54,0x54,0x78}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_62 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x48,0x48,0x30}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_63 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x38,0x44,0x44,0x28}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_64 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x30,0x48,0x48,0x7F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_65 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x38,0x54,0x54,0x18}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_66 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x08,0x7E,0x09,0x02}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_67 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x04,0x2A,0x4A,0x3C}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_68 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7F,0x08,0x08,0x70}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_69 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x7A,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_6A = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x20,0x40,0x3A,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_6B = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x7F,0x10,0x6C,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_6C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x7F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_6D = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x7C,0x04,0x78,0x04,0x78,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_6E = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7C,0x04,0x78,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_6F = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x38,0x44,0x44,0x38}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_70 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x7C,0x12,0x12,0x0C}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_71 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x0C,0x12,0x12,0x7C}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_72 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x7C,0x08,0x04,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_73 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x48,0x54,0x54,0x24}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_74 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x04,0x3F,0x44,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_75 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x3C,0x40,0x20,0x7C}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_76 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x1C,0x20,0x40,0x20,0x1C,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_77 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x3C,0x40,0x3C,0x40,0x3C,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_78 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x6C,0x10,0x6C,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_79 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x4C,0x50,0x3C,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_7A = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x64,0x54,0x4C,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_7B = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x36,0x41,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_7C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x7F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_7D = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x41,0x36,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_7E = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x08,0x04,0x08,0x10,0x08,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_small_7F = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x00,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct *Core::Extensions::SSD1306::m_font_small[] = {
    &ascii_small_20,
    &ascii_small_21,
    &ascii_small_22,
    &ascii_small_23,
    &ascii_small_24,
    &ascii_small_25,
    &ascii_small_26,
    &ascii_small_27,
    &ascii_small_28,
    &ascii_small_29,
    &ascii_small_2A,
    &ascii_small_2B,
    &ascii_small_2C,
    &ascii_small_2D,
    &ascii_small_2E,
    &ascii_small_2F,
    &ascii_small_30,
    &ascii_small_31,
    &ascii_small_32,
    &ascii_small_33,
    &ascii_small_34,
    &ascii_small_35,
    &ascii_small_36,
    &ascii_small_37,
    &ascii_small_38,
    &ascii_small_39,
    &ascii_small_3A,
    &ascii_small_3B,
    &ascii_small_3C,
    &ascii_small_3D,
    &ascii_small_3E,
    &ascii_small_3F,
    &ascii_small_40,
    &ascii_small_41,
    &ascii_small_42,
    &ascii_small_43,
    &ascii_small_44,
    &ascii_small_45,
    &ascii_small_46,
    &ascii_small_47,
    &ascii_small_48,
    &ascii_small_49,
    &ascii_small_4A,
    &ascii_small_4B,
    &ascii_small_4C,
    &ascii_small_4D,
    &ascii_small_4E,
    &ascii_small_4F,
    &ascii_small_50,
    &ascii_small_51,
    &ascii_small_52,
    &ascii_small_53,
    &ascii_small_54,
    &ascii_small_55,
    &ascii_small_56,
    &ascii_small_57,
    &ascii_small_58,
    &ascii_small_59,
    &ascii_small_5A,
    &ascii_small_5B,
    &ascii_small_5C,
    &ascii_small_5D,
    &ascii_small_5E,
    &ascii_small_5F,
    &ascii_small_60,
    &ascii_small_61,
    &ascii_small_62,
    &ascii_small_63,
    &ascii_small_64,
    &ascii_small_65,
    &ascii_small_66,
    &ascii_small_67,
    &ascii_small_68,
    &ascii_small_69,
    &ascii_small_6A,
    &ascii_small_6B,
    &ascii_small_6C,
    &ascii_small_6D,
    &ascii_small_6E,
    &ascii_small_6F,
    &ascii_small_70,
    &ascii_small_71,
    &ascii_small_72,
    &ascii_small_73,
    &ascii_small_74,
    &ascii_small_75,
    &ascii_small_76,
    &ascii_small_77,
    &ascii_small_78,
    &ascii_small_79,
    &ascii_small_7A,
    &ascii_small_7B,
    &ascii_small_7C,
    &ascii_small_7D,
    &ascii_small_7E,
    &ascii_small_7F
};

/************************************************************************/
/* LARGE FONT CHARACTERS                                                */
/************************************************************************/
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_20 = {.unByteSize=4, .unRows=16, .unColumns=2, .aData={0x00,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_21 = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0xFE,0x23,0xFF,0x77,0xFE,0x23,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_22 = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x0F,0x00,0x00,0x00,0x0F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_23 = {.unByteSize=16, .unRows=16, .unColumns=8, .aData={0x20,0x04,0x20,0x7C,0xE0,0x07,0x3E,0x04,0x20,0x7C,0xE0,0x07,0x3E,0x04,0x20,0x04}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_24 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x30,0x08,0x48,0x10,0x84,0x20,0xFE,0x7F,0x04,0x21,0x08,0x12,0x10,0x0C,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_25 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x00,0x60,0x0C,0x18,0x12,0x06,0x8C,0x31,0x60,0x48,0x18,0x30,0x06,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_26 = {.unByteSize=16, .unRows=16, .unColumns=8, .aData={0x18,0x0F,0xA4,0x10,0x42,0x20,0xA4,0x20,0x18,0x13,0x00,0x0C,0x00,0x14,0x00,0x22}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_27 = {.unByteSize=2, .unRows=16, .unColumns=1, .aData={0x0F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_28 = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0xF8,0x0F,0x06,0x30,0x01,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_29 = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x01,0x40,0x06,0x30,0xF8,0x0F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_2A = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x2A,0x00,0x1C,0x00,0x3E,0x00,0x1C,0x00,0x2A,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_2B = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x80,0x00,0x80,0x00,0xE0,0x03,0x80,0x00,0x80,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_2C = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x00,0x40,0x00,0x3C,0x00,0x1C,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_2D = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_2E = {.unByteSize=4, .unRows=16, .unColumns=2, .aData={0x00,0x60,0x00,0x60}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_2F = {.unByteSize=16, .unRows=16, .unColumns=8, .aData={0x00,0x60,0x00,0x18,0x00,0x06,0x80,0x01,0x60,0x00,0x18,0x00,0x06,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_30 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0xF0,0x0F,0x0C,0x30,0x02,0x40,0x02,0x40,0x0C,0x30,0xF0,0x0F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_31 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x10,0x00,0x08,0x40,0x04,0x40,0xFE,0x7F,0x00,0x40,0x00,0x40}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_32 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x08,0x60,0x04,0x50,0x02,0x48,0x02,0x44,0x04,0x43,0xF8,0x40}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_33 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x08,0x10,0x04,0x20,0x02,0x40,0x82,0x40,0x44,0x21,0x38,0x1E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_34 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x80,0x03,0x60,0x02,0x18,0x02,0x04,0x02,0xFE,0x7F,0x00,0x02}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_35 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0xFE,0x20,0x82,0x40,0x82,0x40,0x82,0x40,0x02,0x21,0x00,0x1E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_36 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0xF8,0x1F,0x04,0x21,0x82,0x40,0x82,0x40,0x02,0x21,0x04,0x1E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_37 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x02,0x60,0x02,0x18,0x02,0x06,0x82,0x01,0x62,0x00,0x1E,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_38 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x38,0x1E,0x44,0x21,0x82,0x40,0x82,0x40,0x44,0x21,0x38,0x1E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_39 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x38,0x10,0x44,0x20,0x82,0x40,0x82,0x40,0x44,0x20,0xF8,0x1F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_3A = {.unByteSize=4, .unRows=16, .unColumns=2, .aData={0x60,0x0C,0x60,0x0C}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_3B = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x00,0x40,0x60,0x3C,0x60,0x1C,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_3C = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x01,0x80,0x02,0x40,0x04,0x20,0x08,0x10,0x10,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_3D = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x60,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x0C,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_3E = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x10,0x10,0x20,0x08,0x40,0x04,0x80,0x02,0x00,0x01,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_3F = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0x08,0x00,0x04,0x00,0x02,0x4F,0x82,0x00,0x44,0x00,0x38,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_40 = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0xC0,0x1F,0x20,0x20,0x90,0x4F,0x50,0x50,0x50,0x50,0xD0,0x5F,0x10,0x48,0x20,0x50,0xC0,0x4F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_41 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xC0,0x7F,0xB0,0x00,0x8C,0x00,0x82,0x00,0x8C,0x00,0xB0,0x00,0xC0,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_42 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x82,0x40,0x82,0x40,0x82,0x40,0x82,0x40,0x44,0x21,0x38,0x1E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_43 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xF8,0x1F,0x04,0x20,0x02,0x40,0x02,0x40,0x02,0x40,0x04,0x20,0x18,0x18,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_44 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x02,0x40,0x02,0x40,0x02,0x40,0x04,0x20,0x08,0x10,0xF0,0x0F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_45 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x82,0x40,0x82,0x40,0x82,0x40,0x82,0x40,0x02,0x40,0x02,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_46 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x02,0x00,0x02,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_47 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xF8,0x1F,0x04,0x20,0x02,0x40,0x02,0x41,0x02,0x41,0x04,0x21,0x08,0x1F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_48 = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0xFE,0x7F,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xFE,0x7F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_49 = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x02,0x40,0xFE,0x7F,0x02,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_4A = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x00,0x1C,0x02,0x20,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x20,0xFE,0x1F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_4B = {.unByteSize=12, .unRows=16, .unColumns=6, .aData={0xFE,0x7F,0x80,0x00,0x40,0x01,0x20,0x06,0x18,0x18,0x06,0x60}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_4C = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_4D = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0xFE,0x7F,0x04,0x00,0x08,0x00,0x10,0x00,0x20,0x00,0x10,0x00,0x08,0x00,0x04,0x00,0xFE,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_4E = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0xFE,0x7F,0x04,0x00,0x18,0x00,0x60,0x00,0x80,0x01,0x00,0x06,0x00,0x18,0x00,0x20,0xFE,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_4F = {.unByteSize=16, .unRows=16, .unColumns=8, .aData={0xF8,0x1F,0x04,0x20,0x02,0x40,0x02,0x40,0x02,0x40,0x02,0x40,0x04,0x20,0xF8,0x1F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_50 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x82,0x00,0x82,0x00,0x82,0x00,0x82,0x00,0x44,0x00,0x38,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_51 = {.unByteSize=16, .unRows=16, .unColumns=8, .aData={0xF8,0x1F,0x04,0x20,0x02,0x40,0x02,0x40,0x02,0x48,0x02,0x50,0x04,0x20,0xF8,0x5F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_52 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0xFE,0x7F,0x82,0x01,0x82,0x01,0x82,0x02,0x82,0x04,0x44,0x18,0x38,0x60,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_53 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x38,0x18,0x44,0x20,0x82,0x40,0x82,0x40,0x82,0x40,0x04,0x21,0x08,0x1E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_54 = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0xFE,0x7F,0x02,0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_55 = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0xFE,0x0F,0x00,0x10,0x00,0x20,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x10,0xFE,0x0F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_56 = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0xFE,0x00,0x00,0x03,0x00,0x0C,0x00,0x30,0x00,0x40,0x00,0x30,0x00,0x0C,0x00,0x03,0xFE,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_57 = {.unByteSize=18, .unRows=16, .unColumns=9, .aData={0xFE,0x0F,0x00,0x30,0x00,0x40,0x00,0x30,0xF0,0x0F,0x00,0x30,0x00,0x40,0x00,0x30,0xFE,0x0F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_58 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x06,0x60,0x18,0x18,0x60,0x06,0x80,0x01,0x60,0x06,0x18,0x18,0x06,0x60,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_59 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x06,0x00,0x18,0x00,0x60,0x00,0x80,0x7F,0x60,0x00,0x18,0x00,0x06,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_5A = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x02,0x60,0x02,0x58,0x02,0x46,0x82,0x41,0x62,0x40,0x1A,0x40,0x06,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_5B = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0xFE,0x7F,0x02,0x40,0x02,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_5C = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x06,0x00,0x18,0x00,0x60,0x00,0x80,0x01,0x00,0x06,0x00,0x18,0x00,0x60,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_5D = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x02,0x40,0x02,0x40,0xFE,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_5E = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x08,0x00,0x04,0x00,0x02,0x00,0x04,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_5F = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_60 = {.unByteSize=6, .unRows=16, .unColumns=3, .aData={0x02,0x00,0x04,0x00,0x08,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_61 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x32,0x00,0x49,0x00,0x49,0x00,0x49,0x00,0x7E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_62 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0xF8,0x7F,0x00,0x41,0x00,0x41,0x00,0x41,0x00,0x3E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_63 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x3E,0x00,0x41,0x00,0x41,0x00,0x41,0x00,0x22,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_64 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x3E,0x00,0x41,0x00,0x41,0x00,0x41,0xF8,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_65 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x3E,0x00,0x49,0x00,0x49,0x00,0x49,0x00,0x2E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_66 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x80,0x00,0xF0,0x7F,0x88,0x00,0x08,0x00,0x10,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_67 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x4E,0x00,0x91,0x00,0x91,0x00,0x92,0x00,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_68 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0xF8,0x7F,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x7E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_69 = {.unByteSize=2, .unRows=16, .unColumns=1, .aData={0x40,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_6A = {.unByteSize=8, .unRows=16, .unColumns=4, .aData={0x00,0x40,0x00,0x80,0x00,0x80,0x40,0x7F}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_6B = {.unByteSize=8, .unRows=16, .unColumns=4, .aData={0xF8,0x7F,0x00,0x08,0x00,0x14,0x00,0x63}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_6C = {.unByteSize=2, .unRows=16, .unColumns=1, .aData={0xF8,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_6D = {.unByteSize=16, .unRows=16, .unColumns=8, .aData={0x00,0x7F,0x00,0x02,0x00,0x01,0x00,0x01,0x00,0x7E,0x00,0x01,0x00,0x01,0x00,0x7E}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_6E = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x7F,0x00,0x02,0x00,0x01,0x00,0x01,0x00,0x7E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_6F = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x3E,0x00,0x41,0x00,0x41,0x00,0x41,0x00,0x3E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_70 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0xFF,0x00,0x12,0x00,0x11,0x00,0x11,0x00,0x0E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_71 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x0E,0x00,0x11,0x00,0x11,0x00,0x12,0x00,0xFF,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_72 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x7F,0x00,0x02,0x00,0x01,0x00,0x01,0x00,0x02,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_73 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x26,0x00,0x49,0x00,0x49,0x00,0x49,0x00,0x32,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_74 = {.unByteSize=8, .unRows=16, .unColumns=4, .aData={0x40,0x00,0xF8,0x3F,0x40,0x40,0x00,0x20}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_75 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x3F,0x00,0x40,0x00,0x40,0x00,0x20,0x00,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_76 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x0F,0x00,0x30,0x00,0x40,0x00,0x30,0x00,0x0F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_77 = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x00,0x0F,0x00,0x30,0x00,0x40,0x00,0x38,0x00,0x40,0x00,0x30,0x00,0x0F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_78 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x63,0x00,0x14,0x00,0x08,0x00,0x14,0x00,0x63,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_79 = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x47,0x00,0x88,0x00,0x88,0x00,0x88,0x00,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_7A = {.unByteSize=10, .unRows=16, .unColumns=5, .aData={0x00,0x61,0x00,0x51,0x00,0x49,0x00,0x45,0x00,0x43,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_7B = {.unByteSize=8, .unRows=16, .unColumns=4, .aData={0x80,0x00,0x7C,0x3F,0x02,0x40,0x02,0x40}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_7C = {.unByteSize=2, .unRows=16, .unColumns=1, .aData={0xFE,0x7F,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_7D = {.unByteSize=8, .unRows=16, .unColumns=4, .aData={0x02,0x40,0x02,0x40,0x7C,0x3F,0x80,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_7E = {.unByteSize=14, .unRows=16, .unColumns=7, .aData={0x00,0x02,0x00,0x01,0x00,0x01,0x00,0x02,0x00,0x04,0x00,0x04,0x00,0x02,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_large_7F = {.unByteSize=2, .unRows=16, .unColumns=1, .aData={0x00,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct *Core::Extensions::SSD1306::m_font_large[] = {
    &ascii_large_20,
    &ascii_large_21,
    &ascii_large_22,
    &ascii_large_23,
    &ascii_large_24,
    &ascii_large_25,
    &ascii_large_26,
    &ascii_large_27,
    &ascii_large_28,
    &ascii_large_29,
    &ascii_large_2A,
    &ascii_large_2B,
    &ascii_large_2C,
    &ascii_large_2D,
    &ascii_large_2E,
    &ascii_large_2F,
    &ascii_large_30,
    &ascii_large_31,
    &ascii_large_32,
    &ascii_large_33,
    &ascii_large_34,
    &ascii_large_35,
    &ascii_large_36,
    &ascii_large_37,
    &ascii_large_38,
    &ascii_large_39,
    &ascii_large_3A,
    &ascii_large_3B,
    &ascii_large_3C,
    &ascii_large_3D,
    &ascii_large_3E,
    &ascii_large_3F,
    &ascii_large_40,
    &ascii_large_41,
    &ascii_large_42,
    &ascii_large_43,
    &ascii_large_44,
    &ascii_large_45,
    &ascii_large_46,
    &ascii_large_47,
    &ascii_large_48,
    &ascii_large_49,
    &ascii_large_4A,
    &ascii_large_4B,
    &ascii_large_4C,
    &ascii_large_4D,
    &ascii_large_4E,
    &ascii_large_4F,
    &ascii_large_50,
    &ascii_large_51,
    &ascii_large_52,
    &ascii_large_53,
    &ascii_large_54,
    &ascii_large_55,
    &ascii_large_56,
    &ascii_large_57,
    &ascii_large_58,
    &ascii_large_59,
    &ascii_large_5A,
    &ascii_large_5B,
    &ascii_large_5C,
    &ascii_large_5D,
    &ascii_large_5E,
    &ascii_large_5F,
    &ascii_large_60,
    &ascii_large_61,
    &ascii_large_62,
    &ascii_large_63,
    &ascii_large_64,
    &ascii_large_65,
    &ascii_large_66,
    &ascii_large_67,
    &ascii_large_68,
    &ascii_large_69,
    &ascii_large_6A,
    &ascii_large_6B,
    &ascii_large_6C,
    &ascii_large_6D,
    &ascii_large_6E,
    &ascii_large_6F,
    &ascii_large_70,
    &ascii_large_71,
    &ascii_large_72,
    &ascii_large_73,
    &ascii_large_74,
    &ascii_large_75,
    &ascii_large_76,
    &ascii_large_77,
    &ascii_large_78,
    &ascii_large_79,
    &ascii_large_7A,
    &ascii_large_7B,
    &ascii_large_7C,
    &ascii_large_7D,
    &ascii_large_7E,
    &ascii_large_7F
};

