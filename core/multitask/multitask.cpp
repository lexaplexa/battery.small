/*****************************************************************************
* multitask.cpp
*
* Created: 31.3.2016 15:15:18
* Revised: 22.5.2019
* Author: uidm2956
* BOARD:
* ABOUT:
*
*****************************************************************************/

#include "multitask.h"

#define SysTimeOverflow             (m_unSysTime > 0x7FFFFFFF)

namespace Core::Multitask
{
    uint32_t MTASK::m_unSysTime = 0;
    uint8_t MTASK::m_unActiveTasks = 0;
    uint8_t MTASK::m_unHighestPrio = 0;
    bool MTASK::m_bDeepSleepEnabled = false;
    bool MTASK::m_bSchedulerRunning = false;
    TASK_struct* MTASK::m_psLastTask = nullptr;
    TASK_struct* MTASK::m_psCurrentTask = nullptr;
    FuncPtr_t MTASK::peventBeforeDeepSleep = nullptr;
    FuncPtr_t MTASK::peventAfterWakeUp = nullptr;
    
    inline void MTASK::Init(uint32_t unCpuFreq, uint32_t unTicksPerSecond)
    {
        /* Set tick counter */
        SysTick_Config(unCpuFreq/unTicksPerSecond);
    }
    
    inline void MTASK::TickElapsed()
    {
        /* Increment system time */
        m_unSysTime++;
    
        /* When system time overflows, most significant bit of every time match needs to be cleared */
        if (SysTimeOverflow)
        {
            m_unSysTime = 0;
            for (TASK_struct* pTask = m_psLastTask; pTask != nullptr; pTask = pTask->psParent)
            {
                pTask->nTimeMatch &= 0x7FFFFFFF;
            }
        }
    
        /* Increment time match if task suspended */
        for (TASK_struct* pTask = m_psLastTask; pTask != nullptr; pTask = pTask->psParent)
        {
            if (pTask->bSuspend) {pTask->nTimeMatch++;}
        }
    }
    
