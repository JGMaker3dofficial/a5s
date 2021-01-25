

#include "temperature.h"
#include "Dlion.h"
//#include "ultralcd.h"
#include "bsp_timer.h"
#include "watchdog.h"
#include "adc.h"
#include "bsp_usart.h"
#include "planner.h"
#include "thermistortables.h"
#include "app_gui.h"

//===========================================================================
//=============================public variables============================
//===========================================================================
int target_temperature[EXTRUDERS] = { 0 };
int target_temperature_bed = 0;
int current_temperature_raw[EXTRUDERS] = { 0 };
float current_temperature[EXTRUDERS] = { 0 };
int current_temperature_bed_raw = 0;
float current_temperature_bed = 0;

#ifdef PIDTEMP
float Kp = DEFAULT_Kp;
float Ki = (DEFAULT_Ki*PID_dT);
float Kd = (DEFAULT_Kd / PID_dT);
#ifdef PID_ADD_EXTRUSION_RATE
float Kc = DEFAULT_Kc;
#endif
#endif //PIDTEMP

#ifdef PIDTEMPBED
float bedKp = DEFAULT_bedKp;
float bedKi = (DEFAULT_bedKi*PID_dT);
float bedKd = (DEFAULT_bedKd / PID_dT);
#endif //PIDTEMPBED


//===========================================================================
//=============================private variables============================
//===========================================================================
static volatile u8 temp_meas_ready = FALSE;
static volatile u8 rccx[3] = {0};
#ifdef PIDTEMP
//static cannot be external:
static float temp_iState[EXTRUDERS] = { 0 };
static float temp_dState[EXTRUDERS] = { 0 };
static float pTerm[EXTRUDERS];
static float iTerm[EXTRUDERS];
static float dTerm[EXTRUDERS];
//int output;
static float pid_error[EXTRUDERS];
static float temp_iState_min[EXTRUDERS];
static float temp_iState_max[EXTRUDERS];
// static float pid_input[EXTRUDERS];
// static float pid_output[EXTRUDERS];
static u8 pid_reset[EXTRUDERS];
#endif //PIDTEMP
#ifdef PIDTEMPBED
//static cannot be external:
static float temp_iState_bed = { 0 };
static float temp_dState_bed = { 0 };
static float pTerm_bed;
static float iTerm_bed;
static float dTerm_bed;
//int output;
static float pid_error_bed;
static float temp_iState_min_bed;
static float temp_iState_max_bed;
#else //PIDTEMPBED
static unsigned long  previous_millis_bed_heater;
#endif //PIDTEMPBED
static unsigned char soft_pwm[EXTRUDERS];
static unsigned char soft_pwm_bed;

#if (defined(EXTRUDER_0_AUTO_FAN_PIN) && EXTRUDER_0_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_1_AUTO_FAN_PIN) && EXTRUDER_1_AUTO_FAN_PIN > -1) || \
    (defined(EXTRUDER_2_AUTO_FAN_PIN) && EXTRUDER_2_AUTO_FAN_PIN > -1)
static unsigned long extruder_autofan_last_check;
#endif

#if EXTRUDERS > 3
# error Unsupported number of extruders
#elif EXTRUDERS > 2
#define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2, v3 }
#elif EXTRUDERS > 1
#define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1, v2 }
#else
#define ARRAY_BY_EXTRUDERS(v1, v2, v3) { v1 }
#endif

// Init min and max temp with extreme values to prevent FALSE errors during startup
static int minttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_RAW_LO_TEMP, HEATER_1_RAW_LO_TEMP, HEATER_2_RAW_LO_TEMP );
static int maxttemp_raw[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_RAW_HI_TEMP, HEATER_1_RAW_HI_TEMP, HEATER_2_RAW_HI_TEMP );
static int minttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS( 0, 0, 0 );
static int maxttemp[EXTRUDERS] = ARRAY_BY_EXTRUDERS( 16383, 16383, 16383 );
//static int bed_minttemp_raw = HEATER_BED_RAW_LO_TEMP; /* No bed mintemp error implemented?!? */
#ifdef BED_MAXTEMP
static int bed_maxttemp_raw = HEATER_BED_RAW_HI_TEMP;
#endif
static void *heater_ttbl_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( (void *)HEATER_0_TEMPTABLE, (void *)HEATER_1_TEMPTABLE, (void *)HEATER_2_TEMPTABLE );
static uint8_t heater_ttbllen_map[EXTRUDERS] = ARRAY_BY_EXTRUDERS( HEATER_0_TEMPTABLE_LEN, HEATER_1_TEMPTABLE_LEN, HEATER_2_TEMPTABLE_LEN );

