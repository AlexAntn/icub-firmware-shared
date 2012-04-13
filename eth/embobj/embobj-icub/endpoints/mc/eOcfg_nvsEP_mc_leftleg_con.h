/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#ifndef _EOCFG_NVSEP_MC_LEFTLEG_CON_H_
#define _EOCFG_NVSEP_MC_LEFTLEG_CON_H_




/** @file       eOcfg_nvsEP_mc_leftleg_con.h
	@brief      This header file gives the constant configuration for the NVs in the endpoint left leg
	@author     marco.accame@iit.it
	@date       04/05/2012
**/

/** @defgroup eo_asfdgr234 Configuation of the NVs for the updater
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EOconstvector.h"
#include "EOarray.h"
#include "EOnv.h"

#include "EoMotionControl.h"

 

// - public #define  --------------------------------------------------------------------------------------------------


// the 16bit value to use to identify the endpoint
#define EOK_cfg_nvsEP_mc_leftleg_endpoint                           (0x0010)




// -- the fun and typ of all the nv in the endpoint are in eOcfg_nvsEP_mc_jxx_con.h and eOcfg_nvsEP_mc_mxx_con.h
// -- the identifiers of all the nv in the endpoint are in eOcfg_nvsEP_mc_jxx_con.h and eOcfg_nvsEP_mc_mxx_con.h
// -- there are 12 nvid for each of the 6 joints. the nvids are a function of the joint number j = [0, 5]
// -- there are 6 nvid for each of the 6 motors. the nvids are a function of the motor number m = [0, 5]

#include "eOcfg_nvsEP_mc_jxx_con.h"


#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig(jth)                                  EOK_cfg_nvsEP_mc_jxx_NVID_jconfig(jth)
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__pidposition(jth)                     EOK_cfg_nvsEP_mc_NVID_jconfig__pidposition(jth)                       
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__pidvelocity(jth)                     EOK_cfg_nvsEP_mc_NVID_jconfig__pidvelocity(jth)                      
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__pidtorque(jth)                       EOK_cfg_nvsEP_mc_NVID_jconfig__pidtorque(jth)       
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__minpositionofjoint(jth)              EOK_cfg_nvsEP_mc_NVID_jconfig__minpositionofjoint(jth)       
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__maxpositionofjoint(jth)              EOK_cfg_nvsEP_mc_NVID_jconfig__maxpositionofjoint(jth)       
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__velocitysetpointtimeout(jth)         EOK_cfg_nvsEP_mc_NVID_jconfig__velocitysetpointtimeout(jth)    
#define EOK_cfg_nvsEP_mc_leftleg_NVID_jconfig__upto02descrforchameleon02(jth)       EOK_cfg_nvsEP_mc_NVID_jconfig__upto02descrforchameleon02(jth)        
#define EOK_cfg_nvsEP_mc_leftleg_NVID__jstatus(jth)                                 EOK_cfg_nvsEP_mc_NVID__jstatus(jth)                  
#define EOK_cfg_nvsEP_mc_leftleg_NVID__calibrator(jth)                              EOK_cfg_nvsEP_mc_NVID__calibrator(jth)                              
#define EOK_cfg_nvsEP_mc_leftleg_NVID__setpoint(jth)                                EOK_cfg_nvsEP_mc_NVID__setpoint(jth)                                
#define EOK_cfg_nvsEP_mc_leftleg_NVID__controlmode(jth)                             EOK_cfg_nvsEP_mc_NVID__controlmode(jth)                           





// - declaration of public user-defined types ------------------------------------------------------------------------- 



// - very important information: on ARM alignment is done using "#pragma pack(8)", thus ... there are rules to follow

typedef struct               
{
    eOmc_joint_t                joints[6]; 
    eOmc_motor_t                motors[6];
} eo_cfg_nvsEP_mc_leftleg_t;    EO_VERIFYsizeof(eo_cfg_nvsEP_mc_leftleg_t, 864);

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

//extern const eo_cfg_nvsEP_mc_leftleg_t eo_cfg_nvsEP_mc_leftleg_default;

// EOconstvector where each element is a EOtreenode whose data field is a EOnv_con_t object (id, capacity, valuedef, offset)
extern const EOconstvector* const eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con;

// if not NULL it contains a mapping from IDs to index inside eo_cfg_nvsEP_mc_leftleg_constvector_of_treenodes_EOnv_con
extern const eOuint16_fp_uint16_t eo_cfg_nvsEP_mc_leftleg_fptr_hashfunction_id2index;



// - declaration of extern public functions ---------------------------------------------------------------------------

extern uint16_t eo_cfg_nvsEP_mc_leftleg_hashfunction_id2index(uint16_t id);





/** @}            
    end of group eo_asfdgr234  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







