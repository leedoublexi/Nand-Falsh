#ifndef NAND_CARD_FLASHER_H_
#define NAND_CARD_FLASHER_H_

#include <DebugMPI.h>
#include <KernelMPI.h>
#include <ButtonMPI.h>
#include <PowerMPI.h>
#include <DisplayMPI.h>
#include <FontMPI.h>

#include "stdlib.h"
#include "string.h"

#define LEFT 0
#define TOP 0
#define HEIGHT 800
#define WIDTH 1024
#define ALPHA 50
#define TEXT_COLOR 0xFFFFFFFF //White
#define BACKGROUNG_COLOR 0xFF000000 //Black
#define FONT_SIZE 30

using namespace std;
LF_USING_BRIO_NAMESPACE()

enum STATE { MAIN_MENU , FLASHING , VERIFYING , FLASH_DONE , TOGGLING , VERIFYING_DONE , TOGGLE_DONE , EXIT };
enum MODE { MANUAL , AUTO };

const U32 kButtonFlashNand = kButtonUp;
const U32 kButtonVerifyNand = kButtonDown;
const U32 kButtonToggle = kButtonLeft;

class graphic_manager
{
    public:
        graphic_manager();
        ~graphic_manager();

        int update(STATE state, int argument);
        void updateCartidge(bool plugged){ cartidge_present = plugged; };

    private:
        void init();
        void clear_display();

        int main_menu();
        int processing(string process_str);
        int process_done(string process_str, bool pass);
        int toggle_done(bool enable);

        //Brio Modules        
        CDebugMPI debug;
        CDisplayMPI display;
        CFontMPI font;

        //Display
        tDisplayScreen screen;
        tDisplayHandle layer;

        //Font
        tFontSurf fontSurf;
        tFontMetrics fontMetrics;
        tFontProp fontProp;
        tFontAttr fontAttr;

        //Variables        
        bool cartidge_present;
};

class nand_flash_manager
{
    public:
        nand_flash_manager();
        ~nand_flash_manager();
        int flash();
        int verify();

    private:
        //Brio Modules        
        CDebugMPI debug;
        CKernelMPI kernel;
};

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
        CDebugMPI debug;
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

#endif //NAND_CARD_FLASHER_H_
