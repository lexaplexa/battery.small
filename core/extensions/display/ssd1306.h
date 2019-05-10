/*****************************************************************************
* ssd1306.h
*
* Created: 5.5.2019 7:18:54
* Revised:
* Author: uidm2956
* BOARD:
* ABOUT:
*
*****************************************************************************/


#ifndef SSD1306_H_
#define SSD1306_H_

#include <core/core.h>

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/
#define SSD1306_ADDR                        0x78

/* Set display resolution */
#define SSD1306_128_64
//#define SSD1306_128_32
//#define SSD1306_96_16

#ifdef SSD1306_128_64
#define SSD1306_LCDWIDTH                    128
#define SSD1306_LCDHEIGHT                   64
#endif
#ifdef SSD1306_128_32
#define SSD1306_LCDWIDTH                    128
#define SSD1306_LCDHEIGHT                   32
#endif
#ifdef SSD1306_96_16
#define SSD1306_LCDWIDTH                    96
#define SSD1306_LCDHEIGHT                   16
#endif

#define SSD1306_GDDRAM_BYTE_SIZE            SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8

#define SSD1306_SETCONTRAST                 0x81
#define SSD1306_DISPLAYALLON_RESUME         0xA4
#define SSD1306_DISPLAYALLON                0xA5
#define SSD1306_NORMALDISPLAY               0xA6
#define SSD1306_INVERTDISPLAY               0xA7
#define SSD1306_DISPLAYOFF                  0xAE
#define SSD1306_DISPLAYON                   0xAF

#define SSD1306_SETDISPLAYOFFSET            0xD3
#define SSD1306_SETCOMPINS                  0xDA

#define SSD1306_SETVCOMDETECT               0xDB

#define SSD1306_SETDISPLAYCLOCKDIV          0xD5
#define SSD1306_SETPRECHARGE                0xD9

#define SSD1306_SETMULTIPLEX                0xA8

#define SSD1306_SETLOWCOLUMN                0x00
#define SSD1306_SETHIGHCOLUMN               0x10

#define SSD1306_SETSTARTLINE                0x40

#define SSD1306_MEMORYMODE                  0x20
#define SSD1306_COLUMNADDR                  0x21
#define SSD1306_PAGEADDR                    0x22

#define SSD1306_COMSCANINC                  0xC0
#define SSD1306_COMSCANDEC                  0xC8

#define SSD1306_SEGREMAP                    0xA0

#define SSD1306_CHARGEPUMP                  0x8D

#define SSD1306_EXTERNALVCC                 0x01
#define SSD1306_SWITCHCAPVCC                0x02

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL                         0x2F
#define SSD1306_DEACTIVATE_SCROLL                       0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA                0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL                 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL                  0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL    0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL     0x2A

enum SSD1306_FONT_enum {
    SSD1306_FONT_Small,
    SSD1306_FONT_Large,
    };

namespace Core
{
    namespace Extensions
    {
        struct SSD1306_PICTURE_struct
        {
            uint16_t unByteSize;
            uint8_t unRows;
            uint8_t unColumns;
            uint8_t aData[];
        };
        
        /************************************************************************/
        /* CLASS                                                                */
        /************************************************************************/
        class SSD1306
        {
            private:
            static uint8_t m_GDDRAM[SSD1306_LCDHEIGHT/8][SSD1306_LCDWIDTH];
            const static SSD1306_PICTURE_struct *m_font_small[];
            const static SSD1306_PICTURE_struct *m_font_large[];
            
            public:
            /**
            * \brief   Display initialization
            *
            *
            * \return void
            */
            static void Init();
            
            /**
            * \brief   Write command to display
            *
            * \param command           - command byte
            *
            * \return void
            */
            static void WriteCmd(uint8_t command);
            
            /**
            * \brief   Refresh display GDDRAM data
            *
            *
            * \return void
            */
            static void RefreshDisplay();
            
            /**
            * \brief   Clear display
            *
            *
            * \return void
            */
            static void ClearDisplay();
            
            /**
            * \brief   Print text on display
            *
            * \param pText             - pointer to text string
            * \param unRow             - start row
            * \param unCol             - start column
            *
            * \return void
            */
            static void Println(uint8_t* pText, uint8_t unRow, uint8_t unCol, SSD1306_FONT_enum eFont);
            
            /**
            * \brief   Set display on
            *
            *
            * \return void
            */
            static void On() {WriteCmd(SSD1306_DISPLAYON);}
            
            /**
            * \brief   Set display off
            *
            *
            * \return void
            */
            static void Off() {WriteCmd(SSD1306_DISPLAYOFF);}
        };
    }
}



#endif /* SSD1306_H_ */