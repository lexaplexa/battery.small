/*****************************************************************************
* ssd1306.cpp
*
* Created: 5.5.2019 7:22:31
* Revised:
* Author: uidm2956
* BOARD:
* ABOUT:
*
*****************************************************************************/

#include "ssd1306.h"

using namespace Core::Drivers;

uint8_t Core::Extensions::SSD1306::m_GDDRAM[SSD1306_LCDHEIGHT/8][SSD1306_LCDWIDTH];

/************************************************************************/
/* SMALL FONT CHARACTERS                                                */
/************************************************************************/
const Core::Extensions::SSD1306_PICTURE_struct ascii_20 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x00,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_21 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_22 = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x03,0x00,0x03,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_23 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x28,0x7F,0x14,0x7F,0x0A,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_24 = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x24,0x2A,0x6B,0x12}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_25 = {.unByteSize=6, .unRows=8, .unColumns=6, .aData={0x20,0x13,0x0B,0x68,0x64,0x02,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_26 = {.unByteSize=5, .unRows=8, .unColumns=5, .aData={0x36,0x49,0x56,0x20,0x50,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_27 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x03,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_28 = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x3E,0x41,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_29 = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x41,0x3E,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_2A = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x05,0x02,0x05,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_2B = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x1C,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_2C = {.unByteSize=2, .unRows=8, .unColumns=2, .aData={0x40,0x20,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_2D = {.unByteSize=3, .unRows=8, .unColumns=3, .aData={0x08,0x08,0x08,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_2E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x40,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_2F = {.unByteSize=4, .unRows=8, .unColumns=4, .aData={0x40,0x30,0x0C,0x03}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_30 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_31 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_32 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_33 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_34 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_35 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_36 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_37 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_38 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_39 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_3A = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_3B = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_3C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_3D = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_3E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_3F = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_40 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_41 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_42 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_43 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_44 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_45 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_46 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_47 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_48 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_49 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_4A = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_4B = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_4C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_4D = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_4E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_4F = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_50 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_51 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_52 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_53 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_54 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_55 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_56 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_57 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_58 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_59 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_5A = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_5B = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_5C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_5D = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_5E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_5F = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_60 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_61 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_62 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_63 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_64 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_65 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_66 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_67 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_68 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_69 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_6A = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_6B = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_6C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_6D = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_6E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_6F = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_70 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_71 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_72 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_73 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_74 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_75 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_76 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_77 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_78 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_79 = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_7A = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_7B = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_7C = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_7D = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_7E = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct ascii_7F = {.unByteSize=1, .unRows=8, .unColumns=1, .aData={0x5F,0x00,0x00,0x00}};
const Core::Extensions::SSD1306_PICTURE_struct *Core::Extensions::SSD1306::m_font_small[] = {
    &ascii_20,
    &ascii_21,
    &ascii_22,
    &ascii_23,
    &ascii_24,
    &ascii_25,
    &ascii_26,
    &ascii_27,
    &ascii_28,
    &ascii_29,
    &ascii_2A,
    &ascii_2B,
    &ascii_2C,
    &ascii_2D,
    &ascii_2E,
    &ascii_2F,
    &ascii_30,
    &ascii_31,
    &ascii_32,
    &ascii_33,
    &ascii_34,
    &ascii_35,
    &ascii_36,
    &ascii_37,
    &ascii_38,
    &ascii_39,
    &ascii_3A,
    &ascii_3B,
    &ascii_3C,
    &ascii_3D,
    &ascii_3E,
    &ascii_3F,
    &ascii_40,
    &ascii_41,
    &ascii_42,
    &ascii_43,
    &ascii_44,
    &ascii_45,
    &ascii_46,
    &ascii_47,
    &ascii_48,
    &ascii_49,
    &ascii_4A,
    &ascii_4B,
    &ascii_4C,
    &ascii_4D,
    &ascii_4E,
    &ascii_4F,
    &ascii_50,
    &ascii_51,
    &ascii_52,
    &ascii_53,
    &ascii_54,
    &ascii_55,
    &ascii_56,
    &ascii_57,
    &ascii_58,
    &ascii_59,
    &ascii_5A,
    &ascii_5B,
    &ascii_5C,
    &ascii_5D,
    &ascii_5E,
    &ascii_5F,
    &ascii_60,
    &ascii_61,
    &ascii_62,
    &ascii_63,
    &ascii_64,
    &ascii_65,
    &ascii_66,
    &ascii_67,
    &ascii_68,
    &ascii_69,
    &ascii_6A,
    &ascii_6B,
    &ascii_6C,
    &ascii_6D,
    &ascii_6E,
    &ascii_6F,
    &ascii_70,
    &ascii_71,
    &ascii_72,
    &ascii_73,
    &ascii_74,
    &ascii_75,
    &ascii_76,
    &ascii_77,
    &ascii_78,
    &ascii_79,
    &ascii_7A,
    &ascii_7B,
    &ascii_7C,
    &ascii_7D,
    &ascii_7E,
    &ascii_7F
};

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

void Core::Extensions::SSD1306::Println(uint8_t* pText, uint8_t unRow, uint8_t unCol)
{
    SSD1306_PICTURE_struct *psChar;
    uint8_t unColStart = unCol;
    
    for (pText; *pText != '\0'; pText++)
    {
        psChar = (SSD1306_PICTURE_struct*)m_font_small[*pText - ' '];
        if (*pText == '\n')
        {
            unRow++;
            unCol = unColStart;
            if (unRow >= SSD1306_LCDHEIGHT/8) {return;}         /* Outside display area */
        }
        else if (*pText > '~') {continue;}
        else
        {
            for (uint8_t i=0; i<psChar->unByteSize; i++)
            {
                m_GDDRAM[unRow][unCol + i] = psChar->aData[i];
            }
            unCol += psChar->unColumns;
            
            if (unCol >= SSD1306_LCDWIDTH)
            {
                unRow++;
                unCol = unColStart;
                if (unRow >= SSD1306_LCDHEIGHT/8) {return;}     /* Outside display area */
            }
            else
            {
                m_GDDRAM[unRow][unCol++] = 0x00;                /* Space 1px between characters */
            }
        }
    }
    
    RefreshDisplay();
}