static float analog2temp(int raw, uint8_t e);
static float analog2tempBed(int raw);
static void updateTemperaturesFromRawValues(void);

#ifdef WATCH_TEMP_PERIOD
int watch_start_temp[EXTRUDERS] = ARRAY_BY_EXTRUDERS(0, 0, 0);
unsigned long watchmillis[EXTRUDERS] = ARRAY_BY_EXTRUDERS(0, 0, 0);
#endif //WATCH_TEMP_PERIOD


//===========================================================================
//=============================   functions      ============================
//===========================================================================

void PID_autotune(float temp, int extruder, int ncycles)
{
    float input = 0.0;
    int cycles = 0;
    u8 heating = TRUE;

    unsigned long temp_millis = millis();
    unsigned long t1 = temp_millis;
    unsigned long t2 = temp_millis;
    long t_high = 0;
    long t_low = 0;

    long bias, d;
    float Ku, Tu;
    float Kp, Ki, Kd;
    float max = 0, min = 10000;

    if (extruder > EXTRUDERS)
    {
        printf("PID Autotune failed. Bad extruder number.");
        return;
    }
    printf("PID Autotune start");

    disable_heater(); // switch off all heaters.

    if (extruder < 0)
    {
        soft_pwm_bed = (MAX_BED_POWER) / 2;
        bias = d = (MAX_BED_POWER) / 2;
    }
    else
    {
        soft_pwm[extruder] = (PID_MAX) / 2;
        bias = d = (PID_MAX) / 2;
    }




    for(;;) {

        if(temp_meas_ready == TRUE)	  // temp sample ready
        {
            updateTemperaturesFromRawValues();

            input = (extruder < 0) ? current_temperature_bed : current_temperature[extruder]; //获取采样值

            max = max(max, input);
            min = min(min, input);

            if(heating == TRUE && input > temp)
            {
                if(millis() - t2 > 5000)
                {
                    heating = FALSE;
                    if (extruder < 0)
                        soft_pwm_bed = (bias - d) >> 1;
                    else
                        soft_pwm[extruder] = (bias - d) >> 1;
                    t1 = millis();
                    t_high = t1 - t2;
                    max = temp;
                }
            }
            if(heating == FALSE && input < temp)
            {
                if(millis() - t1 > 5000)
                {
                    heating = TRUE;
                    t2 = millis();
                    t_low = t2 - t1;
                    if(cycles > 0)
                    {
                        bias += (d * (t_high - t_low)) / (t_low + t_high);
                        bias = constrain(bias, 20, (extruder < 0 ? (MAX_BED_POWER) : (PID_MAX)) - 20);
                        if(bias > (extruder < 0 ? (MAX_BED_POWER) : (PID_MAX)) / 2) d = (extruder < 0 ? (MAX_BED_POWER) : (PID_MAX)) - 1 - bias;
                        else d = bias;

                        printf(" bias: %ld", bias);
                        printf(" d: %ld", d);
                        printf(" min: %f", min);
                        printf(" max: %f", max);
                        if(cycles > 2)
                        {
                            Ku = (4.0 * d) / (3.14159 * (max - min) / 2.0);
                            Tu = ((float)(t_low + t_high) / 1000.0);
                            printf(" Ku: %f", Ku);
                            printf(" Tu: %f", Tu);
                            Kp = 0.6 * Ku;
                            Ki = 2 * Kp / Tu;
                            Kd = Kp * Tu / 8;
                            printf(" Clasic PID ");
                            printf(" Kp: %f", Kp);
                            printf(" Ki: %f", Ki);
                            printf(" Kd: %f", Kd);
                        }
                    }
                    if (extruder < 0)
                        soft_pwm_bed = (bias + d) >> 1;
                    else
                        soft_pwm[extruder] = (bias + d) >> 1;
                    cycles++;
                    min = temp;
                }
            }
        }
        if(input > (temp + 20))
        {
            printf("PID Autotune failed! Temperature to high");
            return;
        }
        if(millis() - temp_millis > 2000)
        {
            int p;
            if (extruder < 0)
            {
                p = soft_pwm_bed;
                printf("ok B:");
            }
            else
            {
                p = soft_pwm[extruder];
                printf("ok T:");
            }

            printf("%f @:%d", input, p);
            temp_millis = millis();
        }
        if(((millis() - t1) + (millis() - t2)) > (10L * 60L * 1000L * 2L))
        {
            printf("PID Autotune failed! timeout");
            return;
        }
        if(cycles > ncycles) {
            printf("PID Autotune finished ! Place the Kp, Ki and Kd constants in the configuration.h");
            return;
        }
        lcd_update();
    }
}

