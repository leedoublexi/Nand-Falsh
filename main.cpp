/******************************************************************************

 @Description  A very simple app that to flashing nand flash cartidge

******************************************************************************/
#include "cartridge_writer.h"
#include "app_controller.h"

/*----------------------------------------------------------------------------------------
 App entry point.
  command line param changes from old format to std Linux format
    PARAM                  MEANING
----------------------------------------------------------------------------------------*/
CDebugMPI debug(kFirstCartridge1DebugSig);

int main(int argc, char **argv)
{
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

