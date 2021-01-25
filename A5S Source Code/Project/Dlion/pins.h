#ifndef PINS_H
#define PINS_H

#include "sys.h"
#include "Configuration.h"
//All these generations of Gen7 supply thermistor power
//via PS_ON, so ignore bad thermistor readings
//#define BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE


//X axis
#define X_AXIS_DIR_PORT					GPIOD
#define X_AXIS_DIR_PIN					GPIO_Pin_3
#define X_AXIS_DIR_CLK					RCC_APB2Periph_GPIOD

#define X_AXIS_STEP_PORT				GPIOD
#define X_AXIS_STEP_PIN					GPIO_Pin_6
#define X_AXIS_STEP_CLK					RCC_APB2Periph_GPIOD

#define X_AXIS_ENABLE_PORT				GPIOG
#define X_AXIS_ENABLE_PIN				GPIO_Pin_9
#define X_AXIS_ENABLE_CLK				RCC_APB2Periph_GPIOG

#define X_AXIS_MIN_PORT					GPIOC
#define X_AXIS_MIN_PIN					GPIO_Pin_6
#define X_AXIS_MIN_CLK					RCC_APB2Periph_GPIOC

#define X_AXIS_MAX_PORT					GPIOC
#define X_AXIS_MAX_PIN					GPIO_Pin_6
#define X_AXIS_MAX_CLK					RCC_APB2Periph_GPIOC

#define X_AXIS_MOTOR_PORT	(X_AXIS_DIR_PORT | X_AXIS_STEP_PORT | X_AXIS_ENABLE_PORT | X_AXIS_MIN_PORT |X_AXIS_MAX_PORT)
#define X_AXIS_MOTOR_CLK	(X_AXIS_DIR_CLK | X_AXIS_STEP_CLK | X_AXIS_ENABLE_CLK | X_AXIS_MIN_CLK | X_AXIS_MAX_CLK)



//Y axis
#define Y_AXIS_DIR_PORT					GPIOG
#define Y_AXIS_DIR_PIN					GPIO_Pin_11
#define Y_AXIS_DIR_CLK					RCC_APB2Periph_GPIOG

#define Y_AXIS_STEP_PORT				GPIOG
#define Y_AXIS_STEP_PIN					GPIO_Pin_12
#define Y_AXIS_STEP_CLK					RCC_APB2Periph_GPIOG

#define Y_AXIS_ENABLE_PORT				GPIOG
#define Y_AXIS_ENABLE_PIN				GPIO_Pin_13
#define Y_AXIS_ENABLE_CLK				RCC_APB2Periph_GPIOG

#define Y_AXIS_MIN_PORT					GPIOG
#define Y_AXIS_MIN_PIN					GPIO_Pin_8
#define Y_AXIS_MIN_CLK					RCC_APB2Periph_GPIOG


#define Y_AXIS_MOTOR_PORT	(Y_AXIS_DIR_PORT | Y_AXIS_STEP_PORT | Y_AXIS_ENABLE_PORT | Y_AXIS_MIN_PORT)
#define Y_AXIS_MOTOR_CLK	(Y_AXIS_DIR_CLK | Y_AXIS_STEP_CLK | Y_AXIS_ENABLE_CLK | Y_AXIS_MIN_CLK)

//Z axis
#define Z_AXIS_DIR_PORT					GPIOG
#define Z_AXIS_DIR_PIN					GPIO_Pin_14
#define Z_AXIS_DIR_CLK					RCC_APB2Periph_GPIOG

#define Z_AXIS_STEP_PORT				GPIOG
#define Z_AXIS_STEP_PIN					GPIO_Pin_15
#define Z_AXIS_STEP_CLK					RCC_APB2Periph_GPIOG

#define Z_AXIS_ENABLE_PORT				GPIOB
#define Z_AXIS_ENABLE_PIN				GPIO_Pin_8
#define Z_AXIS_ENABLE_CLK				RCC_APB2Periph_GPIOB

#define Z_AXIS_MIN_PORT					GPIOG
#define Z_AXIS_MIN_PIN					GPIO_Pin_7
#define Z_AXIS_MIN_CLK					RCC_APB2Periph_GPIOG

