/* 
 * File:   states.c
 * Author: Harry
 *
 * Created on February 20, 2016, 10:05 PM
 */

/////////////////////////////////////INCLUDES///////////////////////////////////

#include "states.h"

/////////////////////////////////////DEFINES////////////////////////////////////

#define speed_slow 150
#define speed_med 300
#define speed_fast 450

// PID controller params
#define kp_linetracking 1
#define ki_linetracking 1
#define kp_turning
#define ki_turning
#define ctrl_max     200 
#define ctrl_min    -200

//////////////////////////////TYPEDEFS,ENUMS,STRUCTS////////////////////////////

//////////////////////////////////GLOBAL VARIABLES//////////////////////////////

///////////////////////////////FUNCTION DEFINITIONS/////////////////////////////

/* -----------------------------------------------------------------------------
 * Function: state_switch(control_variables local_state_vars)
 * 
 * This function is called when the poll_TMR triggers a new update, and switches
 * the appropriate control algorithm into the loop.
 * 
 * INPUTS: all the local state tracking variables from previous loop execution
 * 
 * OUTPUTS: tracking variables are updated
 */
void switch_statecontrol(control_variables * local_state_vars_ptr,
                         spi_state_data * local_state_data_ptr,
                         spi_data_out * local_spi_data_out_ptr)
{  
//execute a different function based on state
    switch (local_state_data_ptr->state)
    { 
        default:  // Default state is stopped/              
            state_stopped();        //Stationary
            break;

        case STOPPED:           
            state_stopped();        //Stationary
            break;

        //Open Loop States//////////////////////////////////////////////////////        
        case OL_FORWARD:                //Open Loop Forward drive       
            state_ol_forward(local_state_vars_ptr);    
            break;
//        case OL_LEFT:                   //Open Loop Forwards Left turn (90)         
//            state_ol_left(control_variables * local_state_vars_ptr);        
//            break;
//        case OL_RIGHT:                  //Open Loop Forwards Right turn (90)         
//            state_ol_right(control_variables * local_state_vars_ptr);       
//            break;
        case OL_REVERSE:                //Open Loop Reverse   
            state_ol_reverse(local_state_vars_ptr);     
            break;
//        case OL_REV_LEFT:               //Open Loop Reverse Left Turn (90)     
//            state_ol_rev_left(control_variables * local_state_vars_ptr);    
//            break;
//        case OL_REV_RIGHT:              //Open Loop Reverse Right Turn (90)
//            state_ol_rev_right(control_variables * local_state_vars_ptr);   
//            break;

        //Closed Loop with Compass states///////////////////////////////////////
        case COMP_FORWARD:              //Closed Loop With Compass Forward drive 
            state_comp_forward(local_state_vars_ptr);   
            break;
        case COMP_LEFT:                 //Closed Loop With Compass Turn angle  
            state_comp_turn(local_state_vars_ptr);      
            break;
        case COMP_RIGHT:                 //Closed Loop With Compass Turn angle  
            state_comp_turn(local_state_vars_ptr);      
            break;    
        case COMP_REVERSE:              //Closed Loop With Compass Reverse   
            state_comp_reverse(local_state_vars_ptr);   
            break;
        case COMP_REV_LEFT:             //Closed Loop With Compass Reverse Left Turn (90) 
            state_comp_rev_left(local_state_vars_ptr);  
            break;
        case COMP_REV_RIGHT:            //Closed Loop With Compass Reverse Right Turn (90)
            state_comp_rev_right(local_state_vars_ptr); 
            break;

        //Closed Loop with Encoders states//////////////////////////////////////
        case ECDR_FORWARD:              //Closed Loop With Encoders Forward drive
            state_ecdr_forward(local_state_vars_ptr);   
            break;
        case ECDR_LEFT:                 //Closed Loop With Encoders Forwards Left turn (90) 
            state_ecdr_left(local_state_vars_ptr);      
            break;
        case ECDR_RIGHT:                //Closed Loop With Encoders Forwards Right turn (90)
            state_ecdr_right(local_state_vars_ptr);     
            break;
        case ECDR_REVERSE:              //Closed Loop With Encoders Reverse   
            state_ecdr_reverse(local_state_vars_ptr);   
            break;
        case ECDR_REV_LEFT:             //Closed Loop With Encoders Reverse Left Turn (90)
            state_ecdr_rev_left(local_state_vars_ptr);  
            break;
        case ECDR_REV_RIGHT:            //Closed Loop With Encoders Reverse Right Turn (90)
            state_ecdr_rev_right(local_state_vars_ptr); 
            break;

        //Closed Loop with Analog Sensors states////////////////////////////////
        case PSNS_FORWARD:              //Closed Loop With PhotoSensors Forward drive   
            state_psns_forward(local_state_vars_ptr);   
            break;
        //Closed Loop with Analog Sensors states////////////////////////////////
        case PSNS_FORWARD_JUNCTION_DETECT:              //Closed Loop With PhotoSensors and detecting junction   
            state_psns_forward(local_state_vars_ptr);   
            break;  
//        
    } // switch(Curr_State)
}