    inline void MTASK::Schedule()
    {
        if (m_bSchedulerRunning) {return;}
        m_bSchedulerRunning = true;

        /* Main loop */
        while (1)
        {
            /* Clear to default values */
            m_unHighestPrio = 0;
            m_unActiveTasks = 0;
            m_psCurrentTask = nullptr;
            
            /* Find task with highest priority */
            for (TASK_struct* pTask = m_psLastTask; pTask != nullptr; pTask = pTask->psParent)
            {
                if (pTask->bSuspend == false) {m_unActiveTasks++;}
                if (m_unSysTime >= pTask->nTimeMatch && pTask->unPriority >= m_unHighestPrio)
                {
                    m_psCurrentTask = pTask;
                    m_unHighestPrio = pTask->unPriority;
                }
            }
            
            /* Run task if available */
            if (m_psCurrentTask != nullptr)
            {
                FuncPtr_t pTaskFunc = m_psCurrentTask->pTaskFunc;
                if (m_psCurrentTask->bRepeat) {m_psCurrentTask->nTimeMatch = m_unSysTime + m_psCurrentTask->unTimeOut;}
                else {DeleteTask(m_psCurrentTask);}
                /* Run task function */
                pTaskFunc();
            }
            /* Controller can be put into deep sleep if no tasks are active.
            * Multi task counter is not running. There are limited wake-up sources. */
            else if (m_bDeepSleepEnabled && !m_unActiveTasks)
            {
                /* Run event before going to deep sleep */
                if (peventBeforeDeepSleep != nullptr) {peventBeforeDeepSleep();}
                
                #ifdef _SAMC21_
                REG_PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_STANDBY;
                #endif
                #ifdef _SAMD21_
                SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
                #endif
                __WFI();
                
                /* Run event after wake up */
                if (peventAfterWakeUp != nullptr) {peventAfterWakeUp();}
            }
            /* If no task is executed, CPU is going to idle mode.
            * Wait for next interrupt (tick or some other interrupt) */
            else
            {
                Sleep();
            }
        }
    }
    
    
    TASK_struct* MTASK::CreateTask()
    {
        TASK_struct* psNewTask = new TASK_struct;
        psNewTask->psParent = m_psLastTask;
        m_psLastTask = psNewTask;
    }
    
    
    TASK_struct* MTASK::FindTask(FuncPtr_t pTaskFunc)
    {
        for (TASK_struct* pTask = m_psLastTask; pTask != nullptr; pTask = pTask->psParent)
        {
            if (pTask->pTaskFunc == pTaskFunc) {return pTask;}
        }
        return nullptr;
    }
    
    
    void MTASK::DeleteTask(TASK_struct* pTask)
    {
        if (m_psLastTask == pTask)
        {
            m_psLastTask = pTask->psParent;
            delete pTask;
            return;
        }
        for (TASK_struct* pTaskTemp = m_psLastTask; pTaskTemp != nullptr; pTaskTemp = pTaskTemp->psParent)
        {
            if (pTaskTemp->psParent == pTask)
            {
                pTaskTemp->psParent = pTask->psParent;
                delete pTask;
                return;
            }
        }
    }
    
    
    void MTASK::Run(FuncPtr_t taskFunc)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask == nullptr) {pTask = CreateTask();}
        pTask->pTaskFunc = taskFunc;
        pTask->nTimeMatch = m_unSysTime;
        pTask->unTimeOut = 0;
        pTask->unPriority = 0;
        pTask->bSuspend = 0;
        pTask->bRepeat = 0;;
    }
    
    
    void MTASK::Run(FuncPtr_t taskFunc, uint8_t unPriority)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask == nullptr) {pTask = CreateTask();}
        pTask->pTaskFunc = taskFunc;
        pTask->nTimeMatch = m_unSysTime;
        pTask->unTimeOut = 0;
        pTask->unPriority = unPriority;
        pTask->bSuspend = 0;
        pTask->bRepeat = 0;
    }
    
    
    void MTASK::Delay(FuncPtr_t taskFunc, uint16_t unTimeout)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask == nullptr) {pTask = CreateTask();}
        pTask->pTaskFunc = taskFunc;
        pTask->nTimeMatch = m_unSysTime + unTimeout;
        pTask->unTimeOut = unTimeout;
        pTask->unPriority = 0;
        pTask->bSuspend = 0;
        pTask->bRepeat = 0;
    }
    
    
    void MTASK::Delay(FuncPtr_t taskFunc, uint16_t unTimeout, uint8_t unPriority)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask == nullptr) {pTask = CreateTask();}
        pTask->pTaskFunc = taskFunc;
        pTask->nTimeMatch = m_unSysTime + unTimeout;
        pTask->unTimeOut = unTimeout;
        pTask->unPriority = unPriority;
        pTask->bSuspend = 0;
        pTask->bRepeat = 0;
    }
    
    
    void MTASK::Repeat(FuncPtr_t taskFunc, uint16_t unTimeout)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask == nullptr) {pTask = CreateTask();}
        pTask->pTaskFunc = taskFunc;
        pTask->nTimeMatch = m_unSysTime + unTimeout;
        pTask->unTimeOut = unTimeout;
        pTask->unPriority = 0;
        pTask->bSuspend = 0;
        pTask->bRepeat = 1;
    }
    
    
    void MTASK::Repeat(FuncPtr_t taskFunc, uint16_t unTimeout, uint8_t unPriority)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask == nullptr) {pTask = CreateTask();}
        pTask->pTaskFunc = taskFunc;
        pTask->nTimeMatch = m_unSysTime + unTimeout;
        pTask->unTimeOut = unTimeout;
        pTask->unPriority = unPriority;
        pTask->bSuspend = 0;
        pTask->bRepeat = 1;
    }
    
    
    void MTASK::Stop(FuncPtr_t taskFunc)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask != nullptr) {DeleteTask(pTask);}
    }
    
    
    void MTASK::Suspend(FuncPtr_t taskFunc)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask != nullptr) {pTask->bSuspend = true;}
    }
    
    
    void MTASK::Resume(FuncPtr_t taskFunc)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask != nullptr) {pTask->bSuspend = false;}
    }
    
    
    void MTASK::SetPriority(FuncPtr_t taskFunc, uint8_t unPriority)
    {
        TASK_struct* pTask = FindTask(taskFunc);
        if (pTask != nullptr) {pTask->unPriority = unPriority;}
    }
    
    
    void MTASK::SetEvent(TASK_EVENT_TYPE_enum eEventType, FuncPtr_t vCallBack)
    {
        switch(eEventType)
        {
            case TASK_EVENT_TYPE_BeforeDeepSleep:   peventBeforeDeepSleep = vCallBack; break;
            case TASK_EVENT_TYPE_AfterWakeUp:       peventAfterWakeUp = vCallBack; break;
        }
    }

    
    void MTASK::Sleep()
    {
        #ifdef _SAMC21_
            REG_PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_IDLE0;
        #endif
        #ifdef _SAMD21_
            SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            REG_PM_SLEEP = PM_SLEEP_IDLE(0);
        #endif
        __WFI();
    }

    void MTASK::Sleep(uint16_t unTimeout)
    {
        uint32_t unSysTimeMatch = m_unSysTime + unTimeout;
        volatile uint32_t* unSysTimeActual = &m_unSysTime;

        /* Wait until system time matches */
        while(*unSysTimeActual < unSysTimeMatch)
        {
            #ifdef _SAMC21_
                REG_PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_IDLE0;
            #endif
            #ifdef _SAMD21_
                SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
                REG_PM_SLEEP = PM_SLEEP_IDLE(0);
            #endif
            __WFI();

            /* System time overflows */
            if (*unSysTimeActual == 0) {unSysTimeMatch &= 0x7FFFFFFF;}
        };
    }
}