void updatePID(void)
{
#ifdef PIDTEMP
    int e;
    for(e = 0; e < EXTRUDERS; e++) {
        temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / Ki;
    }
#endif
#ifdef PIDTEMPBED
    temp_iState_max_bed = PID_INTEGRAL_DRIVE_MAX / bedKi;
#endif
}

int getHeaterPower(int heater) {
    if (heater < 0)
        return soft_pwm_bed;
    return soft_pwm[heater];
}

void manage_heater(void)
{
    float pid_input;
    float pid_output;
    int e;
    if(temp_meas_ready != TRUE)   //better readability
        return;

    updateTemperaturesFromRawValues();

    for(e = 0; e < EXTRUDERS; e++)
    {
#ifdef PIDTEMP
        pid_input = current_temperature[e];

#ifndef PID_OPENLOOP
        pid_error[e] = target_temperature[e] - pid_input;
        if(pid_error[e] > PID_FUNCTIONAL_RANGE) {
            pid_output = BANG_MAX;
            pid_reset[e] = TRUE;
        }
        else if(pid_error[e] < -PID_FUNCTIONAL_RANGE || target_temperature[e] == 0) {
            pid_output = 0;
            pid_reset[e] = TRUE;
        }
        else {
            if(pid_reset[e] == TRUE) {
                temp_iState[e] = 0.0;
                pid_reset[e] = FALSE;
            }
            pTerm[e] = Kp * pid_error[e];
            temp_iState[e] += pid_error[e];
            temp_iState[e] = constrain(temp_iState[e], temp_iState_min[e], temp_iState_max[e]);
            iTerm[e] = Ki * temp_iState[e];

            //K1 defined in Configuration.h in the PID settings
#define K2 (1.0-K1)
            dTerm[e] = (Kd * (pid_input - temp_dState[e])) * K2 + (K1 * dTerm[e]);
            temp_dState[e] = pid_input;

            pid_output = constrain(pTerm[e] + iTerm[e] - dTerm[e], 0, PID_MAX);
        }
#else
        pid_output = constrain(target_temperature[e], 0, PID_MAX);
#endif //PID_OPENLOOP

#ifdef PID_DEBUG
        printf(" PIDDEBUG %d", e);
        printf(": Input %f", pid_input);
        //  printf(pid_input);
        printf(" Output %f", pid_output);
        //  printf(pid_output);
        printf(" pTerm %f", pTerm[e]);
        //  printf(pTerm[e]);
        printf(" iTerm %f", iTerm[e]);
        //  printf(iTerm[e]);
        printf(" dTerm %f", dTerm[e]);
        //  printf(dTerm[e]);
        printf("\n\r");
#endif //PID_DEBUG

#else /* PID off */
        pid_output = 0;
        if(current_temperature[e] < target_temperature[e]) {
            pid_output = PID_MAX;
        }
#endif

        // Check if temperature is within the correct range
        if((current_temperature[e] > minttemp[e]) && (current_temperature[e] < maxttemp[e]))
        {
            soft_pwm[e] = (int)pid_output >> 1;
        }
        else {
            soft_pwm[e] = 0;
        }

#ifdef WATCH_TEMP_PERIOD	 // 加热前检查
        if(watchmillis[e] && millis() - watchmillis[e] > WATCH_TEMP_PERIOD)
        {
            if(degHotend(e) < watch_start_temp[e] + WATCH_TEMP_INCREASE)
            {
                setTargetHotend(0, e);
                LCD_MESSAGEPGM("Heating failed");
                SERIAL_ECHO_START;
                SERIAL_ECHOLN("Heating failed");
            } else {
                watchmillis[e] = 0;
            }
        }
#endif

    } // End extruder for loop

#ifndef PIDTEMPBED
    if(millis() - previous_millis_bed_heater < BED_CHECK_INTERVAL)
        return;
    previous_millis_bed_heater = millis();
#endif

#if TEMP_SENSOR_BED != 0

#ifdef PIDTEMPBED
    pid_input = current_temperature_bed;
#ifndef PID_OPENLOOP
    pid_error_bed = target_temperature_bed - pid_input;
    pTerm_bed = bedKp * pid_error_bed;
    temp_iState_bed += pid_error_bed;
    temp_iState_bed = constrain(temp_iState_bed, temp_iState_min_bed, temp_iState_max_bed);
    iTerm_bed = bedKi * temp_iState_bed;

    //K1 defined in Configuration.h in the PID settings
#define K2 (1.0-K1)
    dTerm_bed = (bedKd * (pid_input - temp_dState_bed)) * K2 + (K1 * dTerm_bed);
    temp_dState_bed = pid_input;

    pid_output = constrain(pTerm_bed + iTerm_bed - dTerm_bed, 0, MAX_BED_POWER);

#else
    pid_output = constrain(target_temperature_bed, 0, MAX_BED_POWER);
#endif //PID_OPENLOOP

    if((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP))
    {
        soft_pwm_bed = (int)pid_output >> 1;
    }
    else {
        soft_pwm_bed = 0;
    }

#elif !defined(BED_LIMIT_SWITCHING)
    // Check if temperature is within the correct range
    if((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP))
    {
        if(current_temperature_bed >= target_temperature_bed)
        {
            soft_pwm_bed = 0;
        }
        else
        {
            soft_pwm_bed = MAX_BED_POWER >> 1;
        }
    }
    else
    {
        soft_pwm_bed = 0;
        HEATER_BED_PIN = 0;
    }
#else //#ifdef BED_LIMIT_SWITCHING
    // Check if temperature is within the correct band
    if((current_temperature_bed > BED_MINTEMP) && (current_temperature_bed < BED_MAXTEMP))
    {
        if(current_temperature_bed > target_temperature_bed + BED_HYSTERESIS)
        {
            soft_pwm_bed = 0;
        }
        else if(current_temperature_bed <= target_temperature_bed - BED_HYSTERESIS)
        {
            soft_pwm_bed = MAX_BED_POWER >> 1;
        }
    }
    else
    {
        soft_pwm_bed = 0;
        HEATER_BED_PIN = 0;
    }

#endif
#endif
}


