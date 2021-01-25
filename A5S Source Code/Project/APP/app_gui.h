#ifndef _APP_GUI_H
#define _APP_GUI_H



/*******************外设头文件**************************/
#include "bsp.h"
#include "common.h"



/*******************GUI界面头文件************************/





#include "mainui.h"
#include "print.h"

#include "preheat.h"
#include "move.h"
#include "powerOffPrint.h"
#include "home.h"
#include "welcome.h"
#include "extruder.h"
#include "level.h"
#include "fan.h"
#include "fila.h"
#include "set.h"
#include "about.h"
#include "uiFont.h"
#include "uiPicture.h"
#include "uiCalibrate.h"
#include "uiLanguage.h"
#include "speed.h"
#include "uiComplmt.h"
#include "more.h"
typedef void (*menuFunc_t)();



extern menuFunc_t currentMenu;
extern uint32_t display_next_update_millis;
extern uint32_t display_CurTemp_millis;

extern uint8_t redraw_screen_flag;
extern uint8_t windows_flag;
extern uint32_t flash_icon_update_millis;
extern uint8_t redraw_fileSystemUI_flag;
void lcd_update(void);


#endif
