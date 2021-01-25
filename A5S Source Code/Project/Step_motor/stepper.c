#include "bsp_timer.h"
#include "Dlion.h"
#include "stepper.h"
#include "Configuration_adv.h"
#include "Configuration.h"
#include "speed_lookuptable.h"
#include "bsp_usart.h"
#include "spi.h"
#include "language.h"
#include "temperature.h"
#include "app_gui.h"
#include "bsp.h"

#define DIGIPOT_CHANNELS {4,1,0,2,3} // X Y Z E0 E1 digipot channels to stepper driver mapping

static u8 subsection_x_value = 1;
static u8 subsection_y_value = 1;
static u8 subsection_z_value = 1;
static u8 subsection_e0_value = 1;
static u8 subsection_e1_value = 1;


#define ENABLE_STEPPER_DRIVER_INTERRUPT()  TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);  //使能TIMx
#define DISABLE_STEPPER_DRIVER_INTERRUPT() TIM_ITConfig(TIM3,TIM_IT_Update, DISABLE);




//===========================================================================
//=============================public variables  ============================
//===========================================================================
block_t *current_block;  // A pointer to the block currently being traced


//===========================================================================
//=============================private variables ============================
//===========================================================================
//static makes it inpossible to be called from outside of this file by extern.!

// Variables used by The Stepper Driver Interrupt
static unsigned char out_bits;        // The next stepping-bits to be output
static long counter_x,       // Counter variables for the bresenham line tracer
       counter_y,
       counter_z,
       counter_e;
volatile static unsigned long step_events_completed; // The number of step events executed in the current block
#ifdef ADVANCE
static long advance_rate, advance, final_advance = 0;
static long old_advance = 0;
static long e_steps[3];
#endif
static long acceleration_time, deceleration_time;
//static unsigned long accelerate_until, decelerate_after, acceleration_rate, initial_rate, final_rate, nominal_rate;
static unsigned short acc_step_rate; // needed for deccelaration start point
static char step_loops;
static unsigned short TIME3_nominal;
//static unsigned short step_loops_nominal;

volatile long endstops_trigsteps[3] = {0, 0, 0};
volatile long endstops_stepsTotal, endstops_stepsDone;
static volatile u8 endstop_x_hit = FALSE;
static volatile u8 endstop_y_hit = FALSE;
static volatile u8 endstop_z_hit = FALSE;
#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
u8 abort_on_endstop_hit = FALSE;
#endif
#if defined X_MIN_PIN
static u8 old_x_min_endstop = TRUE;
#endif
#if defined X_MAX_PIN
static u8 old_x_max_endstop = TRUE;
#endif
#if defined Y_MIN_PIN
static u8 old_y_min_endstop = TRUE;
#endif
#if defined Y_MAX_PIN
static u8 old_y_max_endstop = TRUE;
#endif
#if defined Z_MIN_PIN
static u8 old_z_min_endstop = TRUE;
#endif
#if defined Z_MAX_PIN
static u8 old_z_max_endstop = TRUE;
#endif
static u8 check_endstops = TRUE;

volatile long count_position[NUM_AXIS] = { 0, 0, 0, 0};
volatile signed char count_direction[NUM_AXIS] = { 1, 1, 1, 1};

//===========================================================================
//=============================functions         ============================
//===========================================================================

#define CHECK_ENDSTOPS  if(check_endstops)