static float analog2temp(int raw, uint8_t e)
{
    if(e >= EXTRUDERS)
    {
        SERIAL_ERROR_START;
        printf("%d", e);
        printf(" - Invalid extruder number !");
        kill();
    }


    if(heater_ttbl_map[e] != NULL)
    {
        float celsius = 0;
        uint8_t i;
        short (*tt)[][2] = (short (*)[][2])(heater_ttbl_map[e]);

        for (i = 1; i < heater_ttbllen_map[e]; i++)
        {
            if ((*tt)[i][0] > raw)
            {
                celsius = (*tt)[i - 1][1] +
                          (raw - (*tt)[i - 1][0]) *
                          (float)((*tt)[i][1] - (*tt)[i - 1][1]) /
                          (float)((*tt)[i][0] - (*tt)[i - 1][0]);
                break;
            }
        }

        // Overflow: Set to last value in the table
        if (i == heater_ttbllen_map[e]) celsius = (*tt)[i - 1][1];

        return celsius;
    }
    return 0;
    //return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
}

// Derived from RepRap FiveD extruder::getTemperature()
// For bed temperature measurement.
static float analog2tempBed(int raw) {
#ifdef BED_USES_THERMISTOR
    float celsius = 0;
    u8 i;

    for (i = 1; i < BEDTEMPTABLE_LEN; i++)
    {
        if (BEDTEMPTABLE[i][0] > raw)
        {
            celsius  = BEDTEMPTABLE[i - 1][1] +
                       (raw - BEDTEMPTABLE[i - 1][0]) *
                       (float)(BEDTEMPTABLE[i][1] - BEDTEMPTABLE[i - 1][1]) /
                       (float)(BEDTEMPTABLE[i][0] - BEDTEMPTABLE[i - 1][0]);
            break;
        }
    }

    // Overflow: Set to last value in the table
    if (i == BEDTEMPTABLE_LEN) celsius = BEDTEMPTABLE[i - 1][1];

    return celsius;
#elif defined BED_USES_AD595
    return ((raw * ((5.0 * 100.0) / 1024.0) / OVERSAMPLENR) * TEMP_SENSOR_AD595_GAIN) + TEMP_SENSOR_AD595_OFFSET;
#else
    return 0;
#endif
}

/* Called to get the raw values into the the actual temperatures. The raw values are created in interrupt context,
    and this function is called from normal context as it is too slow to run in interrupts and will block the stepper routine otherwise */
static void updateTemperaturesFromRawValues(void)
{   u8 e;
    for(e = 0; e < EXTRUDERS; e++)
    {
        current_temperature[e] = analog2temp(current_temperature_raw[e], e);
    }
    current_temperature_bed = analog2tempBed(current_temperature_bed_raw);

    //Reset the watchdog after we know we have a temperature measurement.
    watchdog_reset();

    CRITICAL_SECTION_START;
    temp_meas_ready = FALSE;
    CRITICAL_SECTION_END;
}