/* -----------------------------------------------------------------------------
 * Function: state_stopped()
 * 
 * Placeholder function for a stopped state
 * 
 * INPTUTS: none
 * 
 * OUTPUTS: none
 */
void state_stopped(){};

/* -----------------------------------------------------------------------------
 * Function: state_ol_forward(control_variables local_state_vars)
 * 
 * Updates the robot drive to drive forwards in a straight line. 
 * 
 * INPUTS: Local state tracking variables
 * 
 * OUTPUTS: none
 */
void state_ol_forward(control_variables * local_state_vars_ptr)
{
    motors_dual_constspeed(FWD,
                           local_state_vars_ptr->motor_dualspeed);
}

/* -----------------------------------------------------------------------------
 * Function: state_ecdr_left(control_variables * local_state_vars_ptr)
 * 
 * Uses open-loop control to execute a 90 degree turn to the left. The speed of 
 * the turn is controlled by the local_state_vars struct.
 * This function triggers the statehandler general break condition.
 * 
 * INPTUS: local_state_vars struct
 * 
 * OUTPUTS: none
 *
*/
void state_ecdr_left(control_variables * local_state_vars_ptr)
{
    // if encoder count not reached, keep driving wheel
    if (local_state_vars_ptr->wheelencL_count < local_state_vars_ptr->wheelencL_limit) 
    {
        L_motor_constSpeed(REV,    // left mtr reverse
                           local_state_vars_ptr->motor_L_desiredspeed);
    }
   
    if (local_state_vars_ptr->wheelencR_count < local_state_vars_ptr->wheelencR_limit)
    {
        R_motor_constSpeed(FWD,    // right mtr forward
                           local_state_vars_ptr->motor_R_desiredspeed);
    }
    
    // if both wheels reached desired count, trigger break condition
    if (local_state_vars_ptr->wheelencL_count > local_state_vars_ptr->wheelencL_limit &&
        local_state_vars_ptr->wheelencR_count > local_state_vars_ptr->wheelencR_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    }; 
}

/* -----------------------------------------------------------------------------
 * Function: state_ol_right(control_variables * local_state_vars_ptr)
 * 
 * Uses open-loop control to execute a 90 degree turn to the right. The speed of 
 * the turn is controlled by the local_state_vars struct.
 * This function triggers the statehandler general break condition.
 * 
 * INPTUS: local_state_vars struct
 * 
 * OUTPUTS: none
 *
*/
void state_ecdr_right(control_variables * local_state_vars_ptr)
{
    // if encoder count not reached, keep driving wheel
    if (local_state_vars_ptr->wheelencL_count < local_state_vars_ptr->wheelencL_limit) 
    {
        L_motor_constSpeed(FWD,    // left mtr forward
                           local_state_vars_ptr->motor_L_desiredspeed);
    }
   
    if (local_state_vars_ptr->wheelencR_count < local_state_vars_ptr->wheelencR_limit)
    {
        R_motor_constSpeed(REV,    // right mtr reverse
                           local_state_vars_ptr->motor_R_desiredspeed);
    }
    
    // if both wheels reached desired count, trigger break condition
    if (local_state_vars_ptr->wheelencL_count > local_state_vars_ptr->wheelencL_limit &&
        local_state_vars_ptr->wheelencR_count > local_state_vars_ptr->wheelencR_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    }; 
}

/* -----------------------------------------------------------------------------
 * Function: state_ol_reverse(control_variables * local_state_vars_ptr)
 * 
 * Open loop reverse in a straight line
 * 
 * INPUTS: local_state_vars
 * 
 * OUTPUTS: none
 */
