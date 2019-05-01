/*************************************************************************
 * conf_board.h
 *
 * Created: 28.4.2019 10:21:04
 * Revised: 1.5.2018
 * Author: LeXa
 * BOARD: 
 *
 * ABOUT:
 * Configuration of board
 *************************************************************************/


#ifndef CONF_BOARD_H_
#define CONF_BOARD_H_

/************************************************************************/
/* BOARD                                                                */
/************************************************************************/
#define Battery_Management_A                 /* Board name and version */


/************************************************************************/
/* SYSTEM CLOCK OPTIONS                                                 */
/************************************************************************/
#define CPU_FREQ                    SOURCE_FREQ*MULTIPLICATION_FREQ
#define SOURCE_FREQ                 1000000
#define MULTIPLICATION_FREQ         1       /* Used only in DPLL */
#define DPLL_CLKREF                 1       /* DPLL reference clock (0 - XOSC32K, 1 - XOSC, 2 - GCLK) */


/************************************************************************/
/* PIN OPTIONS                                                          */
/*----------------------------------------------------------------------*/
/* All pins configuration (Port, pin position, multiplexer,...)         */
/************************************************************************/
/* POWER SUPPLY */
#define VOUT_EN_pin             PIN_PA14
#define VOUT_EN_msk             PORT_PA14
#define VIN_EN_pin              PIN_PA11
#define VIN_EN_msk              PORT_PA11
#define LOW_POWER_pin           PIN_PA28
#define LOW_POWER_msk           PORT_PA28

#define vout_on()               REG_PORT_OUTCLR0 = VOUT_EN_msk;
#define vout_off()              REG_PORT_OUTSET0 = VOUT_EN_msk;
#define vin_on()                REG_PORT_OUTCLR0 = VIN_EN_msk;
#define vin_off()               REG_PORT_OUTSET0 = VIN_EN_msk;
#define low_power_on()          REG_PORT_OUTCLR0 = LOW_POWER_msk;
#define low_power_off()         REG_PORT_OUTSET0 = LOW_POWER_msk;

/* BATTERY CELLS */
#define VCELL1_pin              PIN_PA04
#define VCELL1_msk              PORT_PA04
#define VCELL1_pinmux           MUX_PA04B_ADC_AIN4
#define VCELL2_pin              PIN_PA05
#define VCELL2_msk              PORT_PA05
#define VCELL2_pinmux           MUX_PA05B_ADC_AIN5
#define VCELL3_pin              PIN_PA06
#define VCELL3_msk              PORT_PA06
#define VCELL3_pinmux           MUX_PA06B_ADC_AIN6
#define VCELL4_pin              PIN_PA07
#define VCELL4_msk              PORT_PA07
#define VCELL4_pinmux           MUX_PA07B_ADC_AIN7
#define VCELL5_pin              PIN_PA08
#define VCELL5_msk              PORT_PA08
#define VCELL5_pinmux           MUX_PA08B_ADC_AIN16
#define VCELL6_pin              PIN_PA09
#define VCELL6_msk              PORT_PA09
#define VCELL6_pinmux           MUX_PA09B_ADC_AIN17
#define VCELL7_pin              PIN_PA10
#define VCELL7_msk              PORT_PA10
#define VCELL7_pinmux           MUX_PA10B_ADC_AIN18

#define VCELL1_ADC_MUXPOS       4
#define VCELL2_ADC_MUXPOS       5
#define VCELL3_ADC_MUXPOS       6
#define VCELL4_ADC_MUXPOS       7
#define VCELL5_ADC_MUXPOS       16
#define VCELL6_ADC_MUXPOS       17
#define VCELL7_ADC_MUXPOS       18

/* DISPLAY */
#define DISP_SDA_pin            PIN_PA16
#define DISP_SDA_msk            PORT_PA16
#define DISP_SDA_pinmux         MUX_PA16C_SERCOM1_PAD0
#define DISP_SCL_pin            PIN_PA17
#define DISP_SCL_msk            PORT_PA17
#define DISP_SCL_pinmux         MUX_PA17C_SERCOM1_PAD1

/* BUTTONS */
#define BUTTON_LEFT_pin         PIN_PA15
#define BUTTON_LEFT_msk         PORT_PA15
#define BUTTON_LEFT_pinmux      MUX_PA15A_EIC_EXTINT15
#define BUTTON_LEFT_extint      15
#define BUTTON_RIGHT_pin        PIN_PA18
#define BUTTON_RIGHT_msk        PORT_PA18
#define BUTTON_RIGHT_pinmux     MUX_PA18A_EIC_EXTINT2
#define BUTTON_RIGHT_extint     2

/* RF */
#define RF_CE_pin               PIN_PA19
#define RF_CE_msk               PORT_PA19
#define RF_MOSI_pin             PIN_PA22
#define RF_MOSI_msk             PORT_PA22
#define RF_MOSI_pinmux          MUX_PA22C_SERCOM3_PAD0
#define RF_SCK_pin              PIN_PA23
#define RF_SCK_msk              PORT_PA23
#define RF_SCK_pinmux           MUX_PA23C_SERCOM3_PAD1
#define RF_CS_pin               PIN_PA24
#define RF_CS_msk               PORT_PA24
#define RF_CS_pinmux            MUX_PA24C_SERCOM3_PAD2
#define RF_MISO_pin             PIN_PA25
#define RF_MISO_msk             PORT_PA25
#define RF_MISO_pinmux          MUX_PA25C_SERCOM3_PAD3
#define RF_IRQ_pin              PIN_PA27
#define RF_IRQ_msk              PORT_PA27
#define RF_IRQ_pinmux           MUX_PA27A_EIC_EXTINT15

#endif /* CONF_BOARD_H_ */