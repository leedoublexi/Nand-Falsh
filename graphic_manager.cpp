#include "graphic_manager.h"
#include <DebugMPI.h>

extern CDebugMPI debug;

graphic_manager::graphic_manager():
    cartidge_present(true)
{
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

int graphic_manager::processing(CString process_str)
{
    debug.DebugOut(kDbgLvlVerbose,"%s(): %s\n", __PRETTY_FUNCTION__, process_str.c_str());
    CString lines[] = {
        "Cartridge Writer",
        "processing",
        process_str,
    };
    int line_num = (sizeof(lines)/sizeof(CString));
    
    clear_display();
    font.SetFontColor(TEXT_COLOR);

    for (int i = 0; i < line_num; ++i)
        font.DrawString(&lines[i], 0, fontMetrics.height*i, &fontSurf);

}

int graphic_manager::process_done(CString process_str, bool pass)
{
    debug.DebugOut(kDbgLvlVerbose,"%s(): %s: %d\n", __PRETTY_FUNCTION__, process_str.c_str(), pass);
    CString lines[] = {
        "Cartridge Writer",
        "process done",
        process_str,
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

