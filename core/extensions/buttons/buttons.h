/*
 * buttons.h
 *
 * Created: 1.5.2019 17:27:39
 * Revised: 
 * Author: LeXa
 * BOARD:
 * ABOUT: 
 *
 *****************************************************************************/


#ifndef BUTTONS_H_
#define BUTTONS_H_

namespace Core
{
    namespace Extensions
    {
        enum BUTTONS_EVENT_enum {
            BUTTONS_EVENT_PushEnter,
            BUTTONS_EVENT_PushNext,
        };
        
        class BUTTONS
        {
            private:
                static void* m_pEventFunction[2];
                
            public:
                static void SetEvent(BUTTONS_EVENT_enum eButtonEvent, void* peventFunction)
                {
                    m_pEventFunction[eButtonEvent] = peventFunction;
                }
                
                static void* GetEvent(BUTTONS_EVENT_enum eButtonEvent)
                {
                    return m_pEventFunction[eButtonEvent];
                }
        };
    }
}



#endif /* BUTTONS_H_ */