void state_ol_reverse(control_variables * local_state_vars_ptr)
{
    motors_dual_constspeed(REV,
                           local_state_vars_ptr->motor_dualspeed);
}

/* -----------------------------------------------------------------------------
 * Function: state_ecdr_rev_left(control_variables * local_state_vars_ptr)
 * 
 * Open loop reverse and turn left 90 deg.
 * This function triggers the statehandler general break condition.
 * 
 * INPUTS: local_state_vars
 * 
 * OUTPUTS: none
 */
void state_ecdr_rev_left(control_variables * local_state_vars_ptr)
{
    if (local_state_vars_ptr->wheelencR_count < local_state_vars_ptr->wheelencR_limit)
    {
        R_motor_constSpeed(REV,    // right mtr reverse
                           local_state_vars_ptr->motor_R_desiredspeed);
    }
    
    // if wheel reaches desired count, trigger break condition
    if (local_state_vars_ptr->wheelencR_count > local_state_vars_ptr->wheelencR_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    }; 
}

/* -----------------------------------------------------------------------------
 * Function: state_ol_rev_right(control_variables * local_state_vars_ptr)
 * 
 * Open loop reverse and turn right 90 deg.
 * This function triggers the statehandler general break condition.
 * 
 * INPUTS: local_state_vars
 * 
 * OUTPUTS: none
 */
void state_ecdr_rev_right(control_variables * local_state_vars_ptr)
{
    if (local_state_vars_ptr->wheelencL_count < local_state_vars_ptr->wheelencL_limit)
    {
        L_motor_constSpeed(REV,    // right mtr reverse
                           local_state_vars_ptr->motor_L_desiredspeed);
    }
    
    // if wheel reaches desired count, trigger break condition
    if (local_state_vars_ptr->wheelencL_count > local_state_vars_ptr->wheelencL_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    }; 
}

/* -----------------------------------------------------------------------------
 * Function: state_comp_forward(control_variables * local_state_vars_ptr);
 * 
 * Drives forward using compass as angular feedback.
 * 
 * INPUTS: local_state_vars
 * 
 * OUTPUTS: none
 */
void state_comp_forward(control_variables * local_state_vars_ptr)
{
    // pid setpoint (desired angle) is set in structure when initialised
    // pid_update calculates a new control variable and writes it into struct
    pid_update(local_state_vars_ptr->compass_currentheading,
               local_state_vars_ptr->pid_ctrl_ptr);
    
    pid_updatemotors_fwd(local_state_vars_ptr);
}

/* -----------------------------------------------------------------------------
 * Function: state_comp_turn(control_variables * local_state_vars_ptr)
 * 
 * Makes a turn using the compass as angular feedback.
 * 
 * INPUTS: local_state_vars_ptr 
 * 
 * OUTPUTS: none
 */
void state_comp_turn(control_variables * local_state_vars_ptr)
{
    // pid setpoint (desired angle) is set in structure when initialised
    // pid_update calculates a new control variable and writes it into struct
    pid_update(local_state_vars_ptr->compass_currentheading,
               local_state_vars_ptr->pid_ctrl_ptr);
    
    // update motors
    pid_updatemotors_turn(local_state_vars_ptr);
    
    // generate break condition if error is small. 
}

/* -----------------------------------------------------------------------------
 * Function: state_comp_reverse(control_variables * local_state_vars_ptr)
 * 
 * Reverse in a straight line using the compass as angular feedback.
 * Use a forward heading for the PID, but set the motors in reverse. 
 * Then, the steering inputs are reversed.
 * 
 * INPUTS: local_state_vars_ptr 
 * 
 * OUTPUTS: none
 */
void state_comp_reverse(control_variables * local_state_vars_ptr)
{
    // pid setpoint (desired angle) is set in structure when initialised
    // pid_update calculates a new control variable and writes it into struct
    pid_update(local_state_vars_ptr->compass_currentheading,
               local_state_vars_ptr->pid_ctrl_ptr);
    
    pid_updatemotors_rev(local_state_vars_ptr);
    
    // generate break condition if error is small.
}

/* -----------------------------------------------------------------------------
 * Function: state_comp_rev_left(control_variables * local_state_vars_ptr)
 * 
 * Reverse to the left 90 degrees, and use the compass as angular feedback.
 * Rotate/turn by rotating one wheel only.
 * 
 * INPUTS: local_state_vars_ptr
 * 
 * OUTPUTS: none
 */