void tp_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    NVIC_InitTypeDef NVIC_InitStructure;
    // Finish init of mult extruder arrays
    int e;
    //	printf("tpint!\n\r");
    for( e = 0; e < EXTRUDERS; e++)
    {   // populate with the first value
        maxttemp[e] = maxttemp[0];
#ifdef PIDTEMP
        temp_iState_min[e] = 0.0;
        temp_iState_max[e] = PID_INTEGRAL_DRIVE_MAX / Ki;
#endif //PIDTEMP
#ifdef PIDTEMPBED
        temp_iState_min_bed = 0.0;
        temp_iState_max_bed = PID_INTEGRAL_DRIVE_MAX / bedKi;
#endif //PIDTEMPBED
    }

    POWER_IO_RCC_APBX_PERIPH_CLK_CMD;
    POWER_IO_REMAP;

//大功率控制开关，热挤出头+热床+风扇
#if defined(HEATER_0_PIN)
    GPIO_InitStructure.GPIO_Pin = HEATER0_EXT0_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HEATE_EXT0_PORT, &GPIO_InitStructure);
	GPIO_SetBits(HEATE_EXT0_PORT,HEATER0_EXT0_PIN);
#endif

#if defined(HEATER_BED_PIN)
    GPIO_InitStructure.GPIO_Pin = HEATE_BED_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HEATE_BED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(HEATE_BED_PORT,HEATE_BED_PIN);
#endif



#if defined(FAN_PIN)
    GPIO_InitStructure.GPIO_Pin = CONT_FAN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CONT_FAN_PORT, &GPIO_InitStructure);
	GPIO_SetBits(CONT_FAN_PORT,CONT_FAN_PIN);
#endif


#if defined(E0_FAN)
    GPIO_InitStructure.GPIO_Pin = E0_FAN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(E0_FAN_PORT, &GPIO_InitStructure);
	GPIO_SetBits(E0_FAN_PORT,E0_FAN_PIN);
#endif

#if defined(HEATER_1_PIN)
#endif

    // Set analog inputs
    //见adc.c 文件 void Adc_Init(void)
    Adc_Init();


    // Use timer5 for sofe pwm control
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);


    TIM_TimeBaseStructure.TIM_Period = 9;//100us中断一次
    TIM_TimeBaseStructure.TIM_Prescaler = 719;//100Khz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE );


    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM5, ENABLE);



    // Use timer6 for temperature measurement
    // Interleave temperature interrupt with millies interrupt
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);


    TIM_TimeBaseStructure.TIM_Period = 9;
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE );


    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM6, ENABLE);
    // Wait for temperature measurement to settle
    bsp_DelayMS(250);

#ifdef HEATER_0_MINTEMP
    minttemp[0] = HEATER_0_MINTEMP;
    while(analog2temp(minttemp_raw[0], 0) < HEATER_0_MINTEMP) {
#if HEATER_0_RAW_LO_TEMP < HEATER_0_RAW_HI_TEMP
        minttemp_raw[0] += OVERSAMPLENR;
#else
        minttemp_raw[0] -= OVERSAMPLENR;
#endif
    }
#endif //MINTEMP

#ifdef HEATER_0_MAXTEMP
    maxttemp[0] = HEATER_0_MAXTEMP;
    while(analog2temp(maxttemp_raw[0], 0) > HEATER_0_MAXTEMP) {
#if HEATER_0_RAW_LO_TEMP < HEATER_0_RAW_HI_TEMP
        maxttemp_raw[0] -= OVERSAMPLENR;
#else
        maxttemp_raw[0] += OVERSAMPLENR;
#endif
    }
#endif //MAXTEMP

#if (EXTRUDERS > 1) && defined(HEATER_1_MINTEMP)
    minttemp[1] = HEATER_1_MINTEMP;
    while(analog2temp(minttemp_raw[1], 1) < HEATER_1_MINTEMP) {
#if HEATER_1_RAW_LO_TEMP < HEATER_1_RAW_HI_TEMP
        minttemp_raw[1] += OVERSAMPLENR;
#else
        minttemp_raw[1] -= OVERSAMPLENR;
#endif
    }
#endif // MINTEMP 1
#if (EXTRUDERS > 1) && defined(HEATER_1_MAXTEMP)
    maxttemp[1] = HEATER_1_MAXTEMP;
    while(analog2temp(maxttemp_raw[1], 1) > HEATER_1_MAXTEMP) {
#if HEATER_1_RAW_LO_TEMP < HEATER_1_RAW_HI_TEMP
        maxttemp_raw[1] -= OVERSAMPLENR;
#else
        maxttemp_raw[1] += OVERSAMPLENR;
#endif
    }
