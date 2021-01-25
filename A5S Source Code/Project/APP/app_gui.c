#include "app_gui.h"



static uint32_t tp_scan_next_update_millis = 0;
uint32_t display_next_update_millis = 0;
uint32_t display_CurTemp_millis = 0;//用于刷新预热界面的当前温度显示，周期时间设置为3s，时间设置的过段，温度波动大

uint32_t flash_icon_update_millis = 0;
uint8_t redraw_screen_flag = IS_TRUE;//液晶界面绘画标志
uint8_t redraw_fileSystemUI_flag = IS_TRUE;//文件系统界面绘图标志

menuFunc_t currentMenu = main_screen;
menuFunc_t lastMenu;

uint8_t windows_flag = IS_FALSE;


void lcd_update(void)
{
    if (tp_scan_next_update_millis <= millis())
    {
        tp_dev.scan(0);
        in_obj.get_key(&tp_dev, IN_TYPE_TOUCH);
        tp_scan_next_update_millis = millis() + 10;
        (*currentMenu)();
    }
}

