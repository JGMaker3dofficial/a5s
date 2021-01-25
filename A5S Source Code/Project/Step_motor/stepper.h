/*
  stepper.h - stepper motor driver: executes motion plans of planner.c using the stepper motors
  Part of Grbl

  Copyright (c) 2009-2011 Simen Svale Skogsrud

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef stepper_h
#define stepper_h 

#include "planner.h"

#define	ST_TIMEX_PRESCALER	35
#define	ST_TIMEX_FREQUENCE	(SystemCoreClock/(ST_TIMEX_PRESCALER + 1))

#if EXTRUDERS > 1  //��ѹ��
  #define WRITE_E_STEP(v) { if(current_block->active_extruder == 1) { E1_STEP_PIN=v; } else { E0_STEP_PIN=v; }}
  #define NORM_E_DIR() { if(current_block->active_extruder == 1) { E1_DIR_PIN=!INVERT_E1_DIR; } else {E0_DIR_PIN=!INVERT_E0_DIR; }}
  #define REV_E_DIR()  { if(current_block->active_extruder == 1) { E1_DIR_PIN=INVERT_E1_DIR; }  else { E0_DIR_PIN=INVERT_E0_DIR; }}
#else
  #define WRITE_E_STEP(v) E0_STEP_PIN=v
  #define NORM_E_DIR() E0_DIR_PIN=!INVERT_E0_DIR
  #define REV_E_DIR()  E0_DIR_PIN=INVERT_E0_DIR
#endif

// Initialize and start the stepper motor subsystem
void st_init(void);

// Block until all buffered steps are executed
void st_synchronize(void);

// Set current position in steps
void st_set_position(const long x, const long y, const long z, const long e);
void st_set_e_position(const long e);

// Get current position in steps
long st_get_position(uint8_t axis);

// The stepper subsystem goes to sleep when it runs out of things to execute. Call this
// to notify the subsystem that it is time to go to work.
void st_wake_up(void);

  
void checkHitEndstops(void); //call from somwhere to create an serial error message with the locations the endstops where hit, in case they were triggered
void endstops_hit_on_purpose(void); //avoid creation of the message, i.e. after homeing and before a routine call of checkHitEndstops();

void enable_endstops(u8 check); // Enable/disable endstop checking

void checkStepperErrors(void); //Print errors detected by the stepper

void finishAndDisableSteppers(void);

//extern block_t *current_block;  // A pointer to the block currently being traced

void quickStop(void);

void digitalPotWrite(uint8_t address, uint8_t value);
void microstep_ms(uint8_t driver, int8_t ms1, int8_t ms2, int8_t ms3);
void microstep_mode(uint8_t driver, uint8_t stepping);
void digipot_init(void);
void digipot_current(uint8_t driver, uint8_t current);
void microstep_init(void);
void microstep_readings(void);
void st_synchronize_no_lcd(void);
void st_synchronize_no_lcd_pause_resume(void);
#endif
