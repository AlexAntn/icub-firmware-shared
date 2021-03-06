/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EONTHESYSTEM_H_
#define _EONTHESYSTEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EONtheSystem.h
    @brief      This header file implements public interface to the system singleton.
    @author     marco.accame@iit.it
    @date       11/21/2013
**/

/** @defgroup eos_thesystem Singleton EONtheSystem
    The EONtheSystem is derived from abstract object EOVtheSystem     
    bla bla bla    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"


// - public #define  --------------------------------------------------------------------------------------------------



// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct eOssystem_cfg_t
    @brief      eOssystem_cfg_t contains the specification for the system singleton in the single-task execution
                environment (SEE).
 **/  
typedef struct
{
    uint8_t                     tobedefined;
} eOnsystem_cfg_t;


/** @typedef    typedef struct EONtheSystem_hid EONtheSystem
    @brief      EONtheSystem is an opaque struct. It is used to implement data abstraction for the timer manager 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EONtheSystem_hid EONtheSystem;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOnsystem_cfg_t eon_system_cfg_default; // = {0}; 


// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern EONtheSystem * eon_sys_Initialise(const eOssystem_cfg_t *syscfg)

    @brief      Initialise the singleton EONtheSystem and all the other entities/objects that are required by the system:
                the memory pool, the error manager. The timer manager is started separately.
    @param      syscfg          The configuration.
    @return     A not NULL handle to the singleton.  In case of errors it is called the EOtheErrorManager
 
 **/
extern EONtheSystem * eon_sys_Initialise(const eOnsystem_cfg_t *syscfg,
                                         const eOmempool_cfg_t *mpoolcfg, 
                                         const eOerrman_cfg_t *errmancfg);


extern eOabstime_t eon_sys_LifeTimeGet(EONtheSystem *p);

extern eOresult_t eon_sys_LifeTimeSet(EONtheSystem *p, eOabstime_t ltime);

extern eOresult_t eon_sys_NanoTimeGet(EONtheSystem *p, eOnanotime_t *nt);
 
 
/** @fn         extern EONtheSystem* eon_sys_GetHandle(void)
    @brief      Returns an handle to the singleton EONtheSystem. The singleton must have been initialised otherwise 
                this function call will return NULL.
    @return     The pointer to the required EONtheSystem (or NULL upon in-initialised singleton).
 **/
extern EONtheSystem* eon_sys_GetHandle(void);


/** @fn         extern void eon_sys_Start(EONtheSystem *p, const void *cfg, eOvoid_fp_void_t init_fn)
    @brief      It starts EONtheSystem.  The singleton must have been initialised otherwise 
                this function call will issue a fatal error to the EOtheErrorManager.
    @param      p               The handler to the singleton.
    @param      userinit_fn     The function executed at startup just before the system is activated (which in
                                the SEE means just before the systick is started and the foop enters in its forever loop).
    @return     The function does not return and the execution stays in there forever.
 **/
extern void eon_sys_Start(EONtheSystem *p, eOvoid_fp_void_t userinit_fn);



/** @}            
    end of group eos_thesystem  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



