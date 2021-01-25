

#include "powerOffPrint.h"



#include "gcodeplayer.h"

#include "temperature.h"
#include "Dlion.h"
#include "planner.h"
#include "stepper.h"
/**********************���Ժ궨��**************************/

#define POP_DEBUG_EN

#ifdef POP_DEBUG_EN

#define POP_DEBUG	DBG

#else
#define POP_DEBUG(format,...)	((void) 0)

#endif
/************************************************/

#define POP_BG_COLOR 				GRAY//����ֹͣ�������ɫ

#define POP_SET_COLOR					GRAY//���ý��汳����ɫ
#define POP_SET_FONT_COLOR			  BLUE//��ӡ���ð�ť������ɫ


#define POP_SURE_BUTTON_ID		0
#define POP_CANCLE_BUTTON_ID	1
#define POP_BACK_BUTTON_ID 		2


#define TEMP_HYSTEREsis			5


char *const CONFIRM_CAPTION_BACK = "BACK";
static char *const power_off_remind[GUI_LANGUAGE_NUM] =
{
"�Ƿ�ָ��ϴδ�ӡ",
"Resume Print of last time ?"
};
char *const power_off_ui_head [GUI_LANGUAGE_NUM]= 
{
	"׼��/��ӡ/�ϵ�����",
	"Prepare/print/power down print"
};

u32 powerDownPrintTime;//����ϵ�ʱ�Ĵ�ӡʱ��

u8 cancelPDP = FALSE;//ȡ���ϵ�����
u8 pdPrint= FALSE;//ѡ��ϵ�����ȷ��

u32 pdPrintHomeTime;//�ϵ�����׼�����������У�ȷ���Ȼص�ԭ��������Z��

_pdp_prepare_state_e pdPrintST = PDP_STATE0;//�ϵ�����׼������״̬��־


typedef enum
{
	PDC_STATE0 = 0,//������ͷ
	PDC_STATE1 = 1,//Z������һ�Σ�X���ԭ��
	PDC_STATE2 = 2,
	PDC_STATE3 = 3,
}_pd_cancel_state_e;
_pd_cancel_state_e pdcST = PDC_STATE0;