void state_comp_rev_left(control_variables * local_state_vars_ptr)
{
    // pid setpoint (desired angle) is set in structure when initialised
    // pid_update calculates a new control variable and writes it into struct
    pid_update(local_state_vars_ptr->compass_currentheading,
               local_state_vars_ptr->pid_ctrl_ptr);
    
    // update motors
    R_motor_acceltoconstSpeed(local_state_vars_ptr->update_counter,
                              REV, 
                              (local_state_vars_ptr->motor_dualspeed + local_state_vars_ptr->Controller1.cv));

    // generate break condition if error is small.
}

/* -----------------------------------------------------------------------------
 * Function: state_comp_rev_right(control_variables * local_state_vars_ptr)
 * 
 * Reverse to the right 90 degrees, and use the compass as angular feedback.
 * 
 * INPUTS: local_state_vars_ptr
 * 
 * OUTPUTS: none
 */
void state_comp_rev_right(control_variables * local_state_vars_ptr)
{
    // pid setpoint (desired angle) is set in structure when initialised
    // pid_update calculates a new control variable and writes it into struct
    pid_update(local_state_vars_ptr->compass_currentheading,
               local_state_vars_ptr->pid_ctrl_ptr);
    
    // update motors
    L_motor_acceltoconstSpeed(local_state_vars_ptr->update_counter,
                              REV, 
                              (local_state_vars_ptr->motor_dualspeed + local_state_vars_ptr->Controller1.cv));

    // generate break condition if error is small.
}

/* -----------------------------------------------------------------------------
 * Function: state_ecdr_forward(control_variables * local_state_vars_ptr)
 * 
 * Drive forward open_loop, until a count limit is reached.
 * 
 * INPTUS: local_state_vars_ptr
 * 
 * OUTPUTS: none 
 */
void state_ecdr_forward(control_variables * local_state_vars_ptr)
{
    // use the open-loop forward state
    state_ol_forward(local_state_vars_ptr);
            
    // set break condition when count limit is reached
    if (local_state_vars_ptr->wheelencL_count > local_state_vars_ptr->wheelencL_limit ||
        local_state_vars_ptr->wheelencR_count > local_state_vars_ptr->wheelencR_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    };
}
        
/* -----------------------------------------------------------------------------
 * Function: state_ecdr_reverse(control_variables * local_state_vars_ptr)
 * 
 * Reverse open-loop, until a count limit is reached.
 * 
 * INPUTS: local_state_vars_ptr
 * 
 * OUTPUTS: none
 */
void state_ecdr_reverse(control_variables * local_state_vars_ptr)
{
    // use the open-loop reverse state
    state_ol_reverse(local_state_vars_ptr);
    
    // set break condition when count limit is reached
    if (local_state_vars_ptr->wheelencL_count > local_state_vars_ptr->wheelencL_limit ||
        local_state_vars_ptr->wheelencR_count > local_state_vars_ptr->wheelencR_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    };
}

/* -----------------------------------------------------------------------------
 * Function: state_psns_forward(control_variables * local_state_vars_ptr); 
 * 
 * Moves the robot forward, while using the photosensors to estimate
 * the angular deviation, and a PID controller to correct the error.
 * 
 * INPUTS: local_state_vars_ptr
 * 
 * OUTPUT: none
 */
