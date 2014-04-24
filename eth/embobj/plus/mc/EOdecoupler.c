
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOemsControllerCfg.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define ZERO_ROTATION_TORQUE 0

#define CABLE_WARNING_x_100  2000

#define MOTORS(m) for (uint8_t m=0; m<o->n_motors; ++m)



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotors* eo_motors_New(uint8_t n_motors) 
{
    if (!n_motors) return NULL;
    
    EOmotors *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotors), 1);

    if (o)
    {
        o->motor_idle_mask = 0x00;
        
        for (uint8_t m=0; m<MAX_MOTORS; ++m)
        {
            o->zero_rotation_torque[m] = ZERO_ROTATION_TORQUE;
        }
        
        if (n_motors > MAX_MOTORS) n_motors = MAX_MOTORS;
        
        o->n_motors = n_motors;
    }

    return o;
}

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, eObool_t bError, eOmc_controlmode_t control_mode)
{
    if (!o) return;
    
    if (bError /*|| (control_mode == eomc_ctrlmval_idle)*/)
    {
        o->motor_idle_mask |=  (1<<m);
    }
    else
    {
        o->motor_idle_mask &= ~(1<<m);
    }
}

//             | 1     0       0   |
// J = dq/dm = | 1   40/65     0   |
//             | 0  -40/65   40/65 | 

//        |    1       0       0   |
// J^-1 = | -65/40   65/40     0   |
//        | -65/40   65/40   65/40 |

//      | 1     1       0   |
// Jt = | 0   40/65  -40/65 |
//      | 0     0     40/65 | 

// speed_motor  = J^-1 * speed_axis
// torque_motor = Jt   * torque_axis

extern uint8_t eo_motors_PWM(EOmotors *o, int32_t *pwm_joint, int16_t *pwm_motor, eObool_t* stiff)
{
    if (!o) return 0xFF;
    
    uint8_t stop_mask = 0;
    
    #if defined(SHOULDER_BOARD)
        if (o->motor_idle_mask & 0x07)
        {
            stop_mask |= 0x07;
            pwm_motor[0] = pwm_motor[1] = pwm_motor[2] = 0;
        }
        else
        {
            int16_t buff;
            
            pwm_motor[0] = (int16_t)(pwm_joint[0]);
            
            if (stiff[0])
            {    
                pwm_motor[1] = pwm_motor[2] = (int16_t)((-65*pwm_joint[0])/40);
            }
            else
            {
                pwm_motor[1] = pwm_motor[2] = 0;
            }
            
            if (stiff[1])
            {
                buff = (int16_t)((65*pwm_joint[1])/40);
                pwm_motor[1] += buff;
                pwm_motor[2] += buff;
            }
            else
            {
                pwm_motor[0] += (int16_t)(pwm_joint[1]);
                pwm_motor[1] += (int16_t)((40*pwm_joint[1])/65);
            }
            
            if (stiff[2])
            {
                pwm_motor[2] += (int16_t)((65*pwm_joint[2])/40);
            }
            else
            {
                buff = (int16_t)((40*pwm_joint[2])/65);
                pwm_motor[1] -= buff;
                pwm_motor[2] += buff;
            }
            
            //pwm_motor[0] = (int16_t)(stiff[0] ? (pwm_joint[0]) : (pwm_joint[0]+pwm_joint[1]));
            //pwm_motor[1] = (int16_t)(stiff[1] ? ((65*(-pwm_joint[0]+pwm_joint[1]))/40) : ((40*(pwm_joint[1]-pwm_joint[2]))/65));
            //pwm_motor[2] = (int16_t)(stiff[2] ? ((65*(-pwm_joint[0]+pwm_joint[1]+pwm_joint[2]))/40) : ((40*pwm_joint[2] )/65));
        }
        
        if (o->motor_idle_mask & 0x08)
        {            
            stop_mask |= 0x08;
            pwm_motor[3] = 0;
        }
        else
        {   
            pwm_motor[3] = (int16_t)pwm_joint[3];
        }
    #elif defined(WAIST_BOARD)
        if (o->motor_idle_mask & 0x07)
        {
            stop_mask |= 0x07;
            pwm_motor[0] = pwm_motor[1] = pwm_motor[2] = 0;
        }
        else
        {
            pwm_motor[0] = (int16_t)((pwm_joint[0]-pwm_joint[1])/2);
            pwm_motor[1] = (int16_t)((pwm_joint[0]+pwm_joint[1])/2);
            pwm_motor[2] = (int16_t)  pwm_joint[2];
        }
    #elif defined(UPPERLEG_BOARD)
        if (o->motor_idle_mask & 0x01)
        {
            stop_mask |= 0x01;
            pwm_motor[0] = 0;
        }
        else
        {
            pwm_motor[0] = pwm_joint[0];
        }
        
        if (o->motor_idle_mask & 0x02)
        {
            stop_mask |= 0x02;
            pwm_motor[1] = 0;
        }
        else
        {
            pwm_motor[1] = pwm_joint[1];
        }
        
        if (o->motor_idle_mask & 0x04)
        {
            stop_mask |= 0x04;
            pwm_motor[2] = 0;
        }
        else
        {
            pwm_motor[2] = pwm_joint[2];
        }
        
        if (o->motor_idle_mask & 0x08)
        {
            stop_mask |= 0x08;
            pwm_motor[3] = 0;
        }
        else
        {
            pwm_motor[3] = pwm_joint[3];
        }
    #elif defined(ANKLE_BOARD)
        if (o->motor_idle_mask & 0x01)
        {
            stop_mask |= 0x01;
            pwm_motor[0] = 0;
        }
        else
        {
            pwm_motor[0] = pwm_joint[0];
        }
        
        if (o->motor_idle_mask & 0x02)
        {
            stop_mask |= 0x02;
            pwm_motor[1] = 0;
        }
        else
        {
            pwm_motor[1] = pwm_joint[1];
        }
    #else
        MOTORS(m) pwm_motor[m] = 0;
    #endif
            
    MOTORS(m) LIMIT(pwm_motor[m], NOMINAL_CURRENT);
    
    return stop_mask;
}

extern eObool_t eo_motors_CableLimitAlarm(int32_t j0, int32_t j1, int32_t j2)
{
    int32_t cond = 171*(j0-j1);
    
    if (cond < -34700 + CABLE_WARNING_x_100) return eobool_true;
 
    cond -= 171*j2;
    
    if (cond < -36657 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  11242 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j1+j2;
    
    if (cond <  -6660 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  21330 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j0;
    
    if (cond <  -9600 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >    500 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j1;
    
    if (cond <          CABLE_WARNING_x_100) return eobool_true;
    if (cond >  19500 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j2;
    
    if (cond < -9000 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  9000 - CABLE_WARNING_x_100) return eobool_true;
    
    return eobool_false;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




