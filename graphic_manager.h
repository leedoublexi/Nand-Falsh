#ifndef __GRAPHIC_MANAGER_H__
#define __GRAPHIC_MANAGER_H__

#include "cartridge_writer.h"

#include <DisplayMPI.h>
#include <FontMPI.h>

LF_USING_BRIO_NAMESPACE()

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
        int processing(CString process_str);
        int process_done(CString process_str, bool pass);
        int toggle_done(bool enable);

        //Brio Modules        
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

#endif