void powerOffPrint_screen(void)
{
    uint8_t num = 2;//������
    uint8_t i;
    uint8_t selx;

    uint8_t free_flag = 0;
	uint8_t font = 16;
	uint16_t xStart = 2;
	uint16_t yStart = 5;

	uint8_t *fileName = NULL;
	uint8_t *dispFileName = NULL;
	uint8_t length;
	uint8_t setLength;

	uint16_t width;
    if( redraw_screen_flag == IS_TRUE )
    {		
    redraw_screen_flag = IS_FALSE;
	BACK_COLOR = BLACK;
    POINT_COLOR = WHITE;
	width = lcddev.width;
	LCD_Clear_DMA(BLACK);//����
	//��ʾ������
	Show_Str(xStart,yStart,lcddev.width,font,(u8 *)power_off_ui_head[LANGUAGE],font,1);//��ʾ��������Ϣ
	
	Show_Str(xStart,yStart + font,lcddev.width,font,(u8 *)power_off_remind[LANGUAGE],font,1);//��ʾ�ϵ�������ʾ��Ϣ
		
	//��ȡ�ϵ籣����ļ�������Ŀ¼
	fileName = get_current_root_file_name(printFileName);//���gcode�ļ���
	//��ʾ��ӡ�ļ���	

	xStart = 10;
	yStart += 3*font;
	setLength = 30;//������2������������ʾ����Ҫ2���ֽ�

	length = strlen((char *)fileName);
	if(length >= setLength)//�Ƿ�ֶ���ʾgcode�ļ���
	{
		dispFileName = (u8*)gui_memin_malloc(setLength + 1);	//�����ڴ�
		if(dispFileName == NULL)
		{	
			POP_DEBUG("file:%s,line:%d -> memory error\r\n",__FILE__,__LINE__);
			
		}
		else
		{
			
			strncpy((char *)dispFileName,(const char *)fileName,setLength);
			dispFileName[setLength] = '\0';
			
			Show_Str(xStart,yStart,width,font,(u8*)dispFileName, font, 1); 
			gui_memin_free(dispFileName);//�ͷ��ڴ�
			
			xStart = 2;
			dispFileName = fileName + setLength;
			Show_Str(xStart,yStart + font, width,font,(u8*)dispFileName, font, 1); 
		}						
	}
	else 
	Show_Str(xStart ,yStart,width,font,(u8*)fileName, font, 1); //��ʾgcode�ļ�

	free_current_root_file_name_memory(fileName);//�ͷ��ڴ�

        screen_key_group = (_btn_obj **)gui_memin_malloc(sizeof(_btn_obj *)*num);
        if(screen_key_group)//���������ɹ�
        {

            uint16_t botton_width = 80;
            uint16_t botton_height = 40;

			xStart = 40;
			yStart = 100 + font;
			
	
            for( i = 0; i < num; i++)//����2������
            {
                screen_key_group[i] = btn_creat(xStart, yStart, botton_width, botton_height, i, 4);
				 xStart += 160;		
            }
			
            for( i = 0; i < num; i++)
            {
                screen_key_group[i]->bcfucolor = BLACK;
                screen_key_group[i]->bcfdcolor = WHITE;

                screen_key_group[i]->bkctbl[0] = CYAN;
                screen_key_group[i]->bkctbl[1] = RED;
                screen_key_group[i]->bkctbl[2] = CYAN;
                screen_key_group[i]->bkctbl[3] = RED;

                screen_key_group[i]->caption = (u8 *)CONFIRM_CAPTION_TBL[LANGUAGE][i];
            }
			

			for(i = 0; i < num; i++)
			{
				btn_draw(screen_key_group[i]);
			}				
        }
        else
        {
            POP_DEBUG("file %s, function %s memory error!\r\n", __FILE__, __FUNCTION__);
            return;
        }

    }



/////////////////////////////	

//�������
    selx = screen_key_chk(screen_key_group, &in_obj, num);
    if(selx & BUTTON_ACTION)
    {
		
        switch(selx & BUTTON_SELECTION_MASK)
        {
		
        case YES://ȷ����ӡ��
            currentMenu = start_print_screen;
            free_flag = 1;
			prepare_power_down_print();
			pdPrint =TRUE;
            POP_DEBUG("YES\r\n");
            break;
        case NO://ȡ��--		
            free_flag = 1;
			 currentMenu = main_screen;
			cancel_power_down_print();
			cancelPDP = TRUE;
            POP_DEBUG("NO\r\n");
            break;
			
        default:
            break;
        }
		
    }
	
	
    if(free_flag == 1) //�ͷ��ڴ�
    {
        redraw_screen_flag = IS_TRUE;
        for(i = 0; i < num; i++)
        {
            btn_delete(screen_key_group[i]);
        }
        gui_memin_free(screen_key_group);

    }


}



void prepare_power_down_print(void)
{
	
		//all axis home
		char cmd[30];
		
	//	power_down_print_init();
		//heat extru1,bed
		sprintf(cmd,"M104 S%.1f",bck_extruder_temperature );//���Ӽ���2���¶�
		 enquecommand(cmd);
		 
		sprintf(cmd,"M140 S%.1f",bck_bed_temperature);
		enquecommand(cmd);	 

}

void cancel_power_down_print(void)
{

	force_sdCard_pause();//sdCard��������ֹͣ
	card_closefile();
	file_pos = 0;
	fil_size = 0;
	powerDownPrintTime = 0;
	line_no = 0;

   eeprom_clear_power_off_status();
  
}