#define Z_AXIS_MOTOR_PORT	(Z_AXIS_DIR_PORT | Z_AXIS_STEP_PORT | Z_AXIS_ENABLE_PORT | Z_AXIS_MIN_PORT)
#define Z_AXIS_MOTOR_CLK	(Z_AXIS_DIR_CLK | Z_AXIS_STEP_CLK | Z_AXIS_ENABLE_CLK | Z_AXIS_MIN_CLK)

//E0 axis
#define E0_AXIS_DIR_PORT				GPIOB
#define E0_AXIS_DIR_PIN					GPIO_Pin_9
#define E0_AXIS_DIR_CLK					RCC_APB2Periph_GPIOB

#define E0_AXIS_STEP_PORT				GPIOE
#define E0_AXIS_STEP_PIN				GPIO_Pin_2
#define E0_AXIS_STEP_CLK				RCC_APB2Periph_GPIOE

#define E0_AXIS_ENABLE_PORT				GPIOE
#define E0_AXIS_ENABLE_PIN				GPIO_Pin_3
#define E0_AXIS_ENABLE_CLK				RCC_APB2Periph_GPIOE

#define E0_AXIS_MOTOR_PORT	(E0_AXIS_DIR_PORT | E0_AXIS_STEP_PORT | E0_AXIS_ENABLE_PORT )
#define E0_AXIS_MOTOR_CLK	(E0_AXIS_DIR_CLK | E0_AXIS_STEP_CLK | E0_AXIS_ENABLE_CLK )



//E1 axis
#define E1_AXIS_DIR_PORT				GPIOE
#define E1_AXIS_DIR_PIN					GPIO_Pin_4
#define E1_AXIS_DIR_CLK					RCC_APB2Periph_GPIOE

#define E1_AXIS_STEP_PORT				GPIOE
#define E1_AXIS_STEP_PIN				GPIO_Pin_5
#define E1_AXIS_STEP_CLK				RCC_APB2Periph_GPIOE

#define E1_AXIS_ENABLE_PORT				GPIOE
#define E1_AXIS_ENABLE_PIN				GPIO_Pin_6
#define E1_AXIS_ENABLE_CLK				RCC_APB2Periph_GPIOE
#define E1_AXIS_MOTOR_PORT				(E1_AXIS_DIR_PORT | E1_AXIS_STEP_PORT | E1_AXIS_ENABLE_PORT )
#define E1_AXIS_MOTOR_CLK				(E1_AXIS_DIR_CLK | E1_AXIS_STEP_CLK | E1_AXIS_ENABLE_CLK )
	


#define	ALL_STEP_MOTOR_CLK				(X_AXIS_MOTOR_CLK | Y_AXIS_MOTOR_CLK | Z_AXIS_MOTOR_CLK |E0_AXIS_MOTOR_CLK |E1_AXIS_MOTOR_CLK)
#define	ALL_STEP_MOTOR_PORT 			(X_AXIS_MOTOR_PORT | Y_AXIS_MOTOR_PORT | Z_AXIS_MOTOR_PORT | E0_AXIS_MOTOR_PORT | E1_AXIS_MOTOR_PORT)
#define STEP_MOTOR_RCC_APBX_PERIPH_CLK_CMD			RCC_APB2PeriphClockCmd(ALL_STEP_MOTOR_CLK,ENABLE)


//power IO
#define HEATE_EXT0_PORT			GPIOA
#define HEATER0_EXT0_PIN		GPIO_Pin_2
#define HEATER0_EXT0_CLK		RCC_APB2Periph_GPIOA 



#define HEATE_BED_PORT		GPIOA
#define HEATE_BED_PIN		GPIO_Pin_3
#define HEATE_BED_CLK		RCC_APB2Periph_GPIOA


#define CONT_FAN_PORT		GPIOA
#define CONT_FAN_PIN		GPIO_Pin_1
#define CONT_FAN_CLK		RCC_APB2Periph_GPIOA


//#define E0_FAN_PORT		GPIOA
//#define E0_FAN_PIN		GPIO_Pin_1
//#define E0_FAN_CLK		RCC_APB2Periph_GPIOA

#define POWER_IO_PORT	(HEATE_EXT0_PORT | HEATE_BED_PORT | CONT_FAN_PORT  )
#define POWER_IO_CLK 	( HEATER0_EXT0_CLK | HEATE_BED_CLK | CONT_FAN_CLK  | RCC_APB2Periph_AFIO )
#define POWER_IO_REMAP    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE)//pb3,pb4原本为J-Link调试口，复用为普通IO

