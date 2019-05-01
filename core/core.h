/*************************************************************************
 * core.h
 *
 * Created: 28.11.2019 10:19:52
 * Revised: 
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#ifndef CORE_H_
#define CORE_H_

#include <conf/conf_board.h>

#include <core/multitask/multitask.h>

#include <core/coreversion.h>
#include <app/appversion.h>


/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/
#define STR(x)          #x
#define XSTR(x)         STR(x)


/************************************************************************/
/* EXTERNAL CLASSES                                                     */
/************************************************************************/


/************************************************************************/
/* TASK PROTOTYPES                                                      */
/*----------------------------------------------------------------------*/
/* Every task function is named void task<name of task>();              */
/* For example:                                                         */
/* void taskStartUpCore();                                              */
/*                                                                      */
/* File names with source code are named task_<name of task>.cpp        */
/* For example:                                                         */
/* task_startup_core.cpp                                                */
/************************************************************************/
void taskStartUpApp();

#endif /* CORE_H_ */