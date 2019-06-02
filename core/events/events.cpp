/*************************************************************************
 * events.cpp
 *
 * Created: 2.6.2019 17:13:01
 * Revised:
 * Author: LeXa
 * BOARD:
 *
 * ABOUT:
 *
 *************************************************************************/

#include <core/core.h>
using namespace Core::Extensions;
using namespace Core::Multitask;

void EIC_Handler()
{
    if (REG_EIC_INTFLAG & (1<<BUTTON_LEFT_extint))
    {
        if(BUTTONS::GetEvent(BUTTONS_EVENT_PushEnter) != 0) {MTASK::Run((FuncPtr_t)BUTTONS::GetEvent(BUTTONS_EVENT_PushEnter));}
    }
    if (REG_EIC_INTFLAG & (1<<BUTTON_RIGHT_extint))
    {
        if(BUTTONS::GetEvent(BUTTONS_EVENT_PushNext) != 0) {MTASK::Run((FuncPtr_t)BUTTONS::GetEvent(BUTTONS_EVENT_PushNext));}
    }
    
    EIC->INTFLAG.reg = 0xFFFFFFFF;      /* Clear flags */
}