#define POWER_IO_RCC_APBX_PERIPH_CLK_CMD			RCC_APB2PeriphClockCmd(POWER_IO_CLK,ENABLE)

//ADC IO
#define	ADC_EXT0_PORT	 			GPIOC
#define	ADC_EXT0_PIN				GPIO_Pin_2
#define ADC_EXT0_CLK				RCC_APB2Periph_GPIOC

#define	ADC_BED_PORT				 GPIOC
#define	ADC_BED_PIN					GPIO_Pin_1
#define ADC_BED_CLK					RCC_APB2Periph_GPIOC

#define ADC_TEMP_PORT	( ADC_EXT0_PORT | ADC_BED_PORT)
#define ADC_TEMP_CLK	( ADC_EXT0_CLK | ADC_BED_CLK)

#define	ADC_NUMx		ADC3
#define ADC_NUMx_CLK	 RCC_APB2Periph_ADC3
#define ADC_TEMP_RCC_APBX_PERIPH_CLK_CMD			RCC_APB2PeriphClockCmd(ADC_TEMP_CLK | ADC_NUMx_CLK,ENABLE)


//export control pin 
#define X_DIR_PIN     PDout(3)
#define X_STEP_PIN    PDout(6)
#define X_ENABLE_PIN  PGout(9)
#if MERCHIN	== A1	//A1 used Xmax pin to detect homing
	#define X_MAX_PIN     PCin(6)
/////////////////////////////////////////////////////
#else
	#define X_MIN_PIN     PCin(6)
	/////////////////////////////////////////////////////
#endif

//#define X_MAX_PIN     PGin(15)
//#define X_MS1_PIN 	   PAout(14)
//#define X_MS2_PIN 	   PAout(13)
//#define X_MS3_PIN 	   PAout(9)

#define Y_DIR_PIN      PGout(11)
#define Y_STEP_PIN     PGout(12)
#define Y_ENABLE_PIN   PGout(13)
#define Y_MIN_PIN      PGin(8)
//#define Y_MAX_PIN      PGin(14)
//#define Y_MS1_PIN      PGout(8)
//#define Y_MS2_PIN      PGout(7)
//#define Y_MS3_PIN      PGout(6)

#define Z_DIR_PIN      PGout(14)
#define Z_STEP_PIN     PGout(15)
#define Z_ENABLE_PIN   PBout(8)
#define Z_MIN_PIN      PGin(7)
//#define Z_MAX_PIN      PGin(10)
//#define Z_MAX_PIN      PGin(13)
//#define Z_MS1_PIN      PCout(4)
//#define Z_MS2_PIN      PCout(3)
//#define Z_MS3_PIN      PCout(2)

#define E0_DIR_PIN     PBout(9)
#define E0_STEP_PIN    PEout(2)
#define E0_ENABLE_PIN  PEout(3)
//#define E0_MS1_PIN     PFout(9)
//#define E0_MS2_PIN 	   PFout(8)
//#define E0_MS3_PIN 	   PFout(7)

#define UART_DTR_PIN     PAin(8)

#define E1_DIR_PIN	   PEout(4)
#define E1_STEP_PIN    PEout(5)
#define E1_ENABLE_PIN   PEout(6)
//#define E1_MS1_PIN      PEout(5)
//#define E1_MS2_PIN      PEout(4)
//#define E1_MS3_PIN      PEout(3)

//风扇、热挤出头、热床
#define  HEATER_0_PIN   PAout(2)//TIM8->CCR2	  //E0_PWM T8_2 
//#define HEATER_1_PIN   TIM5->CCR3	  //E1_PWM T5_3
#define  HEATER_BED_PIN PAout(3)//TIM8->CCR3	  //BED_PWM T8_3

#define  FAN_PIN        PAout(1)//TIM5->CCR1	  //BED_FAN T5_1//FAN1-涡轮风扇
//#define  E0_FAN       PAout(1)//PAout(1) // TIM5->CCR2  	//E0_FAN T5_2 //W-FAN-方形风扇



//温度检测
#define TEMP_0_PIN	   (Get_Adc(ADC_Channel_12)>>2)   						// AD3_4	E0_TEMP
//#define TEMP_1_PIN	 (Get_Adc(ADC_Channel_6)>>2)   						// AD3_6	E1_TEMP
#define TEMP_BED_PIN   (Get_Adc(ADC_Channel_11)>>2)   						// AD3_5  BED_TEMP





#endif
