#include "Dlion.h"
#include "planner.h"
#include "temperature.h"
#include "ConfigurationStore.h"
#include "bsp_usart.h"







void Config_PrintSettings()
{  // Always have this function, even with EEPROM_SETTINGS disabled, the current values will be shown
    SERIAL_ECHO_START;
    printf("Steps per unit:");
    SERIAL_ECHO_START;
    printf("  M92 X%f",axis_steps_per_unit[0]);
    printf(" Y%f",axis_steps_per_unit[1]);
    printf(" Z%f",axis_steps_per_unit[2]);
    printf(" E%f\r\n",axis_steps_per_unit[3]);

      
    SERIAL_ECHO_START;
    printf("Maximum feedrates (mm/s):\r\n");
    SERIAL_ECHO_START;
    printf("  M203 X%f",max_feedrate[0]);
    printf(" Y%f",max_feedrate[1] ); 
    printf(" Z%f", max_feedrate[2] ); 
    printf(" E%f\r\n", max_feedrate[3]);


    SERIAL_ECHO_START;
    printf("Maximum Acceleration (mm/s2):\r\n");
    SERIAL_ECHO_START;
    printf("  M201 X%ld" ,max_acceleration_units_per_sq_second[0] ); 
    printf(" Y%ld" , max_acceleration_units_per_sq_second[1] ); 
    printf(" Z%ld" ,max_acceleration_units_per_sq_second[2] );
    printf(" E%ld\r\n" ,max_acceleration_units_per_sq_second[3]);
  //  SERIAL_ECHOLN("");
    SERIAL_ECHO_START;
    printf("Acceleration: S=acceleration, T=retract acceleration\r\n");
    SERIAL_ECHO_START;
    printf("  M204 S%f",acceleration ); 
    printf(" T%f\r\n" ,retract_acceleration);
 ///   SERIAL_ECHOLN("");

    SERIAL_ECHO_START;
    printf("Advanced variables: S=Min feedrate (mm/s), T=Min travel feedrate (mm/s), B=minimum segment time (ms), X=maximum XY jerk (mm/s),  Z=maximum Z jerk (mm/s),  E=maximum E jerk (mm/s)\r\n");
    SERIAL_ECHO_START;
    printf("  M205 S%f",minimumfeedrate ); 
    printf(" T%f" ,mintravelfeedrate ); 
    printf(" B%ld" ,minsegmenttime ); 
    printf(" X%f" ,max_xy_jerk ); 
    printf(" Z%f" ,max_z_jerk);
    printf(" E%f\r\n" ,max_e_jerk);
   // SERIAL_ECHOLN(""); 

    SERIAL_ECHO_START;
    printf("Home offset (mm):\r\n");
    SERIAL_ECHO_START;
    printf("  M206 X%f",add_homeing[0] );
    printf(" Y%f" ,add_homeing[1] );
    printf(" Z%f" ,add_homeing[2] );
  //  SERIAL_ECHOLN("");
#ifdef PIDTEMP
    SERIAL_ECHO_START;
    printf("PID settings:\r\n");
    SERIAL_ECHO_START;
    printf("   M301 P%f",Kp); 
    printf(" I%f" ,unscalePID_i(Ki)); 
    printf(" D%f\r\n" ,unscalePID_d(Kd));
  //  SERIAL_ECHOLN(""); 
#endif
} 

