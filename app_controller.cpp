#include "app_controller.h"
#include <DebugMPI.h>
    
extern CDebugMPI debug;

void* workerThread(void* arg)
{
    debug.DebugOut(kDbgLvlImportant, "workerThread(%p)\n", arg);
	app_controller* p = (app_controller*)arg;
	return p->worker();
}

app_controller::app_controller():
    IEventListener(kMyHandledTypes, ArrayCount(kMyHandledTypes)),
    state(MAIN_MENU),
    mode(MANUAL),
    running(true),
    taskHndl(0)
{
    graphic_mgr.update(state, 0);
    taskProp.TaskMainFcn = (void*(*)(void*))workerThread;
    taskProp.taskMainArgCount = 1;
    taskProp.pTaskMainArgValues = this;
    kernel.CreateTask(taskHndl, taskProp, NULL);
}

app_controller::~app_controller()
{
    event.UnregisterEventListener(this);
    tPtr ptr;
    running = false;
    if (taskHndl)
        kernel.JoinTask(taskHndl, ptr);
    taskHndl = 0;
}

tEventStatus app_controller::Notify( const IEventMessage &msg )
{
    static bool poweroff_requested = false;
    tEventType type = msg.GetEventType();
    debug.DebugOut(kDbgLvlVerbose,"%s()\n", __PRETTY_FUNCTION__);

    if (poweroff_requested)
        power_off();

    // Handle poweroff event
    if (type == kPowerStateChanged) {
        const CPowerMessage& pwrmsg = reinterpret_cast<const CPowerMessage&>(msg);
        tPowerState powerstate = pwrmsg.GetPowerState();
        if (kPowerShutdown == powerstate) {    // power down event
            debug.DebugOut(kDbgLvlVerbose, "\n");
            debug.DebugOut(kDbgLvlVerbose, "PowerDown event received. \n");
            debug.DebugOut(kDbgLvlVerbose, "\n");
            if (state != FLASHING)
                power_off();
            debug.DebugOut(kDbgLvlVerbose, "Wait until flashing finish. \n");
            poweroff_requested = true;
        }
    }
    else
    {
        U32 button = 0;
        const CButtonMessage& btnmsg = reinterpret_cast<const CButtonMessage&>(msg);
        tButtonData button_data = btnmsg.GetButtonState();
        if ((button_data.buttonState & button_data.buttonTransition) != 0) {
            button = (button_data.buttonState &  (~(kHeadphoneJackDetect | kCartridgeDetect)));
            process_button(button);
        }
    }
    return kEventStatusOKConsumed;
}

void* app_controller::worker()
{
    STATE runningState = MAIN_MENU;
    int ret = 0;

    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);
    event.RegisterEventListener(this);
    while(is_running())
    {
        if (runningState == state)
        {
            kernel.TaskSleep(1000);
            continue;
        }
        if (state == FLASHING)
        {
            graphic_mgr.update(state, 0);
            ret = nand_flash_mgr.flash();
            state = FLASH_DONE;
        }
        if (state == VERIFYING)
        {
            graphic_mgr.update(state, 0);
            ret = nand_flash_mgr.verify();
            state = VERIFYING_DONE;
        }
        if (state == TOGGLING)
        {
            ret = toggle_bootup();
            state = TOGGLE_DONE;
        }
        graphic_mgr.update(state, ret);
        runningState = state;
    }
    debug.DebugOut(kDbgLvlVerbose,"%s OUT\n", __PRETTY_FUNCTION__);
    return 0;
}

int app_controller::toggle_bootup()
{
    return 1;
}

void app_controller::power_off()
{
    CPowerMPI power;
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);
    power.Shutdown();
    return;
}

void app_controller::process_button(U32 button)
{
    int is_state_changed = 0;
    switch (state)
    {
        case MAIN_MENU:
            if (button == kButtonFlashNand) {
                change_state(FLASHING);
            }
            else if (button == kButtonVerifyNand) {
                change_state(VERIFYING);
            }
            else if (button == kButtonToggle) {
                change_state(TOGGLING);
            }
            break;
        case FLASHING:
        case VERIFYING:
        case TOGGLING:
            //Not do anything when working
            debug.DebugOut(kDbgLvlVerbose,"%s Working, no respone to button\n", __PRETTY_FUNCTION__);
            break;
        case FLASH_DONE:
        case VERIFYING_DONE:
        case TOGGLE_DONE:
            debug.DebugOut(kDbgLvlVerbose,"%s Going back to main menu\n", __PRETTY_FUNCTION__);
            change_state(MAIN_MENU);
            break;
        case EXIT:
            debug.DebugOut(kDbgLvlVerbose,"%s Exiting, no respone to button\n", __PRETTY_FUNCTION__);
            break;
        default:
            debug.DebugOut(kDbgLvlVerbose,"%s What's going wrong?\n", __PRETTY_FUNCTION__);
            break;

    }
    return;
}