#endif //MAXTEMP 1

#if (EXTRUDERS > 2) && defined(HEATER_2_MINTEMP)
    minttemp[2] = HEATER_2_MINTEMP;
    while(analog2temp(minttemp_raw[2], 2) < HEATER_2_MINTEMP) {
#if HEATER_2_RAW_LO_TEMP < HEATER_2_RAW_HI_TEMP
        minttemp_raw[2] += OVERSAMPLENR;
#else
        minttemp_raw[2] -= OVERSAMPLENR;
#endif
    }
#endif //MINTEMP 2
#if (EXTRUDERS > 2) && defined(HEATER_2_MAXTEMP)
    maxttemp[2] = HEATER_2_MAXTEMP;
    while(analog2temp(maxttemp_raw[2], 2) > HEATER_2_MAXTEMP) {
#if HEATER_2_RAW_LO_TEMP < HEATER_2_RAW_HI_TEMP
        maxttemp_raw[2] -= OVERSAMPLENR;
#else
        maxttemp_raw[2] += OVERSAMPLENR;
#endif
    }
#endif //MAXTEMP 2

#ifdef BED_MINTEMP
    /* No bed MINTEMP error implemented?!? */ /*
    while(analog2tempBed(bed_minttemp_raw) < BED_MINTEMP) {
    #if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
        bed_minttemp_raw += OVERSAMPLENR;
    #else
        bed_minttemp_raw -= OVERSAMPLENR;
    #endif
      }
      */
#endif //BED_MINTEMP
#ifdef BED_MAXTEMP
    while(analog2tempBed(bed_maxttemp_raw) > BED_MAXTEMP)
    {
#if HEATER_BED_RAW_LO_TEMP < HEATER_BED_RAW_HI_TEMP
        bed_maxttemp_raw -= OVERSAMPLENR;
#else
        bed_maxttemp_raw += OVERSAMPLENR;
#endif
    }
#endif //BED_MAXTEMP
//	printf("end!\n\r");
}

void setWatch(void)
{
#ifdef WATCH_TEMP_PERIOD
    for (int e = 0; e < EXTRUDERS; e++)
    {
        if(degHotend(e) < degTargetHotend(e) - (WATCH_TEMP_INCREASE * 2))
        {
            watch_start_temp[e] = degHotend(e);
            watchmillis[e] = millis();
        }
    }
#endif
}


void disable_heater(void)
{   int i;
   // E0_FAN = 0;
    for(i = 0; i < EXTRUDERS; i++)
        setTargetHotend(0, i);
    setTargetBed(0);
#if defined(TEMP_0_PIN)
    target_temperature[0] = 0;
    soft_pwm[0] = 0;
#if defined(HEATER_0_PIN)
    HEATER_0_PIN = 0;
#endif
#endif

#if defined(TEMP_1_PIN)
    target_temperature[1] = 0;
    soft_pwm[1] = 0;
#if defined(HEATER_1_PIN)
    HEATER_1_PIN = 0;
#endif
#endif

#if defined(TEMP_2_PIN)
    target_temperature[2] = 0;
    soft_pwm[2] = 0;
#if defined(HEATER_2_PIN)
    HEATER_2_PIN = 0;
#endif
#endif

#if defined(TEMP_BED_PIN)
    target_temperature_bed = 0;
    soft_pwm_bed = 0;
#if defined(HEATER_BED_PIN)
    HEATER_BED_PIN = 0;
#endif
#endif
}

void max_temp_error(uint8_t e)
{
    disable_heater();
    if(IsStopped() == FALSE) {
        SERIAL_ERROR_START;
        printf("%d", e);
        printf(": Extruder switched off. MAXTEMP triggered !");
        // LCD_ALERTMESSAGEPGM("Err: MAXTEMP");	 /////////////////////////////////
    }
#ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
    Stop();
#endif
}

void min_temp_error(uint8_t e)
{
    disable_heater();
    if(IsStopped() == FALSE) {
        SERIAL_ERROR_START;
        printf("%d", e);
        printf(": Extruder switched off. MINTEMP triggered !");
        //LCD_ALERTMESSAGEPGM("Err: MINTEMP"); //////////////////////////////////
    }
#ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
    Stop();
#endif
}

