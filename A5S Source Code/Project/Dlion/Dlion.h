// Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.
// Licence: GPL

#ifndef DLION_H
#define DLION_H

//#define  FORCE_INLINE __attribute__((always_inline)) inline

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys.h"
//#include <inttypes.h>

//#include <delay.h>
//#include <avr/pgmspace.h>
//#include <avr/eeprom.h>
//#include <avr/interrupt.h>


//#include "fastio.h"
#include "Configuration.h"
#include "pins.h"
#include "adc.h"
#include "bsp.h"


#define DLION_PRINTF	log_printf_UART1	// GCODEÓ¦´ð


#define DLION_DEBUG_EN

#ifdef DLION_DEBUG_EN
	#define DLION_DEBUG	DBG
#else
	#define DLION_DEBUG(format,...)	((void) 0)
#endif

#define OK_KEY (Get_Adc_Average(ADC_Channel_15,10)>3250)

void get_command(void);
void process_commands(void);
void manage_inactivity(void);
 void homeaxis(int axis);
#define  enable_x()  X_ENABLE_PIN=X_ENABLE_ON
#define disable_x()	 X_ENABLE_PIN=!X_ENABLE_ON

#define  enable_y()  Y_ENABLE_PIN=Y_ENABLE_ON
#define disable_y()	 Y_ENABLE_PIN=!Y_ENABLE_ON

#define  enable_z()  Z_ENABLE_PIN=Z_ENABLE_ON
#define disable_z()	 Z_ENABLE_PIN=!Z_ENABLE_ON

#define  enable_e0() E0_ENABLE_PIN=E_ENABLE_ON
#define disable_e0() E0_ENABLE_PIN=!E_ENABLE_ON

#define  enable_e1() E1_ENABLE_PIN=E_ENABLE_ON
#define disable_e1() E1_ENABLE_PIN=!E_ENABLE_ON

enum AxisEnum {X_AXIS=0, Y_AXIS=1, Z_AXIS=2, E_AXIS=3};

#define SERIAL_ERROR_START	DLION_DEBUG("Error:")
#define SERIAL_ECHO_START	DLION_DEBUG("echo:")
void loop(void);
void setup(void);
void FlushSerialRequestResend(void);
void ClearToSend(void);

void get_coordinates(void);
void prepare_move(void);
void kill(void);
void Stop(void);
void Cmd_MoveAxisCommand(float *pDestination);
u8 IsStopped(void);

void enquecommand(const char *cmd); //put an ascii command at the end of the current buffer.
//void enquecommand_P(const char *cmd); //put an ascii command at the end of the current buffer, read from flash
void prepare_arc_move(u8 isclockwise);

void clamp_to_software_endstops(float target[3]);

#define CRITICAL_SECTION_START  __disable_irq();
#define CRITICAL_SECTION_END    __enable_irq();
extern float feedrate, next_feedrate, saved_feedrate;
extern float homing_feedrate[];
extern u8 axis_relative_modes[];
extern volatile int feedmultiply;
extern int extrudemultiply; // Sets extrude multiply factor (in percent)
extern float current_position[NUM_AXIS] ;
extern float add_homeing[3];
extern float min_pos[3];
extern float max_pos[3];
extern int fanSpeed;
extern float destination[NUM_AXIS];
extern float saved_position[NUM_AXIS];
extern float temp_posistion[NUM_AXIS];
#ifdef BARICUDA
extern int ValvePressure;
extern int EtoPPressure;
#endif

#ifdef FWRETRACT
extern bool autoretract_enabled;
extern bool retracted;
extern float retract_length, retract_feedrate, retract_zlift;
extern float retract_recover_length, retract_recover_feedrate;
#endif

extern unsigned long starttime;
extern unsigned long stoptime;
extern u8 stopDuringPrintFlag;

// Handling multiple extruders pins
extern uint8_t active_extruder;

void pause_print_loop(void);


extern volatile int  buflen;


extern u8 readLineFlag;
void clear_cmd_buf(void);

u8 Get_Move_State(void);


#endif
