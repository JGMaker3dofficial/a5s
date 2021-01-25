#include "app_gui.h"



static uint32_t tp_scan_next_update_millis = 0;
uint32_t display_next_update_millis = 0;
uint32_t display_CurTemp_millis = 0;//����ˢ��Ԥ�Ƚ���ĵ�ǰ�¶���ʾ������ʱ������Ϊ3s��ʱ�����õĹ��Σ��¶Ȳ�����

uint32_t flash_icon_update_millis = 0;
uint8_t redraw_screen_flag = IS_TRUE;//Һ������滭��־
uint8_t redraw_fileSystemUI_flag = IS_TRUE;//�ļ�ϵͳ�����ͼ��־

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