//�ϵ������ʼ������ȡ�ϵ籣�����ز���
void power_down_print_init(void)
{
	u16 fileNameLength;
	u8 selx = 1;
	u8 cnt = 0;
	u8 i;
	u8 fanVal;
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN)
	{

		eeprom_get_file_name_length(&fileNameLength);//����ļ�������
		printFileName = mymalloc(SRAMIN,fileNameLength);
		if(printFileName)
		{
			DBG("# power off happen\r\n");
			DBG("# power off information follow...\r\n");
			eeprom_get_file_name((u8 *)printFileName , &fileNameLength);//����ļ���
			eeprom_get_temp(&bck_bed_temperature , &bck_extruder_temperature);//����¶�
			eeprom_get_print_time(&powerDownPrintTime);
			eeprom_get_print_percentage(&powerDownLineNum , &bckGcodeHeadPos);//��ȡ�к�,�ļ�ָ��λ��
			eeprom_get_axis_position(bck_position);
			eeprom_get_print_fan(&fanVal);
			eeprom_get_not_exacute_gcode_num(&dontExctpPdLineNum);
			fanSpeed = fanVal;
			bck_bed_temperature +=1.0;
			bck_extruder_temperature += 1.0;
		}
		card_checkautostart();//��ʼ��SD��
		while(selx)
		{
			selx = f_open(&card.fgcode, (const TCHAR*)printFileName, FA_READ);
			cnt++;
		
			if(selx != FR_OK && cnt >=5 )
			{
				POP_DEBUG("-> @print file lost \r\n");
				isPrintFileExist = FALSE;//�����ڴ�ӡ�ļ�
				break;
			}
			
		}
		if(selx == FR_OK)
		{
			fil_size = f_size(&card.fgcode);//��¼�ļ���С
			isPrintFileExist = TRUE;
			
			reach_power_down_file_pos(dontExctpPdLineNum,bckGcodeHeadPos,&file_pos);
			line_no = powerDownLineNum;			
		}
		else 
			{
			
			isPrintFileExist = FALSE;
			fil_size = 0x0000ffff;//��ֹ�ϵ�����ʼʱSD��û�в��룬���´�ӡ������ַ��ذ���
			}
		
			for(i=0 ; i< NUM_AXIS ; i++)
			DBG("# bckup_pos[%d]:%.3f\r\n",i,bck_position[i]);
				
			POP_DEBUG("pd file:%s,length:%d\r\n",printFileName,fileNameLength);
			POP_DEBUG("-> bed Temp:%.3f , extru Temp:%.3f\r\n",bck_bed_temperature,bck_extruder_temperature);
			POP_DEBUG("pd time:%d\r\n",powerDownPrintTime);
			
			POP_DEBUG("#pd line_no:%d,file_pos:%d\r\n",powerDownLineNum,bckGcodeHeadPos);
			POP_DEBUG("#pd dont exacute gcode num:%d\r\n",dontExctpPdLineNum);
	}
}
//ȷ��ִ����ϵ������׼�������󣬲���ϵ��־
void PDPState0(void)
{
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN && pdPrint == TRUE)
	{
		u8 zUpFlag;
		if(degHotend0() >= bck_extruder_temperature - TEP_OFFSET)//����ͷ�¶ȼ��ȵ�Ŀ���¶Ⱥ�
		{
		st_synchronize();
		
		
		//����Z�ᵱǰλ��Ϊ�ϵ��λ��
		feedrate = homing_feedrate[Z_AXIS];
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[E_AXIS] = current_position[E_AXIS];
		current_position[Z_AXIS] = bck_position[Z_AXIS] ;//����Z�ᵱǰλ��Ϊ�ϵ�ʱ��λ��	
		destination[Z_AXIS] = current_position[Z_AXIS];
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		st_synchronize();

		zUpFlag = eeprom_get_z_up_flag();
		if(zUpFlag != POWER_OFF_Z_UP_FLAG_SET)
		{
//Z��������һ�Σ�����ڴ˹����ж�ζϵ磬��Z��ֻ����һ��
		feedrate = 40 * 60;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS] + PD_PRINT_Z_UP_LENGTH;
		destination[E_AXIS] = current_position[E_AXIS];		
		
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[Z_AXIS] = destination[Z_AXIS];
		st_synchronize();
		eeprom_set_z_up_flag();//��־Z������
		}
		pdPrintST = PDP_STATE1;
		POP_DEBUG("pd print state0\r\n");	
	  }
		
	}
}

