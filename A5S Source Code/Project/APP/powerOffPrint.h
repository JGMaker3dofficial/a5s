

#ifndef __POWEROFFPRINT_H
#define __POWEROFFPRINT_H 	 


#include "app_gui.h"
#define	TEP_OFFSET	2	//�ϵ�����׼���׶��¶������
#define PD_PRINT_Z_UP_LENGTH	5
#define PD_PRINT_X_LEFT_LENGTH	10
typedef enum
{
	PDP_STATE0 = 0,//Z������һ��
	PDP_STATE1 = 1,//X���ԭ�� + Y���ԭ�� + Z���ԭ��
	PDP_STATE2 = 2,//Z�����Ƴ����ϵ籣���λ��
	PDP_STATE3 = 3,//E�����һ��
	PDP_STATE4 = 4,//��ʼ�ϵ�����
	PDP_STATE5 = 5,
	PDP_STATE6 = 6,
}_pdp_prepare_state_e;
extern _pdp_prepare_state_e pdPrintST;//�ϵ�����׼������״̬��־
extern u32 powerDownPrintTime;


void powerOffPrint_screen(void);
void exacute_power_off_print_callback(void);


void prepare_power_down_print(void);
void cancel_power_down_print(void);
void power_down_print_init(void);
void PDPState0(void);
void PDPState1(void);
void PDPState2(void);
void PDPState3(void);
void PDPState4(void);
void pdBeginPrint(void);
void pdCancelPSt0(void);
void pdCancelPSt1(void);
void pdCancelPSt2(void);
void pdCancelPrint(void);
void pdPrintProcess(void);

#endif