void bed_max_temp_error(void)
{
#ifdef HEATER_BED_PIN
    HEATER_BED_PIN = 0;
#endif
    if(IsStopped() == FALSE) {
        SERIAL_ERROR_START;
        printf("Temperature heated bed switched off. MAXTEMP triggered !!");
        //  LCD_ALERTMESSAGEPGM("Err: MAXTEMP BED");///////////////////////////////////////////
    }
#ifndef BOGUS_TEMPERATURE_FAILSAFE_OVERRIDE
    Stop();
#endif
}

void TIM5_IRQHandler(void)
{
    static unsigned char time_count[3] = {0};
    u8 i;
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
        time_count[EXT0]++;
        time_count[BED]++;
        time_count[FAN]++;
        for( i = 0 ; i < 3 ; i++ )
        {
            if( time_count[i] >= DEFUALT_PERRIOD )
            {
                time_count[i] = 0;
            }
        }

        if( time_count[EXT0] >= rccx[EXT0] )
        {
            HEATER_0_PIN = 1;
        }
        else HEATER_0_PIN = 0;
#ifdef HEATER_BED_PIN
        if( time_count[BED] >=  rccx[BED] )
        {
            HEATER_BED_PIN = 1;
        }
        else HEATER_BED_PIN = 0;
#endif

#ifdef FAN_PIN
        if( time_count[FAN] >= rccx[FAN] )
        {
            FAN_PIN = 1;
        }
        else FAN_PIN = 0;
#endif

    }
}


// Timer 6 is shared with millies
void TIM6_IRQHandler(void)
{
    //these variables are only accesible from the ISR, but static, so they don't loose their value
    static unsigned char temp_count = 0;
    static unsigned long raw_temp_0_value = 0;
#if EXTRUDERS > 1
    static unsigned long raw_temp_1_value = 0;
#endif
#if EXTRUDERS > 2
    static unsigned long raw_temp_2_value = 0;
#endif
    static unsigned long raw_temp_bed_value = 0;
    static unsigned char temp_state = 0;

    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

        rccx[EXT0] = (uint16_t)soft_pwm[0];
#if EXTRUDERS > 1
        HEATER_1_PIN = (uint16_t)soft_pwm[1];
#endif
#if EXTRUDERS > 2
        HEATER_2_PIN = (uint16_t)soft_pwm[2];
#endif

#ifdef HEATER_BED_PIN
        rccx[BED] = (uint16_t)soft_pwm_bed;
//	printf(" soft_pwm_bed=%d \n\r", soft_pwm_bed );
#endif
#ifdef FAN_PIN
        rccx[FAN] = (uint16_t)fanSpeed;
//	 printf ("fanspeed:%d\n\r",fanSpeed);
#endif



        switch(temp_state) {
        case 0:
#if defined(TEMP_0_PIN)
            raw_temp_0_value += TEMP_0_PIN;
#endif
            temp_state = 1;
            break;
        case 1:
#if defined(TEMP_1_PIN)
            raw_temp_1_value += TEMP_1_PIN;
#endif
            temp_state = 2;
            break;
        case 2:
#if defined(TEMP_2_PIN)
            raw_temp_2_value += TEMP_2_PIN;
#endif
            temp_state = 3;
            break;
        case 3:
#if defined(TEMP_BED_PIN)
            raw_temp_bed_value += TEMP_BED_PIN; //28ms
#endif
            //  printf("%ld\r\n", millis());
            temp_state = 0;
            temp_count++;
            break;

            //  default:
//      SERIAL_ERROR_START;
//      SERIAL_ERRORLNPGM("Temp measurement error!");
//      break;
        }

        //lcd_buttons_update();	///////////////////////////////////

        if(temp_count >= 16) // 4 ms * 16 = 64ms.
        {   //
            if (!temp_meas_ready) //Only update the raw values if they have been read. Else we could be updating them during reading.
            {
                current_temperature_raw[0] = raw_temp_0_value;
#if EXTRUDERS > 1
                current_temperature_raw[1] = raw_temp_1_value;
#endif
#if EXTRUDERS > 2
                current_temperature_raw[2] = raw_temp_2_value;
#endif
                current_temperature_bed_raw = raw_temp_bed_value;
                //    printf("\r\n ex0:%d\r\n",current_temperature_raw[0]);
                //	  printf("\r\n bed:%d\r\n",current_temperature_bed_raw);
            }

            temp_meas_ready = TRUE;
            temp_count = 0;
            raw_temp_0_value = 0;
#if EXTRUDERS > 2
            raw_temp_1_value = 0;
#endif
#if EXTRUDERS > 2
            raw_temp_2_value = 0;
#endif
            raw_temp_bed_value = 0;
#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
            if(current_temperature_raw[0] <= maxttemp_raw[0]) {
                max_temp_error(0);
            }
#else
            if(current_temperature_raw[0] >= maxttemp_raw[0]) {
                max_temp_error(0);
            }
#endif

#if HEATER_0_RAW_LO_TEMP > HEATER_0_RAW_HI_TEMP
            if(current_temperature_raw[0] >= minttemp_raw[0]) {
                min_temp_error(0);
            }
#else
            if(current_temperature_raw[0] <= minttemp_raw[0]) {
                min_temp_error(0);
            }
#endif


#if EXTRUDERS > 1
#if HEATER_1_RAW_LO_TEMP > HEATER_1_RAW_HI_TEMP
            if(current_temperature_raw[1] <= maxttemp_raw[1]) {
                max_temp_error(1);
            }
#else
            if(current_temperature_raw[1] >= maxttemp_raw[1]) {
                max_temp_error(1);
            }
#endif
#if HEATER_1_RAW_LO_TEMP > HEATER_1_RAW_HI_TEMP
            if(current_temperature_raw[1] >= minttemp_raw[1]) {
                min_temp_error(1);
            }
#else
            if(current_temperature_raw[1] <= minttemp_raw[1]) {
                min_temp_error(1);
            }
#endif
#endif


#if EXTRUDERS > 2
#if HEATER_2_RAW_LO_TEMP > HEATER_2_RAW_HI_TEMP
            if(current_temperature_raw[2] <= maxttemp_raw[2]) {
                max_temp_error(2);
            }
#else
            if(current_temperature_raw[2] >= maxttemp_raw[2]) {
                max_temp_error(2);
            }
#endif

#if HEATER_2_RAW_LO_TEMP > HEATER_2_RAW_HI_TEMP
            if(current_temperature_raw[2] >= minttemp_raw[2]) {
                min_temp_error(2);
            }
#else
            if(current_temperature_raw[2] <= minttemp_raw[2]) {
                min_temp_error(2);
            }
#endif

#endif


            /* No bed MINTEMP error? */
#if defined(BED_MAXTEMP) && (TEMP_SENSOR_BED != 0)
# if HEATER_BED_RAW_LO_TEMP > HEATER_BED_RAW_HI_TEMP
            if(current_temperature_bed_raw <= bed_maxttemp_raw) {
                target_temperature_bed = 0;
                bed_max_temp_error();
            }
#else
            if(current_temperature_bed_raw >= bed_maxttemp_raw) {
                target_temperature_bed = 0;
                bed_max_temp_error();
            }
#endif
#endif
        }
    }

}