void PDPState1(void) 
{
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN && pdPrint == TRUE)
	{
		if(degHotend0() >= bck_extruder_temperature -TEP_OFFSET)//����ͷ�¶ȼ��ȵ�Ŀ���¶Ⱥ�
		{
			st_synchronize();
			//X,Y,Z��ش�ӡԭ��
			user_cmd_move_xHome();
			user_cmd_move_yHome();
			POP_DEBUG("pd print state1\r\n");	
			pdPrintST = PDP_STATE3;

			pdPrintHomeTime = millis();
		}
	}
}

void PDPState2(void) 
{
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN && pdPrint == TRUE)
	{
		if(degHotend0() >= bck_extruder_temperature)//����ͷ�¶ȼ��ȵ�Ŀ���¶Ⱥ�
		{	
			
			if(millis() - pdPrintHomeTime < 8000)	return;//ȷ����ԭ������ѹ��Planner��������
			
			st_synchronize();//ȷ��ǰһ�׶�����ִ�����
			

			
			
			//Z������һ��
			
			feedrate = homing_feedrate[Z_AXIS];
			destination[X_AXIS] = current_position[X_AXIS];
			destination[Y_AXIS] = current_position[Y_AXIS];
			destination[Z_AXIS] = current_position[Z_AXIS] +  PD_PRINT_Z_UP_LENGTH;
			destination[E_AXIS] = current_position[E_AXIS]; 	
			
			plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
			plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
			current_position[Z_AXIS] = destination[Z_AXIS];
			st_synchronize();
			


			
			POP_DEBUG("pd print state2\r\n");	
			pdPrintST = PDP_STATE3;
		}
	}
}

void PDPState3(void) 
{
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN && pdPrint == TRUE)
	{
		if(degHotend0() >= bck_extruder_temperature -TEP_OFFSET)//����ͷ�¶ȼ��ȵ�Ŀ���¶Ⱥ�
		{
		if(millis() - pdPrintHomeTime < 8000)	return;//ȷ����ԭ������ѹ��Planner��������	
		
		st_synchronize();//ȷ��ǰһ�׶�����ִ�����
//E���Ƚ���һ��		
		feedrate = 30 * 60;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		current_position[E_AXIS] = 0.0f;
		destination[E_AXIS] = current_position[E_AXIS] + POWER_DOWN_PRINT_FAST_FILA_IN_LEN;
		
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] = destination[E_AXIS];
		st_synchronize();
	
	
		feedrate = 20 * 60;
		destination[X_AXIS] = current_position[X_AXIS];
		destination[Y_AXIS] = current_position[Y_AXIS];
		destination[Z_AXIS] = current_position[Z_AXIS];
		
		destination[E_AXIS] = current_position[E_AXIS] + POWER_DOWN_PRINT_SLOW_FILA_IN_LEN;
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		current_position[E_AXIS] = destination[E_AXIS];
		st_synchronize();
		
		//�ָ��ϵ�ʱE���λ��
		current_position[E_AXIS] = bck_position[E_AXIS] ;//����E�ᵱǰλ��Ϊ�ϵ�ʱ��λ��	
		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);//�ָ�������ǰe��λ��

	
			
		POP_DEBUG("pd print state3\r\n");	
		pdPrintST = PDP_STATE4;
		}
	}
}
void PDPState4(void) 
{
	if(eeprom_is_power_off_happen() == PW_OFF_HAPPEN && pdPrint == TRUE)
	{
		if((degHotend0() >= bck_extruder_temperature - TEP_OFFSET) && isPrintFileExist ==TRUE)//����ͷ�¶ȼ��ȵ�Ŀ���¶Ⱥ�
		{	
			u8 i;
			u8 zUpFlag;
		
			 pdPrint = FALSE;	
		st_synchronize();
		for (i = 0; i < NUM_AXIS; i++)	destination[i] = bck_position[i];//�����������Ŀ��λ��Ϊ�ϵ�ʱ��λ��
		feedrate = homing_feedrate[X_AXIS];	

		
		zUpFlag = eeprom_get_z_up_flag();
		if(zUpFlag == POWER_OFF_Z_UP_FLAG_SET)//Z��֮ǰ���ƹ�
			{
		destination[Z_AXIS] = current_position[Z_AXIS] - PD_PRINT_Z_UP_LENGTH;//Z����ǰ������һ�Σ��ָ��ϵ�ʱ��λ��
		POP_DEBUG("z up before\r\n");	
			}

		plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
		plan_buffer_line(destination[X_AXIS], destination[Y_AXIS], destination[Z_AXIS], destination[E_AXIS], feedrate / 60, active_extruder);
		for (i = 0; i < NUM_AXIS; i++)	current_position[i] = destination[i];
		
		
		st_synchronize();			
		
		eeprom_clear_z_up_flag();//�����־
		//ȷ����ӡͷ�ص��ϵ�ʱ��λ�ú󣬲���Ϊ�ϵ�׼��������ɣ�����ϵ��־		
		eeprom_clear_power_off_status();		


			
		card.FILE_STATE = PS_PIRNT;//���ڴ�ӡ	
		card_startFileprint();
		starttime = millis();
		sdCardUnplug.unplugFlag = TRUE;//ģ��β�һ��SD��
			POP_DEBUG("pd print state4\r\n");	
			pdPrintST = PDP_STATE5;
		}
	}
}