#define MultiU24X24toH16(intRes, longIn1, longIn2) intRes= ((uint64_t)(longIn1) * (longIn2)) >> 24
#define MultiU16X8toH16(intRes, charIn1, intIn2) intRes = ((charIn1) * (intIn2)) >> 16
void checkHitEndstops(void)
{
    if( endstop_x_hit || endstop_y_hit || endstop_z_hit) {
        SERIAL_ECHO_START;
        printf(MSG_ENDSTOPS_HIT);
        if(endstop_x_hit) {
            printf(" X:%f", (float)endstops_trigsteps[X_AXIS] / axis_steps_per_unit[X_AXIS]);
            // LCD_MESSAGEPGM(MSG_ENDSTOPS_HIT "X");  ////////////////////////////////////////////////////
        }
        if(endstop_y_hit) {
            printf(" Y:%f", (float)endstops_trigsteps[Y_AXIS] / axis_steps_per_unit[Y_AXIS]);
            //  LCD_MESSAGEPGM(MSG_ENDSTOPS_HIT "Y");   ////////////////////////////////////////////////////////////
        }
        if(endstop_z_hit) {
            printf(" Z:%f", (float)endstops_trigsteps[Z_AXIS] / axis_steps_per_unit[Z_AXIS]);
            //  LCD_MESSAGEPGM(MSG_ENDSTOPS_HIT "Z");  ////////////////////////////////////////////
        }
        printf("\n");
        endstop_x_hit = FALSE;
        endstop_y_hit = FALSE;
        endstop_z_hit = FALSE;
#ifdef ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED
        if (abort_on_endstop_hit)
        {
            card.sdprinting = FALSE;
            card.closefile();
            quickStop();
            setTargetHotend0(0);
            setTargetHotend1(0);
            setTargetHotend2(0);
        }
#endif
    }
}

void endstops_hit_on_purpose(void)
{
    endstop_x_hit = FALSE;
    endstop_y_hit = FALSE;
    endstop_z_hit = FALSE;
}

void enable_endstops(u8 check)
{
    check_endstops = check;
}


void st_wake_up(void) {
    ENABLE_STEPPER_DRIVER_INTERRUPT();
}
//         __________________________
//        /|                        |\     _________________         ^
//       / |                        | \   /|               |\        |
//      /  |                        |  \ / |               | \       s
//     /   |                        |   |  |               |  \      p
//    /    |                        |   |  |               |   \     e
//   +-----+------------------------+---+--+---------------+----+    e
//   |               BLOCK 1            |      BLOCK 2          |    d
//
//                           time ----->
//
//  The trapezoid is the shape the speed curve over time. It starts at block->initial_rate, accelerates
//  first block->accelerate_until step_events_completed, then keeps going at constant speed until
//  step_events_completed reaches block->decelerate_after after which it decelerates until the trapezoid generator is reset.
//  The slope of acceleration is calculated with the leib ramp alghorithm.

void step_wait(void)
{
    u8 i;
    for( i = 0; i < 6; i++) {
    }
}


unsigned short calc_timer(unsigned short step_rate)
{
    unsigned short timer;
    if(step_rate > MAX_STEP_FREQUENCY) step_rate = MAX_STEP_FREQUENCY;

    if(step_rate > 20000) {
        step_rate = (step_rate >> 2) & 0x3fff;
        step_loops = 4;
    }
    else if(step_rate > 10000) {
        step_rate = (step_rate >> 1) & 0x7fff;
        step_loops = 2;
    }
    else {
        step_loops = 1;
    }

    if(step_rate < 32) step_rate = 32;
    timer = ST_TIMEX_FREQUENCE / step_rate - 1;
    if(timer < 100) {
        timer = 100;    //(20kHz this should never happen)
        printf(MSG_STEPPER_TO_HIGH);
        printf("%d", step_rate);
    }
    return timer;
}

// Initializes the trapezoid generator from the current block. Called whenever a new
// block begins.
void trapezoid_generator_reset(void)
{
#ifdef ADVANCE
    advance = current_block->initial_advance;
    final_advance = current_block->final_advance;
    // Do E steps + advance steps
    e_steps[current_block->active_extruder] += ((advance >> 8) - old_advance);
    old_advance = advance >> 8;
#endif
    deceleration_time = 0;
    // step_rate to timer interval
    TIME3_nominal = calc_timer(current_block->nominal_rate);
    // make a note of the number of step loops required at nominal speed
    //step_loops_nominal = step_loops;
    acc_step_rate = current_block->initial_rate;
    acceleration_time = calc_timer(acc_step_rate);
    TIM_SetAutoreload(TIM3, acceleration_time - 1);
}

