/******************************************************************************

 @Description  A very simple app that to flashing nand flash cartidge

******************************************************************************/
#include "nand_card_flasher.h"

const tDebugSignature kMyApp = kFirstCartridge1DebugSig;
const tEventType kMyHandledTypes[] = { kAllButtonEvents, kAllPowerEvents };

/*----------------------------------------------------------------------------------------
 App entry point.
  command line param changes from old format to std Linux format
    PARAM                  MEANING
----------------------------------------------------------------------------------------*/

int main(int argc, char **argv)
{
	CDebugMPI debug(kMyApp);
    debug.SetDebugLevel(kDbgLvlVerbose);
	debug.DebugOut(kDbgLvlVerbose, "Welcome to Nand Cartidge Flashing Utility\n");
	debug.DebugOut(kDbgLvlVerbose, "    I'm launched from %s\n", *argv);

    //Main Loop
    app_controller main_controller;
    while (main_controller.is_running()) {
        sleep(1);
    }
    //
	debug.DebugOut(kDbgLvlVerbose, "Farewell my friend\n");
    exit(0);
}

//
// app_controller
//
void* workerThread(void* arg)
{
    CDebugMPI debug(kMyApp);
    debug.DebugOut(kDbgLvlImportant, "workerThread(%p)\n", arg);
	app_controller* p = (app_controller*)arg;
	return p->worker();
}

app_controller::app_controller():
    IEventListener(kMyHandledTypes, ArrayCount(kMyHandledTypes)),
    debug(kMyApp),
    state(MAIN_MENU),
    mode(MANUAL),
    running(true),
    taskHndl(0)
{
    debug.SetDebugLevel(kDbgLvlVerbose);
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

//
// graphic_manager
//
graphic_manager::graphic_manager():
    debug(kMyApp),
    cartidge_present(true)
{
    debug.SetDebugLevel(kDbgLvlVerbose);
    init();
}

graphic_manager::~graphic_manager()
{
}

void graphic_manager::init()
{
	U8* buffer;
    int pitch;

    //TODO We assume this app only running on Santiago, have to set different display size for different platform

    // Create Display
    layer = display.CreateHandle(HEIGHT, WIDTH, kPixelFormatARGB8888, NULL);
    debug.DebugOut(kDbgLvlVerbose, "displaymgr.CreateHandle return: %p\n", (void*)layer);
    display.Register(layer, LEFT, TOP, kDisplayOnTop, 0);
    
    // Create Display Surface to draw on
	buffer = display.GetBuffer(layer);

	// Need to know pitch for font surface descriptor
	pitch = display.GetPitch(layer);

	// Set alpha channel for display surface transparency
	display.SetAlpha(layer, ALPHA, true);

	// Render font text strings into draw surface buffer
	fontSurf.width = WIDTH;
	fontSurf.height = HEIGHT;
	fontSurf.pitch = pitch;
	fontSurf.buffer = buffer;
	fontSurf.format = kPixelFormatARGB8888;

	// Set font properties and drawing attributes
	font.SetFontResourcePath("/usr/lib/fonts/");
	font.LoadFont("Vera.ttf", FONT_SIZE);

	font.SetFontColor(TEXT_COLOR);
	font.GetFontMetrics(&fontMetrics);
    clear_display();
}

void graphic_manager::clear_display()
{
    for (int i = 0; i < fontSurf.height; i++)
	{
		for (int j = 0, m = i*fontSurf.pitch; j < fontSurf.width; j++, m+=4)
		{
			U32 *buf32 = reinterpret_cast<U32*>(&fontSurf.buffer[m]);
			*buf32 = BACKGROUNG_COLOR;
		}
	}
    display.Invalidate(0, NULL);
}

int graphic_manager::update(STATE state, int argument)
{
    debug.DebugOut(kDbgLvlVerbose,"%s() IN\n", __PRETTY_FUNCTION__);
    switch (state)
    {
        case MAIN_MENU:
            main_menu();
            break;
        case FLASHING:
            processing("Cartrige Flashing");
            break;
        case VERIFYING:
            processing("Cartrige Verifying");
            break;
        case FLASH_DONE:
            process_done("Cartrige Flashing Finish", argument);
            break;
        case VERIFYING_DONE:
            process_done("Cartrige Verifying Finish", argument);
            break;
        case TOGGLE_DONE:
            toggle_done(argument);
            break;
        default:
            debug.DebugOut(kDbgLvlVerbose,"%s What's going wrong?\n", __PRETTY_FUNCTION__);
            break;
    }
}

int graphic_manager::main_menu()
{
    debug.DebugOut(kDbgLvlVerbose,"%s()\n", __PRETTY_FUNCTION__);
    CString lines[] = {
        "Cartridge Writer",
        "Testing Menu, Testing 123",
        "Up for Flashing Nand",
        "Down for Verifying Nand",
        "Left for enabling/disabling this app when bootup",
        "Right for Nothing",
    };
    int line_num = (sizeof(lines)/sizeof(CString));
    
    clear_display();
    font.SetFontColor(TEXT_COLOR);

    for (int i = 0; i < line_num; ++i)
        font.DrawString(&lines[i], 0, fontMetrics.height*i, &fontSurf);
}

int graphic_manager::processing(string process_str)
{
    debug.DebugOut(kDbgLvlVerbose,"%s(): %s\n", __PRETTY_FUNCTION__, process_str.c_str());
    CString lines[] = {
        "Cartridge Writer",
        "processing",
        process_str.c_str(),
    };
    int line_num = (sizeof(lines)/sizeof(CString));
    
    clear_display();
    font.SetFontColor(TEXT_COLOR);

    for (int i = 0; i < line_num; ++i)
        font.DrawString(&lines[i], 0, fontMetrics.height*i, &fontSurf);

}

int graphic_manager::process_done(string process_str, bool pass)
{
    debug.DebugOut(kDbgLvlVerbose,"%s(): %s: %d\n", __PRETTY_FUNCTION__, process_str.c_str(), pass);
    CString lines[] = {
        "Cartridge Writer",
        "process done",
        process_str.c_str(),
        pass?"Pass":"Fail",
    };
    int line_num = (sizeof(lines)/sizeof(CString));

    clear_display();
    font.SetFontColor(TEXT_COLOR);

    for (int i = 0; i < line_num; ++i)
        font.DrawString(&lines[i], 0, fontMetrics.height*i, &fontSurf);
}

int graphic_manager::toggle_done(bool enable)
{
    debug.DebugOut(kDbgLvlVerbose,"%s(): %d\n", __PRETTY_FUNCTION__, enable);
}

//
// nand_flash_manager
//
nand_flash_manager::nand_flash_manager():
    debug(kMyApp)
{
    debug.SetDebugLevel(kDbgLvlVerbose);
}

nand_flash_manager::~nand_flash_manager()
{
}

int nand_flash_manager::flash()
{
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);    
    static int ret = 0;
    kernel.TaskSleep(10000);
    ret = 1 - ret;
    return ret;
}

int nand_flash_manager::verify()
{
    debug.DebugOut(kDbgLvlVerbose,"%s IN\n", __PRETTY_FUNCTION__);    
    static int ret = 0;
    kernel.TaskSleep(10000);
    ret = 1 - ret;
    return ret;
}