void state_psns_forward(control_variables * local_state_vars_ptr)
{
    // ADC sample data is already in buffer
    
    signed int diffs[2] = {0};
    signed int * diffs_ptr = diffs;
    
    // calculate two differences
    linefollow_calcpairdiffs(local_state_vars_ptr->psns_adc_samples, diffs_ptr);
    
    // estimate angle from differences
    signed int psns_estimate_angle = 0;
    psns_estimate_angle = linefollow_estimateangle_ldr(diffs_ptr);

    // input new angular error estimate into PID controller
    pid_update(psns_estimate_angle,
               local_state_vars_ptr->pid_ctrl_ptr);
    
    // update motors based on new cv
    pid_updatemotors_fwd(local_state_vars_ptr);
    
    // generate break condition when encoder count is reached.
    if (local_state_vars_ptr->wheelencL_count > local_state_vars_ptr->wheelencL_limit ||
        local_state_vars_ptr->wheelencR_count > local_state_vars_ptr->wheelencR_limit)
    {
        local_state_vars_ptr->general_break_condition = STATE_BREAK; 
        return;
    };
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
/* -----------------------------------------------------------------------------
 * Function: state_switch(control_variables local_state_vars)
 * 
 * This function is called when the poll_TMR triggers a new update, and switches
 * the appropriate control algorithm into the loop.
 * 
 * INPUTS: spi_newstate, state_vars and state_break_condition structs.
 * 
 * OUTPUTS: initialise state_vars and state_break_condition structs.
 */
void switch_statesetup(control_variables * local_state_vars_ptr,
                       spi_state_data * local_currentstate_data_ptr)                     
{  
//execute a different function based on state
    switch (local_currentstate_data_ptr->state)
    { 
        default:  // Default state is stopped/              
            state_stopped();        //Stationary
            break;

        case STOPPED:           
            state_stopped();        //Stationary
            break;

        //Open Loop States//////////////////////////////////////////////////////        
        case OL_FORWARD:                //Open Loop Forward drive       
            setup_state_ol_forward(local_currentstate_data_ptr,
                                   local_state_vars_ptr);    
            break;
        case OL_REVERSE:                //Open Loop Reverse   
            setup_state_ol_reverse(local_currentstate_data_ptr,
                                   local_state_vars_ptr);     
            break;

        //Closed Loop with Compass states///////////////////////////////////////
        case COMP_FORWARD:              //Closed Loop With Compass Forward drive 
            setup_state_comp_forward(local_currentstate_data_ptr,
                                     local_state_vars_ptr);  
            break;
        case COMP_LEFT:                 //Closed Loop With Compass Turn angle  
            setup_state_comp_turn(local_currentstate_data_ptr,
                                  local_state_vars_ptr);      
            break;
        case COMP_RIGHT:                 //Closed Loop With Compass Turn angle  
            setup_state_comp_turn(local_currentstate_data_ptr,
                                  local_state_vars_ptr);      
            break;    
        case COMP_REVERSE:              //Closed Loop With Compass Reverse   
            setup_state_comp_reverse(local_currentstate_data_ptr,
                                     local_state_vars_ptr); 
            break;
        case COMP_REV_LEFT:             //Closed Loop With Compass Reverse Left Turn (90) 
            setup_state_comp_rev_left(local_currentstate_data_ptr,
                                      local_state_vars_ptr);  
            break;
        case COMP_REV_RIGHT:            //Closed Loop With Compass Reverse Right Turn (90)
            setup_state_comp_rev_right(local_currentstate_data_ptr,
                                       local_state_vars_ptr);
            break;

        //Closed Loop with Encoders states//////////////////////////////////////
        case ECDR_FORWARD:              //Closed Loop With Encoders Forward drive
            setup_state_ecdr_forward(local_currentstate_data_ptr,
                                     local_state_vars_ptr);   
            break;
        case ECDR_LEFT:                 //Closed Loop With Encoders Forwards Left turn (90) 
            setup_state_ecdr_left(local_currentstate_data_ptr,
                                  local_state_vars_ptr);    
            break;
        case ECDR_RIGHT:                //Closed Loop With Encoders Forwards Right turn (90)
            setup_state_ecdr_right(local_currentstate_data_ptr,
                                   local_state_vars_ptr);    
            break;
        case ECDR_REVERSE:              //Closed Loop With Encoders Reverse   
            setup_state_ecdr_reverse(local_currentstate_data_ptr,
                                     local_state_vars_ptr);   
            break;
        case ECDR_REV_LEFT:             //Closed Loop With Encoders Reverse Left Turn (90)
            setup_state_ecdr_rev_left(local_currentstate_data_ptr,
                                      local_state_vars_ptr);  
            break;
        case ECDR_REV_RIGHT:            //Closed Loop With Encoders Reverse Right Turn (90)
            setup_state_ecdr_rev_right(local_currentstate_data_ptr,
                                       local_state_vars_ptr); 
            break;

        //Closed Loop with Analog Sensors states////////////////////////////////
        case PSNS_FORWARD:              //Closed Loop With PhotoSensors Forward drive   
            setup_state_psns_forward(local_currentstate_data_ptr,
                                     local_state_vars_ptr);   
            break;
        //Closed Loop with Analog Sensors states////////////////////////////////
        case PSNS_FORWARD_JUNCTION_DETECT:              //Closed Loop With PhotoSensors and detecting junction   
            setup_state_psns_forward_junctiondetect(local_currentstate_data_ptr,
                                                    local_state_vars_ptr);   
            break;  
    }   
} // switch(Curr_State)

/* -----------------------------------------------------------------------------
 * Function: setup_state_ol_forward(spi_state_data * local_currentstate_data_ptr,
 *                                  control_variables * local_state_vars_ptr);
 * 
 * Setups the local tracking structs for the state ol_forward.
 * 
 * INPUTS: spi_state_data * local_currentstate_data_ptr,
 *         control_variables * local_state_vars_ptr
 * 
 * OUTPUTS: none
 */
void setup_state_ol_forward(spi_state_data * local_currentstate_data_ptr,
                            control_variables * local_state_vars_ptr)
{
    // setup local data structures
    local_state_vars_ptr->motor_dualspeed = local_currentstate_data_ptr->state_data.value;
    local_state_vars_ptr->motor_dualdirection = FWD;
    
    // setup interrupts, enables, etc.
    enableMotorPSU();
    enc1_setupInterrupt();
    enc2_setupInterrupt();
}

void setup_state_ecdr_left(spi_state_data * local_currentstate_data_ptr,
                           control_variables * local_state_vars_ptr)
{
    
}

void setup_state_ecdr_right(spi_state_data * local_currentstate_data_ptr,
                            control_variables * local_state_vars_ptr)
{
    
}

void setup_state_ol_reverse(spi_state_data * local_currentstate_data_ptr,
                            control_variables * local_state_vars_ptr)
{
    
}

void setup_state_ecdr_rev_left(spi_state_data * local_currentstate_data_ptr,
                               control_variables * local_state_vars_ptr)
{
    
}

void setup_state_ecdr_rev_right(spi_state_data * local_currentstate_data_ptr,
                                control_variables * local_state_vars_ptr)
{
    
}

void setup_state_comp_forward(spi_state_data * local_currentstate_data_ptr,
                              control_variables * local_state_vars_ptr)
{
    
}

void setup_state_comp_turn(spi_state_data * local_currentstate_data_ptr,
                           control_variables * local_state_vars_ptr)
{
    
}

void setup_state_comp_reverse(spi_state_data * local_currentstate_data_ptr,
                              control_variables * local_state_vars_ptr)
{
    
}


void setup_state_comp_rev_left(spi_state_data * local_currentstate_data_ptr,
                               control_variables * local_state_vars_ptr)
{
    
}

void setup_state_comp_rev_right(spi_state_data * local_currentstate_data_ptr,
                                control_variables * local_state_vars_ptr)
{
    
}

void setup_state_ecdr_forward(spi_state_data * local_currentstate_data_ptr,
                              control_variables * local_state_vars_ptr)
{
    
}

void setup_state_ecdr_reverse(spi_state_data * local_currentstate_data_ptr,
                              control_variables * local_state_vars_ptr)
{
    
}

void setup_state_psns_forward(spi_state_data * local_currentstate_data_ptr,
                              control_variables * local_state_vars_ptr)
{
    // setup local data structures
    local_state_vars_ptr->motor_dualspeed = local_currentstate_data_ptr->state_data.value;
    local_state_vars_ptr->motor_dualdirection = FWD;
    
    local_state_vars_ptr->wheelencL_limit=0xFFFF;
    local_state_vars_ptr->wheelencR_limit=0xFFFF;
    
    local_state_vars_ptr->usepsns = 1;
    local_state_vars_ptr->psns_desiredheading = 0;
    
    local_state_vars_ptr->Controller1.kp = kp_linetracking;
    local_state_vars_ptr->Controller1.ki = ki_linetracking;
    local_state_vars_ptr->Controller1.max = ctrl_max;
    local_state_vars_ptr->Controller1.min = ctrl_min;
    
    // setup interrupts, enables, etc.
    enableMotorPSU();
    enc1_setupInterrupt();
    enc2_setupInterrupt();
}

void setup_state_psns_forward_junctiondetect(spi_state_data * local_currentstate_data_ptr,
                                             control_variables * local_state_vars_ptr)
{
    
}