
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOPID_HID_H_
#define _EOPID_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOpid_hid.h
    @brief      This header file implements hidden interface to PID controller.
    @author     alessandro.scalzo@iit.it
    @date       23/03/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOpid.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------



// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOpid_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOpid_hid 
{
    float Kp;
    float Kd;
    float Ki;
    
    float Kff;
    float Kbemf;
    float Ktau;
    
    float Dn;
    float En;
    float In;
    float Imax;
    
    float xv0,xv1;
    float yv0,yv1;
    
    float A,B;
    
    float stiction_up_val;
    float stiction_down_val;
  
    float pwm_offset;
    float pwm_max;
    float pwm;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




