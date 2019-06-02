/*****************************************************************************
 * multitask.h
 *
 * Created: 31.3.2016 15:16:18
 * Revised: 22.5.2019
 * Author: uidm2956
 * BOARD: 
 * ABOUT:
 *
 *****************************************************************************/


#ifndef MULTITASK_H_
#define MULTITASK_H_

#include <sam.h>
#include <conf/conf_multitask.h>
#include <conf/conf_board.h>

/************************************************************************/
/* TICKS_PER_SECOND                                                     */
/* Set number of ticks per second                                       */
/************************************************************************/
#ifndef TICKS_PER_SECOND
    #define TICKS_PER_SECOND            1000
#endif

#define TASK_TOUT_MS(x)                   (uint16_t)(((uint32_t)x*TICKS_PER_SECOND)/1000)

/************************************************************************/
/* START UP TIMEOUT                                                     */
/* Function StartUp is always first running function. It can be delayed */
/* after start.                                                         */
/************************************************************************/
#ifndef STARTUP_TIMEOUT
    #define STARTUP_TIMEOUT             10
#endif

/************************************************************************/
/* BOOT LOADER                                                          */
/* If controller starts in boot loader, application start and reset     */
/* interrupt vector must be defined.                                    */
/************************************************************************/
#ifndef APP_BASE_ADDR
    #define APP_BASE_ADDR               0x2000
#endif

#define APP_RESET_VECTOR_ADDR           APP_BASE_ADDR + 4

/************************************************************************/
/* MICRO TRACE BUFFER                                                   */
/* For debugging purposes. See www.arm.com                              */
/************************************************************************/
#ifndef MICRO_TRACE_BUFFER
    #define MICRO_TRACE_BUFFER          false
#endif


namespace Core
{
    namespace Multitask
    {
        /************************************************************************/
        /* TASK STRUCTURE                                                       */
        /************************************************************************/
        typedef void (*FuncPtr_t)();
        
        struct TASK_struct {
            FuncPtr_t pTaskFunc;            /* Pointer to task function */
            int32_t nTimeMatch;             /* Time point when task is ready to run. -1 = task free to use */
            uint32_t unTimeOut:16;          /* Timeout for task. Used for repeated task */
            uint32_t unPriority:3;          /* Task priority (0 - lowest, 7 - highest) */
            uint32_t bSuspend:1;            /* Task is suspended */
            uint32_t bRepeat:1;             /* Task is repeated */
            TASK_struct *psParent;          /* Pointer to parent task */
        };
        
        
        /************************************************************************/
        /* ENUMS                                                                */
        /************************************************************************/
        enum TASK_EVENT_TYPE_enum {
            TASK_EVENT_TYPE_BeforeDeepSleep,
            TASK_EVENT_TYPE_AfterWakeUp,
            TASK_EVENT_TYPE_sum             /* Sum of all events. DO NOT DELETE!! */
        };
    
    
        /************************************************************************/
        /* DEFAULT FUNCTION PROTOTYPES                                          */
        /************************************************************************/
        /**
         * \brief   Prototype to start up function
         * 
         * 
         * \return void
         */
        void taskStartUpCore();
        
        /**
         * \brief   Prototype of function which jumps to application
         *          Use only in boot loader
         * 
         * 
         * \return void
         */
        void BootJumpApp();
        
        
        /************************************************************************/
        /* CLASS                                                                */
        /************************************************************************/
        class MTASK
        {
            private:
                static uint32_t m_unSysTime;                /* System time in ticks */
                static uint8_t m_unActiveTasks;             /* Number of active tasks */
                static uint8_t m_unHighestPrio;             /* Highest priority in a schedule loop */
                static bool m_bDeepSleepEnabled;            /* Deep sleep activation */
                static bool m_bSchedulerRunning;            /* Scheduler is running status */
                static TASK_struct* m_psLastTask;           /* Pointer to first task */
                static TASK_struct* m_psCurrentTask;        /* Pointer to current task to run */
                static FuncPtr_t peventBeforeDeepSleep;     /* Pointer to event before deep sleep */
                static FuncPtr_t peventAfterWakeUp;         /* Pointer to event after wake up */
                
                        
                /**
                 * \brief   Create new task
                 * 
                 * 
                 * \return TASK_struct*     - pointer to created task 
                 */
                static TASK_struct* CreateTask();
                
                
                /**
                 * \brief   Find task according task function
                 * 
                 * \param pTaskFunc         - task function
                 * 
                 * \return TASK_struct*     - pointer to existing task, if not found then nullptr
                 */
                static TASK_struct* FindTask(FuncPtr_t pTaskFunc);
                
