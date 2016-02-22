/* 
 * File:   StateMachine.h
 * Author: Luke
 *
 * Created on 02 February 2016, 19:54
 */

#ifndef debuggersonpi_pic_statemachine_h
#define debuggersonpi_pic_statemachine_h

/////////////////////////////////////INCLUDES///////////////////////////////////

#include "xc.h"
#include "OpenLoop.h"

#include "spi.h"
#include "states.h"
#include "pid.h"

/////////////////////////////////////DEFINES////////////////////////////////////

//////////////////////////////////GLOBAL VARIABLES//////////////////////////////

//Enumerated Type for various states
typedef enum {
    //***********************************************************************//
    //Motor Driving States
    STOPPED         = 0x80, //Stationary
    
    //Open Loop States        
    OL_FORWARD      = 0x81, //Open Loop Forward drive
    OL_LEFT         = 0x82, //Open Loop Forwards Left turn (90)
    OL_RIGHT        = 0x83, //Open Loop Forwards Right turn (90)
    OL_REVERSE      = 0x84, //Open Loop Reverse
    OL_REV_LEFT     = 0x85, //Open Loop Reverse Left Turn (90)
    OL_REV_RIGHT    = 0x86, //Open Loop Reverse Right Turn (90)
            
    //Closed Loop with Compass states
    COMP_FORWARD    = 0x87, //Closed Loop With Compass Forward drive
    COMP_LEFT       = 0x88, //Closed Loop With Compass Forwards Left turn (90)
    COMP_RIGHT      = 0x89, //Closed Loop With Compass Forwards Right turn (90)
    COMP_REVERSE    = 0x8A, //Closed Loop With Compass Reverse
    COMP_REV_LEFT   = 0x8B, //Closed Loop With Compass Reverse Left Turn (90)
    COMP_REV_RIGHT  = 0x8C, //Closed Loop With Compass Reverse Right Turn (90)      
            
    //Closed Loop with Encoders states
    ECDR_FORWARD    = 0x8D, //Closed Loop With Encoders Forward drive
    ECDR_LEFT       = 0x8E, //Closed Loop With Encoders Forwards Left turn (90)
    ECDR_RIGHT      = 0x8F, //Closed Loop With Encoders Forwards Right turn (90)
    ECDR_REVERSE    = 0x90, //Closed Loop With Encoders Reverse
    ECDR_REV_LEFT   = 0x91, //Closed Loop With Encoders Reverse Left Turn (90)
    ECDR_REV_RIGHT  = 0x92, //Closed Loop With Encoders Reverse Right Turn (90) 
            
    //Closed Loop with Analog Sensors states
    PSNS_FORWARD    = 0x93, //Closed Loop With PhotoSensors Forward drive
    PSNS_LEFT       = 0x94, //Closed Loop With PhotoSensors Forwards Left turn (90)
    PSNS_RIGHT      = 0x95, //Closed Loop With PhotoSensors Forwards Right turn (90)
    PSNS_REVERSE    = 0x96, //Closed Loop With PhotoSensors Reverse
    PSNS_REV_LEFT   = 0x97, //Closed Loop With PhotoSensors Reverse Left Turn (90)
    PSNS_REV_RIGHT  = 0x98, //Closed Loop With PhotoSensors Reverse Right Turn (90)     
} state_t;

//enumerated type for various commands
typedef enum {
    //Grabber commands
    OPEN_GRABBER = 0x01,
    CLOSE_GRABBER = 0x02,
    READ_GRABBER = 0x03,
            
    //Motor Commands
    WRITE_MOTOR_LEFT = 0x04,
    READ_MOTOR_LEFT = 0x05,
    WRITE_MOTOR_RIGHT = 0x06,
    READ_MOTOR_RIGHT = 0x07,
            
    //Encoder Commands
    READ_ECDR1      = 0x08,
    READ_ECDR1_SUM  = 0x09,
    READ_ECDR2      = 0x0A,
    READ_ECDR2_SUM  = 0x0B,
            
    //Compass commands
    READ_COMP = 0x0C,

    //Photosensor commands
    READ_PSNS1 = 0x0D,
    READ_PSNS2 = 0x0E,
    READ_PSNS3 = 0x0F,
    READ_PSNS4 = 0x10,
    READ_PSNS5 = 0x11,
    READ_PSNS6 = 0x12,
    READ_PSNSFNT = 0x13,
    READ_PSNSCBE = 0x14,
            
    //Operate LED
    WRITE_LED = 0x15,
            
    //Read DIP Switches
    READ_DIP = 0x16
} command_t;

typedef enum {
    TIME, DISTANCE, JUNCTION, NONE
} condition_t;

typedef struct {
    condition_t stop;
    unsigned int value;
} state_conditions_t; 

typedef enum {
    STATE_CONTINUE,
    STATE_BREAK
} boolean_breakstate;

/* ----------------------------------------------------------------------------
 * Struct for storage of all variables that need to be tracked throughout 
 * state lifetime.
 */
typedef struct 
{
    //----------------------------------DATA------------------------------------
    unsigned int update_counter;        // increment counter on every update
    boolean_breakstate general_break_condition; 
    
    pid_ctrl Controller1;               
     
    // unsigned int psns_prev_samples[400];
    unsigned int psns_samples[4];
    
    unsigned int wheelencL_count;
    unsigned int wheelencR_count;
    
    unsigned int wheelencL_limit;
    unsigned int wheelencR_limit;
    
    unsigned int motor_L_desiredspeed;
    unsigned int motor_R_desiredspeed;
    unsigned int motor_dualspeed;
    
    motor_direction_type motor_L_direction;
    motor_direction_type motor_R_direction;
    motor_direction_type motor_dualdirection;
    
    int psnscurrentheading;
    int psns_desiredheading;
    
    float compass_currentheading;   // update on measurement
    float compass_desiredheading;   // determined at start of state
    
    int psns_compass_currentheading;
    int psns_compass_desiredheading;
    
    //-------------------------------POINTERS-----------------------------------
    
    unsigned int * update_counter_ptr;
    boolean_breakstate * general_break_condition_ptr;
    
    pid_ctrl * pid_ctrl_ptr;
    
    unsigned int * psns_prev_samples_ptr;
    unsigned int * psns_samples_ptr;
    
    unsigned int * wheelencL_count_ptr;
    unsigned int * wheelencR_count_ptr;
    
    unsigned int * wheelencL_limit_ptr;
    unsigned int * wheelencR_limit_ptr;
    
    unsigned int * motor_L_desiredspeed_ptr;
    unsigned int * motor_R_desiredspeed_ptr;
    unsigned int * motor_dualspeed_ptr;
    
    motor_direction_type * motor_L_direction_ptr;
    motor_direction_type * motor_R_direction_ptr;
    motor_direction_type * motor_dualdirection_ptr;
    
    float * compass_currentheading_ptr;   // update on measurement
    float * compass_desiredheading_ptr;
    
    
} control_variables;

/* -----------------------------------------------------------------------------
 * Struct to locally store any break conditions for the current state.
 */ 
typedef struct
{
    
    
} break_conditions;

////////////////////////////////FUNCTION DECLARATIONS///////////////////////////

state_t get_curr_state();
state_t get_next_state();
state_t get_prev_state();
state_conditions_t get_conditions();

void set_curr_state(state_t state);
void set_next_state(state_t state);
void set_prev_state(state_t state);
void set_conditions(state_conditions_t conditions);

void state_handler();

#endif //debuggersonpi_pic_statemachine_h





