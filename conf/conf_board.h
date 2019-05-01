/*************************************************************************
 * conf_board.h
 *
 * Created: 28.4.2019 10:21:04
 * Revised: 
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
#define BOARD_NAME                 /* Board name and version */


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


#endif /* CONF_BOARD_H_ */