                /**
                 * \brief   Delete existing task
                 * 
                 * 
                 * \return void
                 */
                static void DeleteTask(TASK_struct* pTask);
                
            public:
                /**
                * \brief   Multi task initialization. After that global interrupt must be enabled.
                *
                * \param unCpuFreq         - processor frequency
                * \param unTicksPerSecond  - ticks per second
                *
                * \return void
                */
                static inline void Init(uint32_t unCpuFreq, uint32_t unTicksPerSecond);
                
                
                /**
                * \brief   Tick has been elapsed. System time is incremented.
                *          Handle this function in a interrupt routine, which is periodically generated.
                *
                * \return void
                */
                static inline void TickElapsed();
                
                
                /**
                * \brief   Task Scheduler decides which task will run as next. Must be
                *          located in main loop.
                *
                * \return void
                */
                static inline void Schedule();
                
                
                /**
                * \brief   Run immediately task function in next scheduler loop (depends also on priority)
                *
                * \param taskFunc       - task function
                *
                * \return void
                */
                static void Run(FuncPtr_t taskFunc);
                
                
                /**
                 * \brief   Run immediately task function in next scheduler loop with priority
                 * 
                 * \param taskFunc      - task function
                 * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
                 * 
                 * \return void
                 */
                static void Run(FuncPtr_t taskFunc, uint8_t unPriority);
                
                        
                /**
                * \brief   Run task function after time out
                *
                * \param taskFunc       - task function
                * \param unTimeout      - time out in ticks
                *
                * \return void
                */
                static void Delay(FuncPtr_t taskFunc, uint16_t unTimeout);
                
                /**
                 * \brief   Run task function after time out with priority
                 * 
                 * \param taskFunc      - task function
                 * \param unTimeout     - time out in ticks
                 * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
                 * 
                 * \return void
                 */
                static void Delay(FuncPtr_t taskFunc, uint16_t unTimeout, uint8_t unPriority);
                
                
                /**
                * \brief   Repeat task function after time out
                *
                * \param taskFunc      - task function
                * \param unTimeout     - time out in ticks
                *
                * \return void
                */
                static void Repeat(FuncPtr_t taskFunc, uint16_t unTimeout);
                
                
                /**
                 * \brief   Repeat task function after time out with priority
                 * 
                 * \param taskFunc      - task function
                 * \param unTimeout     - time out in ticks
                 * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
                 * 
                 * \return void
                 */
                static void Repeat(FuncPtr_t taskFunc, uint16_t unTimeout, uint8_t unPriority);
                
                
                /**
                * \brief   Stop task function taskFunc
                *
                * \param taskFunc      - task function
                *
                * \return void
                */
                static void Stop(FuncPtr_t taskFunc);
                
                
                /**
                * \brief   Suspend task function taskFunc
                *
                * \param taskFunc      - task function
                *
                * \return void
                */
                static void Suspend(FuncPtr_t taskFunc);
                
                
                /**
                * \brief   Resume task function taskFunc
                *
                * \param taskFunc      - task function
                *
                * \return void
                */
                static void Resume(FuncPtr_t taskFunc);
                
                
                /**
                * \brief	Set priority to task function taskFunc
                *
                * \param taskFunc      - task function
                * \param unPriority    - priority (0 -> lowest,..., 7 -> highest)
                *
                * \return void
                */
                static void SetPriority(FuncPtr_t taskFunc, uint8_t unPriority);
                
                
                /**
                 * \brief   Get system time
                 * 
                 * 
                 * \return uint32_t     - system time
                 */
                static uint32_t unGetSysTime() {return m_unSysTime;}
                
                
                /**
                 * \brief   Assign function to a event type
                 * 
                 * \param eventType     - type of event
                 * \param vCallBack     - event function
                 * 
                 * \return void
                 */
                static void SetEvent(TASK_EVENT_TYPE_enum eEventType, FuncPtr_t vCallBack);
                
                
                /**
                 * \brief   Enable deep sleep
                 * 
                 * 
                 * \return void
                 */
                static void EnableDeepSleep() {m_bDeepSleepEnabled = true;}
                
                
                /**
                 * \brief   Disable deep sleep
                 * 
                 * 
                 * \return void
                 */
                static void DisableDeepSleep() {m_bDeepSleepEnabled = false;}


                /**
                 * \brief   Sleep until next interrupt
                 * 
                 * 
                 * \return void
                 */
                static void Sleep();


                /**
                 * \brief   Sleep during timeout. During this sleep no other task can be executed
                 * 
                 * \param unTimeout     - time out in ticks
                 * 
                 * \return void
                 */
                static void Sleep(uint16_t unTimeout);
        };
    }
}

#endif /* MULTITASK_H_ */