#ifdef PIDTEMP
// Apply the scale factors to the PID values


float scalePID_i(float i)
{
    return i * PID_dT;
}

float unscalePID_i(float i)
{
    return i / PID_dT;
}

float scalePID_d(float d)
{
    return d / PID_dT;
}

float unscalePID_d(float d)
{
    return d * PID_dT;
}

#endif //PIDTEMP
float degHotend(u8 extruder)
{
    return current_temperature[extruder];
}

float degBed(void)
{
    return current_temperature_bed;
}


float degTargetHotend(u8 extruder)
{
    return target_temperature[extruder];
}

float degTargetBed(void)
{
    return target_temperature_bed;
}

void setTargetHotend(const float celsius, uint8_t extruder)
{
//    if (celsius == 0) E0_FAN = 0;
//    else E0_FAN = 1;
    if(celsius > HEATER_0_MAXTEMP)
    {   target_temperature[extruder] = HEATER_0_MAXTEMP;
    }
    else
    {   target_temperature[extruder] = celsius;
    }
}

void setTargetBed(const float celsius)
{   if(celsius > BED_MAXTEMP)
    {   target_temperature_bed = BED_MAXTEMP;
    }
    else
    {
        target_temperature_bed = celsius;
    }
}

u8 isHeatingHotend(u8 extruder)
{
    return target_temperature[extruder] > current_temperature[extruder];
}

u8 isHeatingBed(void)
{
    return target_temperature_bed > current_temperature_bed;
}

u8 isCoolingHotend(u8 extruder)
{
    return target_temperature[extruder] < current_temperature[extruder];
}

u8 isCoolingBed(void)
{
    return target_temperature_bed < current_temperature_bed;
}
void autotempShutdown(void)
{
#ifdef AUTOTEMP
    if(autotemp_enabled)
    {
        autotemp_enabled = FALSE;
        if(degTargetHotend(active_extruder) > autotemp_min)
            setTargetHotend(0, active_extruder);
    }
#endif
}