// "The Stepper Driver Interrupt" - This timer interrupt is the workhorse.
// It pops blocks from the block_buffer and executes them by pulsing the stepper pins appropriately.

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {   TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
//printf("T3\n");
        // If there is no current block, attempt to pop one from the buffer

        if (current_block == NULL) {
            // Anything in the buffer?
            current_block = plan_get_current_block();
            if (current_block != NULL) {
                current_block->busy = TRUE;
                trapezoid_generator_reset();
                counter_x = -(current_block->step_event_count >> 1);
                counter_y = counter_x;
                counter_z = counter_x;
                counter_e = counter_x;
                step_events_completed = 0;

#ifdef Z_LATE_ENABLE
                if(current_block->steps_z > 0) {
                    enable_z();
                    TIM_SetAutoreload(TIM3, 2000 - 1); //1ms wait
                    return;
                }
#endif
//      #ifdef ADVANCE
//      e_steps[current_block->active_extruder] = 0;
//      #endif
            }
            else {
                TIM_SetAutoreload(TIM3, 2000 - 1);
            }
        }

        if (current_block != NULL) {
            // Set directions TO DO This should be done once during init of trapezoid. Endstops -> interrupt
            out_bits = current_block->direction_bits;

            // Set direction en check limit switches
            if ((out_bits & (1 << X_AXIS)) != 0) { // stepping along -X axis
#if !defined COREXY  //NOT COREXY
                X_DIR_PIN = INVERT_X_DIR;
#endif
                count_direction[X_AXIS] = -1;
                CHECK_ENDSTOPS
                {
#if defined X_MIN_PIN
                    u8 x_min_endstop = X_MIN_PIN != X_ENDSTOPS_INVERTING ;
                    if(x_min_endstop && old_x_min_endstop && (current_block->steps_x > 0))
                    {
                        endstops_trigsteps[X_AXIS] = count_position[X_AXIS];
                        endstop_x_hit = TRUE;
                        step_events_completed = current_block->step_event_count;
                    }
                    old_x_min_endstop = x_min_endstop;
#endif
                }
            }
            else { // +direction
#if !defined COREXY  //NOT COREXY
                X_DIR_PIN = !INVERT_X_DIR;
#endif

                count_direction[X_AXIS] = 1;
                CHECK_ENDSTOPS
                {
#if defined X_MAX_PIN
                    u8 x_max_endstop = X_MAX_PIN != X_ENDSTOPS_INVERTING;
                    if(x_max_endstop && old_x_max_endstop && (current_block->steps_x > 0)) {
                        endstops_trigsteps[X_AXIS] = count_position[X_AXIS];
                        endstop_x_hit = TRUE;
                        step_events_completed = current_block->step_event_count;
                    }
                    old_x_max_endstop = x_max_endstop;
#endif
                }
            }

            if ((out_bits & (1 << Y_AXIS)) != 0) { // -direction
#if !defined COREXY  //NOT COREXY
                Y_DIR_PIN = INVERT_Y_DIR;
#endif
                count_direction[Y_AXIS] = -1;
                CHECK_ENDSTOPS
                {
#if defined(Y_MIN_PIN) //&& Y_MIN_PIN > -1
                    u8 y_min_endstop = Y_MIN_PIN != Y_ENDSTOPS_INVERTING;
                    if(y_min_endstop && old_y_min_endstop && (current_block->steps_y > 0)) {
                        endstops_trigsteps[Y_AXIS] = count_position[Y_AXIS];
                        endstop_y_hit = TRUE;
                        step_events_completed = current_block->step_event_count;
                    }
                    old_y_min_endstop = y_min_endstop;
#endif
                }
            }
            else { // +direction
#if !defined COREXY  //NOT COREXY
                Y_DIR_PIN = !INVERT_Y_DIR;
#endif
                count_direction[Y_AXIS] = 1;
                CHECK_ENDSTOPS
                {
#if defined(Y_MAX_PIN)// && Y_MAX_PIN > -1
                    u8 y_max_endstop = Y_MAX_PIN != Y_ENDSTOPS_INVERTING;
                    if(y_max_endstop && old_y_max_endstop && (current_block->steps_y > 0)) {
                        endstops_trigsteps[Y_AXIS] = count_position[Y_AXIS];
                        endstop_y_hit = TRUE;
                        step_events_completed = current_block->step_event_count;
                    }
                    old_y_max_endstop = y_max_endstop;
#endif
                }
            }



            if ((out_bits & (1 << Z_AXIS)) != 0) { // -direction
                Z_DIR_PIN = INVERT_Z_DIR;

#ifdef Z_DUAL_STEPPER_DRIVERS
                // WRITE(Z2_DIR_PIN,INVERT_Z_DIR);
#endif

                count_direction[Z_AXIS] = -1;
                CHECK_ENDSTOPS
                {
#if defined(Z_MIN_PIN)
                    u8 z_min_endstop = Z_MIN_PIN != Z_ENDSTOPS_INVERTING;
                    if(z_min_endstop && old_z_min_endstop && (current_block->steps_z > 0)) {
                        endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
                        endstop_z_hit = TRUE;
                        step_events_completed = current_block->step_event_count;
                    }
                    old_z_min_endstop = z_min_endstop;
#endif
                }
            }
            else { // +direction
                Z_DIR_PIN = !INVERT_Z_DIR;

#ifdef Z_DUAL_STEPPER_DRIVERS
                // WRITE(Z2_DIR_PIN,!INVERT_Z_DIR);
#endif

                count_direction[Z_AXIS] = 1;
                CHECK_ENDSTOPS
                {
#if defined(Z_MAX_PIN)
                    u8 z_max_endstop = Z_MAX_PIN != Z_ENDSTOPS_INVERTING;
                    if(z_max_endstop && old_z_max_endstop && (current_block->steps_z > 0)) {
                        endstops_trigsteps[Z_AXIS] = count_position[Z_AXIS];
                        endstop_z_hit = TRUE;
                        step_events_completed = current_block->step_event_count;
                    }
                    old_z_max_endstop = z_max_endstop;
#endif
                }
            }

#ifndef ADVANCE

            if ((out_bits & (1 << E_AXIS)) != 0) { // -direction
                REV_E_DIR();
                count_direction[E_AXIS] = -1;
            }
            else { // +direction
                NORM_E_DIR();
                count_direction[E_AXIS] = 1;
            }

#endif //!ADVANCE


            {   u8 i;
                for(i = 0; i < step_loops; i++) { // Take multiple steps per interrupt (For high speed moves)
                    //  printf("1\n\r");
//	  #if !EN_USART1_RX
//      checkRx(); // Check for serial chars.//20160228
//     #endif
#ifdef ADVANCE

                    counter_e += current_block->steps_e;
                    if (counter_e > 0) {
                        counter_e -= current_block->step_event_count;
                        if ((out_bits & (1 << E_AXIS)) != 0) { // - direction
                            e_steps[current_block->active_extruder]--;
                        }
                        else {
                            e_steps[current_block->active_extruder]++;
                        }
                    }

#endif //ADVANCE

#if !defined COREXY

                    counter_x += current_block->steps_x;
                    if (counter_x > 0) {
                        X_STEP_PIN = !INVERT_X_STEP_PIN;
                        counter_x -= current_block->step_event_count;
                        count_position[X_AXIS] += count_direction[X_AXIS];
                        X_STEP_PIN = INVERT_X_STEP_PIN;
                    }

                    counter_y += current_block->steps_y;
                    if (counter_y > 0) {
                        Y_STEP_PIN = !INVERT_Y_STEP_PIN;
                        counter_y -= current_block->step_event_count;
                        count_position[Y_AXIS] += count_direction[Y_AXIS];
                        Y_STEP_PIN = INVERT_Y_STEP_PIN;
                    }
#endif

                    counter_z += current_block->steps_z;
                    if (counter_z > 0) {
                        Z_STEP_PIN = !INVERT_Z_STEP_PIN;

#ifdef Z_DUAL_STEPPER_DRIVERS
                        Z2_STEP_PIN = !INVERT_Z_STEP_PIN;
#endif

                        counter_z -= current_block->step_event_count;
                        count_position[Z_AXIS] += count_direction[Z_AXIS];
                        Z_STEP_PIN = INVERT_Z_STEP_PIN;

#ifdef Z_DUAL_STEPPER_DRIVERS
                        Z2_STEP_PIN = INVERT_Z_STEP_PIN;
#endif
                    }

#ifndef ADVANCE

                    counter_e += current_block->steps_e;
                    if (counter_e > 0) {
                        WRITE_E_STEP(!INVERT_E_STEP_PIN);
                        counter_e -= current_block->step_event_count;
                        count_position[E_AXIS] += count_direction[E_AXIS];
                        WRITE_E_STEP(INVERT_E_STEP_PIN);
                    }

#endif //!ADVANCE

                    step_events_completed += 1;
                    if(step_events_completed >= current_block->step_event_count) break;
                }
            }
            {   // Calculare new timer value

                unsigned short timer;
                unsigned short step_rate;
                if (step_events_completed <= (unsigned long int)current_block->accelerate_until)
                {
                    MultiU24X24toH16(acc_step_rate, acceleration_time, current_block->acceleration_rate);
                    acc_step_rate += current_block->initial_rate;

                    // upper limit
                    if(acc_step_rate > current_block->nominal_rate)
                        acc_step_rate = current_block->nominal_rate;

                    // step_rate to timer interval
                    timer = calc_timer(acc_step_rate);
                    //  printf("1:%ld\r\n",timer);//timer4_millis);
                    TIM_SetAutoreload(TIM3, timer - 1);
                    acceleration_time += timer;
#ifdef ADVANCE
                    for(i = 0; i < step_loops; i++) {
                        advance += advance_rate;
                    }
                    //if(advance > current_block->advance) advance = current_block->advance;
                    // Do E steps + advance steps
                    e_steps[current_block->active_extruder] += ((advance >> 8) - old_advance);
                    old_advance = advance >> 8;

#endif
                }
                else if (step_events_completed > (unsigned long int)current_block->decelerate_after)
                {
                    MultiU24X24toH16(step_rate, deceleration_time, current_block->acceleration_rate);

                    if(step_rate > acc_step_rate) { // Check step_rate stays positive
                        step_rate = current_block->final_rate;
                    }
                    else {
                        step_rate = acc_step_rate - step_rate; // Decelerate from aceleration end point.
                    }

                    // lower limit
                    if(step_rate < current_block->final_rate)
                        step_rate = current_block->final_rate;

                    // step_rate to timer interval
                    timer = calc_timer(step_rate);
                    //  printf("2:%ld\r\n",timer);
                    TIM_SetAutoreload(TIM3, timer - 1);
                    deceleration_time += timer;
#ifdef ADVANCE
                    for(i = 0; i < step_loops; i++) {
                        advance -= advance_rate;
                    }
                    if(advance < final_advance) advance = final_advance;
                    // Do E steps + advance steps
                    e_steps[current_block->active_extruder] += ((advance >> 8) - old_advance);
                    old_advance = advance >> 8;
#endif //ADVANCE	
                }
                else
                {   // printf("3:%ld\r\n",TIME3_nominal);
                    TIM_SetAutoreload(TIM3, TIME3_nominal - 1);
                    // ensure we're running at the correct step rate, even if we just came off an acceleration
                    //  step_loops = step_loops_nominal;
                }
            }     // Calculare new timer value
            // If current block is finished, reset pointer
            if (step_events_completed >= current_block->step_event_count)
            {
                current_block = NULL;
                plan_discard_current_block();
            }
        }
    }
}

