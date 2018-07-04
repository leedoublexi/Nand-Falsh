#ifndef __APP_CONTROLLER_H__
#define __APP_CONTROLLER_H__

#include <KernelMPI.h>

#include "nand_flash_manager.h"
#include "graphic_manager.h"

LF_USING_BRIO_NAMESPACE()

const tEventType kMyHandledTypes[] = { kAllButtonEvents, kAllPowerEvents };

class app_controller: IEventListener
{
    public:
        app_controller();
        ~app_controller();
        
        virtual tEventStatus Notify( const IEventMessage &msg );
        bool is_running(){ return running; };

        void* worker();

    private:
        inline void change_state(STATE new_state){ state = new_state; };
        inline void toggle_mode(){ mode=(mode==AUTO)?MANUAL:AUTO; };

        void process_button(U32 button);

        int toggle_bootup();
        void exit_app(){};
        void power_off();

        //Brio Modules
        CKernelMPI kernel;
        CEventMPI event;

        //Managers
        graphic_manager graphic_mgr;
        nand_flash_manager nand_flash_mgr;

        //Variables
        STATE state;
        MODE mode;

        //child task
        bool running;
        tTaskHndl taskHndl;
        tTaskProperties taskProp;
};

#endif