void pdBeginPrint(void)
{
	u8 state;
	state = pdPrintST;
	switch (state)
	{
		case PDP_STATE0:
			PDPState0();
			
			break;
			
		case PDP_STATE1:
		PDPState1();
	
			break;
			
		case PDP_STATE2:
		PDPState2();

			break;		
			
		case PDP_STATE3:
		PDPState3();

			break;

		case PDP_STATE4:
		PDPState4();

			break;						
		default:
			break;
	}

}


void pdCancelPSt0(void)
{

	if(cancelPDP ==TRUE)
	{
		if(degHotend0() < bck_extruder_temperature)
		{
			char cmd[30];
			
			//heat extru1,bed
			sprintf(cmd,"M104 S%.1f",bck_extruder_temperature);//���Ӽ���2���¶�
			 enquecommand(cmd);
			pdcST = PDC_STATE1;		
			POP_DEBUG("pdc state0\r\n");	
		}
	}
	
}

void pdCancelPSt1(void)
{

	if(cancelPDP ==TRUE)
	{
		if(degHotend0() >= bck_extruder_temperature)
		{
			char cmd[30];
			float zPos = bck_position[Z_AXIS] + PD_PRINT_Z_UP_LENGTH;
			st_synchronize();//ȷ��ǰһ�׶�����ִ�����
			//Z������һ��
		
			
			feedrate = homing_feedrate[Z_AXIS];
			sprintf(cmd,"G1 F%d Z%.1f ",(u32)feedrate,zPos);//z ����������ڶϵ�ʱ��λ��
			enquecommand(cmd);		
			
	
		 
			POP_DEBUG("pdc state1\r\n");	
			pdcST = PDC_STATE2;						
		}
	}
	
}

void pdCancelPSt2(void) 
{
	if(cancelPDP ==TRUE)
	{
		if(degHotend0() >= bck_extruder_temperature)//����ͷ�¶ȼ��ȵ�Ŀ���¶Ⱥ�
		{
		
			st_synchronize();//ȷ��ǰһ�׶�����ִ�����
			//X,Y,Z��ش�ӡԭ��
			user_cmd_move_xHome();
			
			
			menu_action_gcode("M104 S0");
			 menu_action_gcode("M140 S0");
			 menu_action_gcode("M84");//����ϵ�
			POP_DEBUG("pdc state2\r\n");	
			pdcST = PDC_STATE3;	
			cancelPDP = FALSE;
		}
	}
}
void pdCancelPrint(void)
{
	u8 state;
	state = pdcST;
	switch (state)
	{
		case PDC_STATE0:
		pdCancelPSt0();
			break;
			
		case PDC_STATE1:
		pdCancelPSt1();
			break;
			
		case PDC_STATE2:
		pdCancelPSt2();
			break;
			
		
		default:
			break;
	}
}

void pdPrintProcess(void)
{
	pdBeginPrint();
	//pdCancelPrint();
}
