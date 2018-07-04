#ifndef __CARTRIDGE_WRITER_H__
#define __CARTRIDGE_WRITER_H__

#include <ButtonMPI.h>
#include <PowerMPI.h>
#include <DebugMPI.h>

#include "stdlib.h"
#include "string.h"

LF_USING_BRIO_NAMESPACE()

enum STATE { MAIN_MENU , FLASHING , VERIFYING , FLASH_DONE , TOGGLING , VERIFYING_DONE , TOGGLE_DONE , EXIT };
enum MODE { MANUAL , AUTO };

#define kButtonFlashNand kButtonUp
#define kButtonVerifyNand kButtonDown
#define kButtonToggle kButtonLeft

//Marco for Graphic
#define LEFT 0
#define TOP 0
#define HEIGHT 800
#define WIDTH 1024
#define ALPHA 50
#define TEXT_COLOR 0xFFFFFFFF //White
#define BACKGROUNG_COLOR 0xFF000000 //Black
#define FONT_SIZE 30

#endif //__CARTRIDGE_WRITER_H__
