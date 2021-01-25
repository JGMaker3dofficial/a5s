#ifndef __24CXX_H
#define __24CXX_H
#include "myiic.h"   
//////////////////////////////////////////////////////////////////////////////////	 


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define EE_TYPE AT24C64

#define	RESET_VALUE	0

//�����Լ�����
#define OPEN_STATE_ADDR			0
#define OPEN_STATE_VALUE		0X55

//ϵͳͼ���ļ�OK
#define SYSTEM_STATE_ADDR			1
#define SYSTEM_STATE_VALUE		0X35

//��׼�ֿ���ڵļ�¼
#define ST_FONT_STATE_ADDR			2
#define ST_FONT_STATE_VALUE		0XB5


//LCD��ĻУ׼
#define LCD_CALBT_STATE_ADDR		3
#define LCD_CALBT_STATE_VALUE		0X1A

//Һ��������������
#define LANGUAGE_STATE_ADDR		4
#define LANGUAGE_STATE_EN		0X25
#define LANGUAGE_STATE_CH		0XC5

//��ӡ����ͼ��״̬�ļ�¼
#define PRINTUI_BMP_STATE_ADDR		5
#define PRINTUI_BMP_STATE_VALUE		0XA5

//��������LOGOͼ��״̬�ļ�¼
#define LOGO_BMP_STATE_ADDR			6
#define LOGO_BMP_STATE_VALUE		0X75

//��Ŀ����14bytes
#define PROJECT_NAME_BASE_ADDR			7
#define PROJECT_NAME	"A3"


//PCBA(Ӳ���汾��)20bytes
//PCBA�汾��һ������·��еİ汾���࣬���෽���һ�����԰汾��һ�����а汾
#define PCBA_BASE_ADDR			21
#define	PCBA_VER_TEST					"V1.0 Test"
#define	PCBA_VER_RELEASE				"V1.0 Release"

//////////////////////////////////////////////////////////////////////////	 
//����TFT_LCD�������ĳ�ʼ��������Ϣ��ռ��14���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+13)
#define SAVE_ADDR_BASE 40

//PN�ţ�ռ20bytes
//���磺20171016-001A,���嶨��δ����
#define PN_BASE_ADDR   60
#define PN_INFOR	"20171016-001A"

//SN�ţ�ռ20bytes
//���磺20171016-001A,���嶨��δ����
#define SN_BASE_ADDR   80
#define SN_INFOR	"20171016-001A"

//����汾��--FW��25bytes
//����2017-10-10 15��21��32 Test
#define FW_BASE_ADDR   100
#define FW_VER_TEST		"2017-10-16 15��21��32 Test"
#define FW_VER_RELEASE	"2017-10-16 15��21��32 Release"

//�ϵ籣�漷��ͷ�¶���Ϣ,4bytes
#define POWER_OFF_EXTUDER_BASE_ADDR   125

//�ϵ籣���ȴ��¶���Ϣ,4bytes
#define POWER_OFF_BED_BASE_ADDR   129

//�ϵ籣��X��λ����Ϣ,4bytes
#define POWER_OFF_X_BASE_ADDR   133

//�ϵ籣��Y��λ����Ϣ,4bytes
#define POWER_OFF_Y_BASE_ADDR   137

//�ϵ籣��Z��λ����Ϣ,4bytes
#define POWER_OFF_Z_BASE_ADDR   141					  
					  
//�ϵ籣��E��λ����Ϣ,4bytes
#define POWER_OFF_E_BASE_ADDR   145					  
					  
//�ϵ籣���ӡ�ٷֱ�,1byte
#define POWER_OFF_PERCENTAGE_BASE_ADDR   149		

//�ϵ��־��1byte
#define POWER_OFF_FLAG_BASE_ADDR   150
#define POWER_OFF_FLAG_VAL_SET   0XAA
#define POWER_OFF_FLAG_VAL_RESET   0X05

//�ϵ籣��gcode�ļ��к�,4bytes
#define POWER_OFF_FILE_LINE_NUM_BASE_ADDR   151

//�ϵ籣��gcode�ļ�λ��,4bytes
#define POWER_OFF_FILE_POS_BASE_ADDR   155

//�ϵ籣��gcode�ļ���,2bytes					  
#define POWER_OFF_FN_LENGTH_BASE_ADDR   159	
	
//�ϵ籣���ӡʱ�䣬4bytes	
#define POWER_OFF_PRINT_TIME_BASE_ADDR  161	

//�ϵ籣�����ַ���ֵ��1byte	
#define POWER_OFF_FAN__BASE_ADDR  	165	
	
//�ϵ�ʱ����������� + plannerִ�л������е�gcode������	
#define POWER_OFF_NOT_EXACUTE_GCODE_BASE_ADDR	166


#define POWER_OFF_Z_UP_FLAG_ADDR	167//�ϵ�����Z�����Ʊ�־
#define POWER_OFF_Z_UP_FLAG_SET		0X3E
#define POWER_OFF_Z_UP_FLAG_RESET		0XA0




//�ϵ籣��gcode�ļ���,101bytes					  
#define POWER_OFF_FILE_NAME_BASE_ADDR   168					  
					  
					  
					  
					  
					  
u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ�����ȵ�����
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�����
u8 AT24CXX_Check(void);  //�������
void AT24CXX_Init(void); //��ʼ��IIC
#endif
