#ifdef ADVANCE

#endif // ADVANCE 

void st_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


//电机方向、使能、脉冲
    STEP_MOTOR_RCC_APBX_PERIPH_CLK_CMD;
//x axis
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = X_AXIS_DIR_PIN;
    GPIO_Init(X_AXIS_DIR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = X_AXIS_STEP_PIN;
    GPIO_Init(X_AXIS_STEP_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = X_AXIS_ENABLE_PIN;
    GPIO_Init(X_AXIS_ENABLE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = X_AXIS_MIN_PIN;
    GPIO_Init(X_AXIS_MIN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = X_AXIS_MAX_PIN;
    GPIO_Init(X_AXIS_MAX_PORT, &GPIO_InitStructure);
// y axis

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = Y_AXIS_DIR_PIN;
    GPIO_Init(Y_AXIS_DIR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Y_AXIS_STEP_PIN;
    GPIO_Init(Y_AXIS_STEP_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Y_AXIS_ENABLE_PIN;
    GPIO_Init(Y_AXIS_ENABLE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = Y_AXIS_MIN_PIN;
    GPIO_Init(Y_AXIS_MIN_PORT, &GPIO_InitStructure);

//z axis
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = Z_AXIS_DIR_PIN;
    GPIO_Init(Z_AXIS_DIR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Z_AXIS_STEP_PIN;
    GPIO_Init(Z_AXIS_STEP_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = Z_AXIS_ENABLE_PIN;
    GPIO_Init(Z_AXIS_ENABLE_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = Z_AXIS_MIN_PIN;
    GPIO_Init(Z_AXIS_MIN_PORT, &GPIO_InitStructure);

// e0 axis
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = E0_AXIS_DIR_PIN;
    GPIO_Init(E0_AXIS_DIR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = E0_AXIS_STEP_PIN;
    GPIO_Init(E0_AXIS_STEP_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = E0_AXIS_ENABLE_PIN;
    GPIO_Init(E0_AXIS_ENABLE_PORT, &GPIO_InitStructure);

//e1 axis
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = E1_AXIS_DIR_PIN;
    GPIO_Init(E1_AXIS_DIR_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = E1_AXIS_STEP_PIN;
    GPIO_Init(E1_AXIS_STEP_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = E1_AXIS_ENABLE_PIN;
    GPIO_Init(E1_AXIS_ENABLE_PORT, &GPIO_InitStructure);


    disable_x();
    disable_y();
    disable_z();
    disable_e0();
    disable_e1();


    X_STEP_PIN = INVERT_X_STEP_PIN;
    Y_STEP_PIN = INVERT_Y_STEP_PIN;
    Z_STEP_PIN = INVERT_Z_STEP_PIN;
    E0_STEP_PIN = INVERT_E_STEP_PIN;
    E1_STEP_PIN = INVERT_E_STEP_PIN;


//电机驱动定时器
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 0x4000;
    TIM_TimeBaseStructure.TIM_Prescaler = ST_TIMEX_PRESCALER;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE );

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    TIM_Cmd(TIM3, ENABLE);
    ENABLE_STEPPER_DRIVER_INTERRUPT();

#ifdef ADVANCE

#endif

    enable_endstops(1); // Start with endstops active. After homing they can be disabled

}


// Block until all buffered steps are executed
void st_synchronize(void)
{
    while( blocks_queued()) {
        manage_heater();
        manage_inactivity();
        lcd_update();
    }
}



void st_synchronize_no_lcd(void)
{
	while( blocks_queued()) {
	 manage_heater();
  manage_inactivity();
  checkHitEndstops();
		}

}

void st_synchronize_no_lcd_pause_resume(void)
{
	while( blocks_queued()) {
	 manage_heater();
  manage_inactivity();
  checkHitEndstops();
  paueStartFlag = 1;//标志暂停/恢复打印过程
		}
		paueStartFlag = 0;

}
void st_set_position(const long x, const long y, const long z, const long e)
{
    CRITICAL_SECTION_START;
    count_position[X_AXIS] = x;
    count_position[Y_AXIS] = y;
    count_position[Z_AXIS] = z;
    count_position[E_AXIS] = e;
    CRITICAL_SECTION_END;
}

void st_set_e_position(const long e)
{
    CRITICAL_SECTION_START;
    count_position[E_AXIS] = e;
    CRITICAL_SECTION_END;
}

long st_get_position(uint8_t axis)
{
    long count_pos;
    CRITICAL_SECTION_START;
    count_pos = count_position[axis];
    CRITICAL_SECTION_END;
    return count_pos;
}

void finishAndDisableSteppers(void)
{
    st_synchronize();
    disable_x();
    disable_y();
    disable_z();
    disable_e0();
    disable_e1();
}

void quickStop(void)
{
    DISABLE_STEPPER_DRIVER_INTERRUPT();
    while(blocks_queued())
        plan_discard_current_block();
    current_block = NULL;
    ENABLE_STEPPER_DRIVER_INTERRUPT();
}


void digipot_init(void) //Initialize Digipot Motor Current
{   const uint8_t digipot_motor_current[] = DIGIPOT_MOTOR_CURRENT;
    int i;
    for(i = 0; i <= 4; i++)
        digipot_current(i, digipot_motor_current[i]);
}

void digipot_current(uint8_t driver, uint8_t current)
{
    const uint8_t digipot_ch[] = DIGIPOT_CHANNELS;
//	printf("%d:%d\r\n",digipot_ch[driver],current);
    digitalPotWrite(digipot_ch[driver], (uint8_t)current);
}

void digitalPotWrite(uint8_t address, uint8_t value) // From Arduino DigitalPotControl example
{
    /*
        DIGIPOTSS_PIN=1; // take the SS pin low to select the chip
        SPI1_ReadWriteByte(address); //  send in the address and value via SPI:
        SPI1_ReadWriteByte(value);
        DIGIPOTSS_PIN=0; // take the SS pin high to de-select the chip:
    	*/
}

void microstep_init(void)
{   int i;
    for(i = 0; i <= 4; i++) microstep_mode(i, 8);
// for(i=3;i<=4;i++) microstep_mode(i,16);
}

void microstep_ms(uint8_t driver, int8_t ms1, int8_t ms2, int8_t ms3)
{   /*
     if(ms1 > -1) switch(driver)
     {
       case 0:X_MS1_PIN=ms1 ; break;
       case 1:Y_MS1_PIN=ms1 ; break;
       case 2:Z_MS1_PIN=ms1 ; break;
       case 3:E0_MS1_PIN=ms1 ; break;
       case 4:E1_MS1_PIN=ms1 ; break;
    default:  break;
     }
     if(ms2 > -1)
     switch(driver)
     {
       case 0:X_MS2_PIN=ms2 ; break;
       case 1:Y_MS2_PIN=ms2 ; break;
       case 2:Z_MS2_PIN=ms2 ; break;
       case 3:E0_MS2_PIN=ms2 ; break;
       case 4:E1_MS2_PIN=ms2 ; break;
    default:  break;
     }
       if(ms3 > -1) switch(driver)
     {
       case 0:X_MS3_PIN=ms3 ; break;
       case 1:Y_MS3_PIN=ms3 ; break;
       case 2:Z_MS3_PIN=ms3 ; break;
       case 3:E0_MS3_PIN=ms3 ; break;
       case 4:E1_MS3_PIN=ms3 ; break;
    default:  break;
     }
    */
}

void microstep_mode(uint8_t driver, uint8_t stepping_mode)
{   switch(driver)
    {
    case 0:
        subsection_x_value = stepping_mode;
        break;
    case 1:
        subsection_y_value = stepping_mode;
        break;
    case 2:
        subsection_z_value = stepping_mode;
        break;
    case 3:
        subsection_e0_value = stepping_mode;
        break;
    case 4:
        subsection_e1_value = stepping_mode;
        break;
    default:
        break;
    }
    switch(stepping_mode)
    {
    case 1:
        microstep_ms(driver, MICROSTEP1);
        break;
    case 2:
        microstep_ms(driver, MICROSTEP2);
        break;
    case 4:
        microstep_ms(driver, MICROSTEP4);
        break;
    case 8:
        microstep_ms(driver, MICROSTEP8);
        break;
    case 16:
        microstep_ms(driver, MICROSTEP16);
        break;
    case 32:
        microstep_ms(driver, MICROSTEP32);
        break;
    case 64:
        microstep_ms(driver, MICROSTEP64);
        break;
    case 128:
        microstep_ms(driver, MICROSTEP128);
        break;
    default:
        break;
    }
}
void microstep_readings(void)
{
    printf("Motor_Subsection \n");
    printf("X: %d\n", subsection_x_value);
    printf("Y: %d\n", subsection_y_value);
    printf("Z: %d\n", subsection_z_value);
    printf("E0: %d\n", subsection_e0_value);
    printf("E1: %d\n", subsection_e1_value);
}