/************************************************************************/
/* INTERRUPT HANDLER                                                    */
/************************************************************************/
void SysTick_Handler(void)
{
    Core::Multitask::MTASK::TickElapsed();
}


/************************************************************************/
/* MICRO TRACE BUFFER                                                   */
/************************************************************************/
#if (MICRO_TRACE_BUFFER == true)
#define IS_MTB_ENABLED  REG_MTB_MASTER & MTB_MASTER_EN
#define DISABLE_MTB     REG_MTB_MASTER = REG_MTB_MASTER & ~MTB_MASTER_EN
#define ENABLE_MTB      REG_MTB_MASTER = REG_MTB_MASTER | MTB_MASTER_EN

__attribute__((aligned(1024)))
volatile char __tracebuffer__[1024];
volatile int __tracebuffersize__ = sizeof(__tracebuffer__);

void InitTraceBuffer()
{
    int index = 0;
    uint32_t mtbEnabled = IS_MTB_ENABLED;
    DISABLE_MTB;
    for(index =0; index<1024; index++)
    {
        __tracebuffer__[index];
        __tracebuffersize__;
    }
    if(mtbEnabled)
    ENABLE_MTB;
}
#endif

/************************************************************************/
/* JUMP TO APPLICATION (only in Boot loader)                            */
/************************************************************************/
void Core::Multitask::BootJumpApp()
{
    typedef void (*AppPtr_t)() __attribute__((noreturn));

    __disable_irq();
    
    /* Set Stack pointer */
    __set_MSP(*(uint32_t*)APP_BASE_ADDR);
    
    /* Change vector table offset */
    SCB->VTOR = APP_BASE_ADDR;
    
    /* Jump to application */
    ((AppPtr_t)(APP_RESET_VECTOR_ADDR))();
}


/************************************************************************/
/* MAIN PROGRAM                                                         */
/************************************************************************/
int main(void)
{    
    /* Multitask initialization */
    Core::Multitask::MTASK::Init(CPU_FREQ, TICKS_PER_SECOND);
    Core::Multitask::MTASK::Delay(Core::Multitask::taskStartUpCore,TASK_TOUT_MS(STARTUP_TIMEOUT));
    
    /* Enable all interrupts */
    __enable_irq();
    
#if MICRO_TRACE_BUFFER == true
    /* Enable micro trace buffer */
    InitTraceBuffer();
#endif

    /* Main Loop */
    Core::Multitask::MTASK::Schedule();
}
